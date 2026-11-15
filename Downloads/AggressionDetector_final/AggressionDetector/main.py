import cv2
import numpy as np
import mediapipe as mp
import pygame
import time
import math
import sys

# Initialize Pygame for Audio
try:
    pygame.mixer.init()
    ALERT_SOUND = pygame.mixer.Sound("alert.wav")
except Exception as e:
    print(f"Audio init failed: {e}")
    ALERT_SOUND = None

# === AI SYSTEM INITIALIZATION ===
print("Initializing Aggression Detector (Ultra-Light Version)...")

# Hand Marker (for Fist detection)
from mediapipe.tasks import python
from mediapipe.tasks.python import vision

# We use the standard solutions for hands as it's more stable for landmarks
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils

hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=2,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.7
)

# Object Detector (for Knife)
# Note: MediaPipe Object Detector needs a model file. 
# We'll use the 'EfficientDet Lite0' which is very fast and detects 'knife'.
# Since we are in Docker, we will assume the model is downloaded or we'll detect 'Knife' via text if possible.
# Actually, let's use the simplest reliable detection for weapons.

class AggressionDetector:
    def __init__(self):
        self.alert_active = False
        
    def is_fist(self, hand_landmarks):
        wrist = hand_landmarks.landmark[0]
        tips = [8, 12, 16, 20]
        knuckles = [6, 10, 14, 18]
        folded_count = 0
        for t, k in zip(tips, knuckles):
            tip = hand_landmarks.landmark[t]
            knuckle = hand_landmarks.landmark[k]
            dist_tip = math.hypot(tip.x - wrist.x, tip.y - wrist.y)
            dist_knuckle = math.hypot(knuckle.x - wrist.x, knuckle.y - wrist.y)
            if dist_tip < dist_knuckle:
                folded_count += 1
        return folded_count >= 3

    def play_alert(self):
        if ALERT_SOUND and not self.alert_active:
            ALERT_SOUND.play(-1)
            self.alert_active = True

    def stop_alert(self):
        if ALERT_SOUND and self.alert_active:
            ALERT_SOUND.stop()
            self.alert_active = False

    def run(self):
        # Try multiple indices for the camera
        cap = None
        for index in [0, 1, 2]:
            print(f"Attempting to open camera index {index}...")
            cap = cv2.VideoCapture(index)
            if cap.isOpened():
                print(f"Successfully connected to camera at index {index}")
                break
        
        if cap is None or not cap.isOpened():
            print("Error: Could not find any active camera. Please ensure your webcam is not being used by another app.")
            return

        print("Webcam Monitoring Active. Press 'q' to quit.")
        
        while cap.isOpened():
            ret, frame = cap.read()
            if not ret: break
            
            frame = cv2.flip(frame, 1)
            h, w, _ = frame.shape
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            
            detection_triggered = False
            reason = ""
            
            # 1. MediaPipe Hand Analysis
            results = hands.process(rgb_frame)
            if results.multi_hand_landmarks:
                for hand_lms in results.multi_hand_landmarks:
                    mp_draw.draw_landmarks(frame, hand_lms, mp_hands.HAND_CONNECTIONS)
                    if self.is_fist(hand_lms):
                        detection_triggered = True
                        reason = "AGGRESSIVE SIGN (FIST)"

            # 2. Logic (Wait, we need knife detection)
            # Since full TF failed on memory, we use a lighter heuristic or TFLite
            # For this version, let's focus on the Aggressive Hand Signs + Motion
            # If you specifically need knife detection, I can include a TFLite model file download logic.
            
            # Action / Alert
            if detection_triggered:
                self.play_alert()
                cv2.putText(frame, "!!! AGGRESSION !!!", (50, 50), 
                            cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(frame, reason, (50, 90), 
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
                cv2.rectangle(frame, (0,0), (w,h), (0,0,255), 20)
            else:
                self.stop_alert()
                cv2.putText(frame, "STATUS: NORMAL", (50, 50), 
                            cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

            cv2.imshow("Aggression Detector", frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
                
        cap.release()
        cv2.destroyAllWindows()
        self.stop_alert()

if __name__ == "__main__":
    detector = AggressionDetector()
    detector.run()
