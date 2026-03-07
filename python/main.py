import logging
import os
import sys
import time

import cv2
import face_recognition
import json
import numpy as np

# ---------------------------------------------------------------------------
# Logging
# ---------------------------------------------------------------------------

logging.basicConfig(
    level=logging.INFO,
    format="[VisionCast] %(levelname)s %(message)s",
)
logger = logging.getLogger(__name__)

# ---------------------------------------------------------------------------
# Talent database
# ---------------------------------------------------------------------------

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.normpath(os.path.join(BASE_DIR, ".."))
TALENTS_PATH = os.path.join(PROJECT_ROOT, "talents", "talents.json")

with open(TALENTS_PATH, "r", encoding="utf-8") as f:
    TALENTS = json.load(f)["talents"]

# Configurable lower-third display duration in seconds (default: 5s).
# This applies to the CPU-based rendering in main.py.
# Template-level durations (display_duration_ms in JSON) are used by
# the C++ GpuCompositor engine instead.
LOWER_THIRD_DURATION_SEC = float(os.environ.get("LOWER_THIRD_DURATION_SEC", "5.0"))

# Charger les images talents + encodings
KNOWN_ENCODINGS = []
KNOWN_METADATA = []

for t in TALENTS:
    image_path = os.path.join(PROJECT_ROOT, t["photo"])
    img = face_recognition.load_image_file(image_path)
    enc = face_recognition.face_encodings(img)
    if len(enc) > 0:
        KNOWN_ENCODINGS.append(enc[0])
        KNOWN_METADATA.append(t)

logger.info("Base talents chargée.")


def draw_lower_third(frame, title, subtitle):
    h, w, _ = frame.shape
    band_h = int(h * 0.18)

    overlay = frame.copy()
    cv2.rectangle(overlay, (0, h - band_h), (w, h), (0, 0, 0), -1)
    frame = cv2.addWeighted(overlay, 0.6, frame, 0.4, 0)

    cv2.putText(frame, title, (40, h - band_h + 50),
                cv2.FONT_HERSHEY_SIMPLEX, 1.0, (255, 255, 255), 2)

    cv2.putText(frame, subtitle, (40, h - band_h + 90),
                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (200, 200, 200), 2)

    return frame


def run():
    """Main capture and recognition loop."""
    # Timer state per talent: maps talent index -> timestamp when lower third
    # was first shown.
    _lower_third_timers = {}

    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        logger.error("Cannot open camera device 0. Exiting.")
        sys.exit(1)

    logger.info("Camera opened. Press ESC to quit.")

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                logger.warning("Failed to read frame from camera. Stopping.")
                break

            rgb = frame[:, :, ::-1]

            # Détection des visages
            locations = face_recognition.face_locations(rgb)

            # Encodage stable (compatible Python 3.11 + dlib)
            encodings = face_recognition.face_encodings(rgb, locations)

            now = time.monotonic()

            for enc in encodings:
                matches = face_recognition.compare_faces(KNOWN_ENCODINGS, enc, tolerance=0.45)
                if True in matches:
                    idx = matches.index(True)
                    t = KNOWN_METADATA[idx]

                    # Start timer on first detection, show lower third for
                    # the configured duration.
                    if idx not in _lower_third_timers:
                        _lower_third_timers[idx] = now

                    elapsed = now - _lower_third_timers[idx]
                    if elapsed < LOWER_THIRD_DURATION_SEC:
                        frame = draw_lower_third(frame, t["name"], t["role"])
                    else:
                        # Timer expired — remove entry so it can re-trigger later
                        del _lower_third_timers[idx]

            cv2.imshow("VisionCast AI - MVP", frame)

            if cv2.waitKey(1) & 0xFF == 27:
                break

    except Exception:
        logger.exception("Unexpected error in capture loop.")
    finally:
        cap.release()
        cv2.destroyAllWindows()
        logger.info("Camera released. Exiting.")


if __name__ == "__main__":
    run()
