import cv2
import face_recognition
import json
import numpy as np

# Charger la base talents
with open("talents/talents.json", "r", encoding="utf-8") as f:
    TALENTS = json.load(f)["talents"]

# Charger les images talents + encodings
KNOWN_ENCODINGS = []
KNOWN_METADATA = []

for t in TALENTS:
    img = face_recognition.load_image_file(t["image"])
    enc = face_recognition.face_encodings(img)
    if len(enc) > 0:
        KNOWN_ENCODINGS.append(enc[0])
        KNOWN_METADATA.append(t)

print("[VisionCast] Base talents chargée.")

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

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    rgb = frame[:, :, ::-1]

    # Détection des visages
    locations = face_recognition.face_locations(rgb)

    # Encodage stable (compatible Python 3.11 + dlib)
    encodings = face_recognition.face_encodings(rgb, locations)

    for enc in encodings:
        matches = face_recognition.compare_faces(KNOWN_ENCODINGS, enc, tolerance=0.45)
        if True in matches:
            idx = matches.index(True)
            t = KNOWN_METADATA[idx]
            frame = draw_lower_third(frame, t["name"], t["role"])

    cv2.imshow("VisionCast AI - MVP", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
