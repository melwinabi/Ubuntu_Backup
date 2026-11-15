#!/bin/bash

# Allow Docker to access X11 (Display)
xhost +local:docker

# Build the image (optional, ensure it exists)
echo "Building Docker image..."
docker build -t aggression-detector .

# Run the container with necessary devices and volumes
echo "Launching Aggression Detector..."
docker run --rm -it \
    --device /dev/video0:/dev/video0 \
    --device /dev/video1:/dev/video1 \
    --device /dev/snd:/dev/snd \
    --group-add video \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    aggression-detector
