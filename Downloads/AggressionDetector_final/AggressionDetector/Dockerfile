# Use an official Python runtime as a parent image
FROM python:3.10

# Set the working directory in the container
WORKDIR /app

# Install system dependencies for OpenCV, MediaPipe, and Audio
RUN apt-get update && apt-get install -y \
    libgl1 \
    libglib2.0-0 \
    libasound2 \
    libasound2-dev \
    libsm6 \
    libxext6 \
    libxrender1 \
    libfontconfig1 \
    libice6 \
    libusb-1.0-0 \
    python3-tk \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

# Copy the requirements file into the container
COPY requirements.txt .

# Install any needed packages specified in requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application code
COPY . .

# Run the application
CMD ["python", "main.py"]
