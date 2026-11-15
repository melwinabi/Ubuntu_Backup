import os
import numpy as np
from fastapi import FastAPI, UploadFile, File, Form
from fastapi.middleware.cors import CORSMiddleware
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing.image import img_to_array
from PIL import Image
from io import BytesIO

# ---------------- CONFIG ----------------
BASE_MODEL_PATH = r"C:\Users\Melwin Abi\OneDrive\Documents\NBR_ACEW\ML_Model\Models"
IMG_SIZE = (224, 224)

# cache loaded models
loaded_models = {}

# ---------------- APP ----------------
app = FastAPI(title="Baby Disease Detection AI")

# ---------------- CORS (MANDATORY FOR BROWSER) ----------------
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],   # allow all (development)
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ---------------- HELPERS ----------------
def get_model(test_name: str):
    """
    Dynamically load model based on test name
    """
    if test_name not in loaded_models:
        model_path = os.path.join(BASE_MODEL_PATH, f"{test_name}.h5")

        if not os.path.exists(model_path):
            return None

        loaded_models[test_name] = load_model(model_path)

    return loaded_models[test_name]


def preprocess(image: Image.Image):
    """
    EXACT SAME preprocessing as training
    """
    image = image.convert("RGB")
    image = image.resize(IMG_SIZE)

    arr = img_to_array(image)
    arr = arr / 255.0
    arr = np.expand_dims(arr, axis=0)

    return arr


# ---------------- API ----------------
@app.post("/predict")
async def predict(
    test_name: str = Form(...),
    file: UploadFile = File(...)
):
    model = get_model(test_name)

    if model is None:
        return {
            "error": f"Model not found for test: {test_name}"
        }

    image_bytes = await file.read()
    image = Image.open(BytesIO(image_bytes))

    tensor = preprocess(image)

    preds = model.predict(tensor)[0]
    class_index = int(np.argmax(preds))
    confidence = float(np.max(preds)) * 100

    return {
        "test": test_name,
        "class_index": class_index,
        "confidence": round(confidence, 2)
    }
