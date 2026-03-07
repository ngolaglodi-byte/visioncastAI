import logging
import os
import time

import cv2
import face_recognition
import json

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

# Configurable lower-third display duration in seconds (default: 5s).
# This applies to the CPU-based rendering in main.py.
# Template-level durations (display_duration_ms in JSON) are used by
# the C++ GpuCompositor engine instead.
LOWER_THIRD_DURATION_SEC = float(os.environ.get("LOWER_THIRD_DURATION_SEC", "5.0"))


def load_talents(path):
    """Load talent list from a JSON file.

    Returns the list of talent dicts on success, or an empty list if the file
    is missing or malformed.
    """
    try:
        with open(path, "r", encoding="utf-8") as f:
            talents = json.load(f)["talents"]
        logger.info("talents.json loaded successfully (%d talent(s)).", len(talents))
        return talents
    except FileNotFoundError:
        logger.error("talents.json not found at %s. Starting with empty talent list.", path)
    except (json.JSONDecodeError, KeyError) as exc:
        logger.error("Failed to parse talents.json: %s. Starting with empty talent list.", exc)
    except Exception as exc:
        logger.error("Unexpected error loading talents.json: %s. Starting with empty talent list.", exc)
    return []


def load_talent_encodings(talents, project_root):
    """Load face encodings for each talent.

    Skips talents whose photo is missing or whose face cannot be encoded
    (fallback behaviour).  Returns *(encodings, metadata)* lists.
    """
    encodings = []
    metadata = []

    for t in talents:
        name = t.get("name", "unknown")

        image_path = os.path.join(project_root, t.get("photo", ""))
        try:
            img = face_recognition.load_image_file(image_path)
        except Exception as exc:
            logger.warning("Photo missing or unreadable for talent '%s': %s. Skipping.", name, exc)
            continue

        try:
            enc = face_recognition.face_encodings(img)
        except Exception as exc:
            logger.warning("Failed to encode face for talent '%s': %s. Skipping.", name, exc)
            continue

        if len(enc) > 0:
            encodings.append(enc[0])
            metadata.append(t)
            logger.info("Talent '%s' loaded successfully.", name)
        else:
            logger.warning("No face detected in photo for talent '%s'. Skipping.", name)

    logger.info("Talent database ready: %d encoding(s) loaded.", len(encodings))
    return encodings, metadata


# Load data at module level — errors are logged, never fatal.
TALENTS = load_talents(TALENTS_PATH)
KNOWN_ENCODINGS, KNOWN_METADATA = load_talent_encodings(TALENTS, PROJECT_ROOT)


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
    _lower_third_timers = {}
    _consecutive_read_failures = 0
    _MAX_READ_FAILURES = 30

    try:
        cap = cv2.VideoCapture(0)
    except Exception as exc:
        logger.error("Failed to create video capture: %s", exc)
        return

    if not cap.isOpened():
        logger.error("Cannot open camera device 0. Exiting run loop.")
        cap.release()
        return

    logger.info("Camera opened successfully. Press ESC to quit.")

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                _consecutive_read_failures += 1
                if _consecutive_read_failures >= _MAX_READ_FAILURES:
                    logger.error(
                        "Failed to read frame %d times in a row. Stopping.",
                        _MAX_READ_FAILURES,
                    )
                    break
                logger.warning("Failed to read frame (attempt %d). Retrying...", _consecutive_read_failures)
                time.sleep(0.1)
                continue

            _consecutive_read_failures = 0
            rgb = frame[:, :, ::-1]

            try:
                locations = face_recognition.face_locations(rgb)
                encodings = face_recognition.face_encodings(rgb, locations)
            except Exception as exc:
                logger.warning("Face detection/encoding error: %s. Skipping frame.", exc)
                locations = []
                encodings = []

            now = time.monotonic()

            for enc in encodings:
                try:
                    matches = face_recognition.compare_faces(KNOWN_ENCODINGS, enc, tolerance=0.45)
                except Exception as exc:
                    logger.warning("Face comparison error: %s. Skipping.", exc)
                    continue

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
