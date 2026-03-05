"""Optimized face encoding with batch processing and caching.

Provides efficient 128-dimensional face encoding computation with
LRU caching, batch support, and configurable jitter for accuracy.
"""

import hashlib
from collections import OrderedDict
from typing import Dict, List, Optional, Tuple

import numpy as np

try:
    import face_recognition
except ImportError:
    face_recognition = None


class Encoding:
    """High-performance face encoder with caching.

    Optimizes encoding throughput via:
    - Batch computation of multiple faces in a single call.
    - Spatial-hash LRU cache: avoids re-encoding faces that haven't moved.
    - Configurable ``num_jitters`` for accuracy vs. speed trade-off.

    Args:
        num_jitters: Times to re-sample each face for encoding.
            Higher values are more accurate but slower (default 1).
        cache_size: Maximum number of cached encodings.
            Set to 0 to disable caching.
    """

    def __init__(self, num_jitters: int = 1, cache_size: int = 128):
        if face_recognition is None:
            raise ImportError(
                "face_recognition is required. Install with: "
                "pip install face_recognition"
            )
        if num_jitters < 1:
            raise ValueError("num_jitters must be >= 1")

        self.num_jitters = num_jitters
        self.cache_size = cache_size
        self._cache: OrderedDict[str, np.ndarray] = OrderedDict()

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def encode_faces(
        self,
        frame: np.ndarray,
        locations: List[Tuple[int, int, int, int]],
    ) -> List[np.ndarray]:
        """Compute 128-d encodings for all faces in *frame*.

        If caching is enabled, faces whose bounding-box hash matches a
        cached entry are returned from cache instead of being recomputed.

        Args:
            frame: RGB image as ``numpy`` array (H, W, 3).
            locations: Face bounding boxes ``(top, right, bottom, left)``.

        Returns:
            List of 128-d ``numpy`` arrays, one per location.
        """
        if not locations:
            return []

        if self.cache_size == 0:
            return self._batch_encode(frame, locations)

        results: List[Optional[np.ndarray]] = [None] * len(locations)
        uncached_indices: List[int] = []
        uncached_locations: List[Tuple[int, int, int, int]] = []

        for i, loc in enumerate(locations):
            key = self._location_key(loc)
            cached = self._cache_get(key)
            if cached is not None:
                results[i] = cached
            else:
                uncached_indices.append(i)
                uncached_locations.append(loc)

        if uncached_locations:
            new_encodings = self._batch_encode(frame, uncached_locations)
            for idx, enc in zip(uncached_indices, new_encodings):
                results[idx] = enc
                key = self._location_key(locations[idx])
                self._cache_put(key, enc)

        return results  # type: ignore[return-value]

    def encode_single(
        self,
        frame: np.ndarray,
        location: Tuple[int, int, int, int],
    ) -> Optional[np.ndarray]:
        """Encode a single face.

        Convenience wrapper around :meth:`encode_faces`.

        Returns:
            128-d encoding or ``None`` if encoding fails.
        """
        encs = self.encode_faces(frame, [location])
        return encs[0] if encs else None

    def clear_cache(self) -> None:
        """Flush the encoding cache."""
        self._cache.clear()

    @property
    def cache_stats(self) -> Dict[str, int]:
        """Return current cache size information."""
        return {"size": len(self._cache), "capacity": self.cache_size}

    # ------------------------------------------------------------------
    # Internal helpers
    # ------------------------------------------------------------------

    def _batch_encode(
        self,
        frame: np.ndarray,
        locations: List[Tuple[int, int, int, int]],
    ) -> List[np.ndarray]:
        """Compute encodings for a batch of face locations."""
        return face_recognition.face_encodings(
            frame, known_face_locations=locations, num_jitters=self.num_jitters
        )

    @staticmethod
    def _location_key(loc: Tuple[int, int, int, int]) -> str:
        """Derive a hash key from a bounding-box tuple."""
        raw = f"{loc[0]}:{loc[1]}:{loc[2]}:{loc[3]}"
        return hashlib.md5(raw.encode()).hexdigest()  # noqa: S324

    def _cache_get(self, key: str) -> Optional[np.ndarray]:
        """Retrieve an encoding from the LRU cache."""
        if key in self._cache:
            self._cache.move_to_end(key)
            return self._cache[key]
        return None

    def _cache_put(self, key: str, value: np.ndarray) -> None:
        """Insert an encoding into the LRU cache, evicting if full."""
        if key in self._cache:
            self._cache.move_to_end(key)
        self._cache[key] = value
        while len(self._cache) > self.cache_size:
            self._cache.popitem(last=False)
