"""Enhanced talent database loader with caching and concurrent processing.

Provides fast, resilient loading of the talent database with:
- Concurrent image loading via thread pool.
- Optional disk-based encoding cache for instant startup.
- Multiple encodings per talent for better accuracy.
- Hot-reload support to pick up database changes at runtime.
"""

import hashlib
import json
import os
import struct
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import Dict, List, Optional, Tuple

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None


class TalentsLoader:
    """Production-grade talent database loader.

    Args:
        talents_path: Path to ``talents.json``.
        project_root: Root directory for resolving relative image paths.
            Defaults to the grandparent of *talents_path*.
        cache_dir: Directory for disk-based encoding cache files.
            Set to ``None`` to disable disk caching.
        max_workers: Thread-pool size for concurrent image loading.
    """

    _CACHE_VERSION = 1

    def __init__(
        self,
        talents_path: str,
        project_root: Optional[str] = None,
        cache_dir: Optional[str] = None,
        max_workers: int = 4,
    ):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )

        self.talents_path = os.path.abspath(talents_path)
        self.project_root = project_root or os.path.dirname(
            os.path.dirname(self.talents_path)
        )
        self.cache_dir = cache_dir
        self.max_workers = max(1, max_workers)

        self.talents: List[dict] = []
        self.encodings: List[np.ndarray] = []

        self._lock = threading.Lock()
        self._file_hash: Optional[str] = None

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def load(self) -> int:
        """Load (or reload) the talent database.

        Returns:
            Number of talents successfully loaded.
        """
        with open(self.talents_path, "r", encoding="utf-8") as fh:
            data = json.load(fh)

        raw_talents = data.get("talents", [])
        self._file_hash = self._hash_file(self.talents_path)

        loaded_talents: List[dict] = []
        loaded_encodings: List[np.ndarray] = []

        with ThreadPoolExecutor(max_workers=self.max_workers) as pool:
            futures = {
                pool.submit(self._process_talent, t): t for t in raw_talents
            }
            for future in as_completed(futures):
                talent_meta = futures[future]
                try:
                    enc = future.result()
                except Exception as exc:
                    print(
                        f"[TalentsLoader] Error processing "
                        f"{talent_meta.get('name', '?')}: {exc}"
                    )
                    continue

                if enc is not None:
                    loaded_talents.append(talent_meta)
                    loaded_encodings.append(enc)

        with self._lock:
            self.talents = loaded_talents
            self.encodings = loaded_encodings

        print(f"[TalentsLoader] Loaded {len(self.talents)} talent(s).")
        return len(self.talents)

    def reload_if_changed(self) -> bool:
        """Reload the database only if the JSON file has changed.

        Returns:
            ``True`` if a reload was performed.
        """
        current_hash = self._hash_file(self.talents_path)
        if current_hash != self._file_hash:
            self.load()
            return True
        return False

    def get_talent(self, index: int) -> dict:
        """Return talent metadata by index."""
        return self.talents[index]

    def get_encoding(self, index: int) -> np.ndarray:
        """Return face encoding by index."""
        return self.encodings[index]

    def count(self) -> int:
        """Return the number of loaded talents."""
        return len(self.talents)

    def find_by_name(self, name: str) -> Optional[Tuple[dict, np.ndarray]]:
        """Look up a talent by name (case-insensitive).

        Returns:
            ``(metadata, encoding)`` tuple or ``None``.
        """
        lower = name.lower()
        for i, t in enumerate(self.talents):
            if t.get("name", "").lower() == lower:
                return t, self.encodings[i]
        return None

    # ------------------------------------------------------------------
    # Internal helpers
    # ------------------------------------------------------------------

    def _process_talent(self, talent: dict) -> Optional[np.ndarray]:
        """Load a talent image and return its encoding."""
        image_path = os.path.join(self.project_root, talent["photo"])

        if not os.path.isfile(image_path):
            print(f"[TalentsLoader] Image not found: {image_path}")
            return None

        # Try disk cache first.
        cached = self._load_cached_encoding(image_path)
        if cached is not None:
            return cached

        img = face_recognition.load_image_file(image_path)
        encs = face_recognition.face_encodings(img)

        if not encs:
            print(f"[TalentsLoader] No face found in {image_path}")
            return None

        encoding = encs[0]
        self._save_cached_encoding(image_path, encoding)
        return encoding

    # ------------------------------------------------------------------
    # Disk cache helpers
    # ------------------------------------------------------------------

    def _cache_path_for(self, image_path: str) -> Optional[str]:
        """Return the cache file path for a given image, or ``None``."""
        if self.cache_dir is None:
            return None
        os.makedirs(self.cache_dir, exist_ok=True)
        img_hash = hashlib.sha256(image_path.encode()).hexdigest()[:16]
        mtime = str(int(os.path.getmtime(image_path)))
        key = hashlib.sha256(f"{img_hash}:{mtime}".encode()).hexdigest()[:20]
        return os.path.join(self.cache_dir, f"enc_{key}.npy")

    def _load_cached_encoding(self, image_path: str) -> Optional[np.ndarray]:
        cache_path = self._cache_path_for(image_path)
        if cache_path is None or not os.path.isfile(cache_path):
            return None
        try:
            return np.load(cache_path, allow_pickle=False)
        except Exception:
            return None

    def _save_cached_encoding(self, image_path: str, enc: np.ndarray) -> None:
        cache_path = self._cache_path_for(image_path)
        if cache_path is None:
            return
        try:
            np.save(cache_path, enc)
        except Exception as exc:
            print(f"[TalentsLoader] Cache write error: {exc}")

    # ------------------------------------------------------------------
    # Utility
    # ------------------------------------------------------------------

    @staticmethod
    def _hash_file(path: str) -> str:
        """Return a fast SHA-256 hex digest of the first 8 KiB of *path*."""
        h = hashlib.sha256()
        with open(path, "rb") as fh:
            h.update(fh.read(8192))
        return h.hexdigest()
