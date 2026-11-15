# Plan: Run Aggression Detector

The goal is to run the Aggression Detector project located in `~/Downloads/AggressionDetector_final/AggressionDetector`. This project uses MediaPipe for fist detection and plays an alert sound when aggression is detected via the webcam.

## Proposed Changes

### Environment Setup
- I will use the existing virtual environment if it's healthy, or create a new one in the scratch directory to avoid permission issues.
- Install required dependencies: `mediapipe`, `opencv-python`, `pygame`, `numpy`.

### Running the Project
- Execute `python main.py` within the project directory.
- Ensure the `DISPLAY` environment variable is set so the OpenCV window can open.
- Ensure the camera device is accessible.

## Verification Plan

### Automated Tests
- None available in the project.

### Manual Verification
- Start the script and check if it opens the webcam feed.
- Observe if it detects a fist and plays the alert sound.
- Confirm the status changes from "NORMAL" to "AGGRESSION" on the screen.
