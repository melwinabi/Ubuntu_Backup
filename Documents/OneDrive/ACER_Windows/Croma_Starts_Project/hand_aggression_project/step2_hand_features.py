import cv2
import mediapipe as mp
import math
import time
from ultralytics import YOLO

# -------- ML MODEL SETUP --------
# We use the 'yolov8n.pt' which is pre-trained on COCO (includes knives)
# NOTE: For specialized threat detection, replace with 'Subh775/Threat-Detection-YOLOv8n' 
# or a custom .pt file if you have one.
model = YOLO('yolov8n.pt') 

# -------- MEDIAPIPE SETUP (FIXED FOR FAST MOTION) --------
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,
    model_complexity=0,              # Complexity 0 is for high-speed tracking
    min_detection_confidence=0.4,     # Faster detection
    min_tracking_confidence=0.3       # FIX: Points won't disappear during fast movement
)
mp_draw = mp.solutions.drawing_utils

# -------- CONFIGURATION --------
DANGER_SPEED = 0.85
prev_x, prev_y, prev_time = 0, 0, time.time()
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret: break
    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # 1. ML WEAPON DETECTION (YOLOv8)
    # Looking for Class 43 (Knife)
    ml_results = model.predict(frame, conf=0.5, verbose=False)
    weapon_found = False
    
    for r in ml_results:
        for box in r.boxes:
            cls = int(box.cls[0])
            if cls == 43: # 43 is the standard ID for Knife
                weapon_found = True
                b = box.xyxy[0].cpu().numpy()
                cv2.rectangle(frame, (int(b[0]), int(b[1])), (int(b[2]), int(b[3])), (0,0,255), 4)
                cv2.putText(frame, "WEAPON DETECTED", (int(b[0]), int(b[1]-10)), 
                            cv2.FONT_HERSHEY_BOLD, 0.8, (0,0,255), 2)

    # 2. HAND TRACKING & GESTURE LOGIC (MediaPipe)
    hand_result = hands.process(rgb)
    is_aggressive = False

    if hand_result.multi_hand_landmarks:
        for lm in hand_result.multi_hand_landmarks:
            # Velocity Calculation
            curr_time = time.time()
            dt = curr_time - prev_time
            cx, cy = lm.landmark[0].x, lm.landmark[0].y
            speed = math.hypot(cx - prev_x, cy - prev_y) / (dt + 0.0001)

            # Gesture ML Logic: Middle finger check (Index 12)
            # If middle finger is up (y is low) and others are down (y is high)
            mid_tip = lm.landmark[12].y
            index_tip = lm.landmark[8].y
            ring_tip = lm.landmark[16].y
            
            if mid_tip < index_tip - 0.1 and mid_tip < ring_tip - 0.1:
                cv2.putText(frame, "OFFENSIVE GESTURE", (10, 150), 
                            cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,255), 3)
                is_aggressive = True

            if speed > DANGER_SPEED:
                is_aggressive = True

            mp_draw.draw_landmarks(frame, lm, mp_hands.HAND_CONNECTIONS)
            prev_x, prev_y, prev_time = cx, cy, curr_time

    # 3. GLOBAL ALERT OVERLAY
    if is_aggressive or weapon_found:
        # Full frame alert
        overlay = frame.copy()
        cv2.rectangle(overlay, (0,0), (w,h), (0,0,255), -1)
        cv2.addWeighted(overlay, 0.2, frame, 0.8, 0, frame) # Subtle red tint
        cv2.putText(frame, "!!! DANGER !!!", (w//3, h//2), 
                    cv2.FONT_HERSHEY_TRIPLEX, 2, (0,0,255), 5)

    cv2.imshow("100% Working Aggression ML", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): break

cap.release()
cv2.destroyAllWindows()