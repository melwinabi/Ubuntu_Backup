import os
import numpy as np
from fastapi import FastAPI, UploadFile, File, Form
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing.image import img_to_array
from PIL import Image
from io import BytesIO

# ---------------- CONFIG ----------------
BASE_MODEL_PATH = r"C:\Users\Melwin Abi\OneDrive\Documents\NBR_ACEW\ML_Model\Models"
IMG_SIZE = (224, 224)

# cache loaded models
loaded_models = {}

app = FastAPI(title="Baby Disease Detection AI")

# ---------------- HELPERS ----------------
def get_model(test_name: str):
    if test_name not in loaded_models:
        model_path = os.path.join(BASE_MODEL_PATH, f"{test_name}.h5")
        if not os.path.exists(model_path):
            raise ValueError("Model not found for test: " + test_name)
        loaded_models[test_name] = load_model(model_path)
    return loaded_models[test_name]

def preprocess(image: Image.Image):
    image = image.convert("RGB")
    image = image.resize(IMG_SIZE)
    arr = img_to_array(image) / 255.0
    return np.expand_dims(arr, axis=0)

# ---------------- API ----------------
@app.post("/predict")
async def predict(
    test_name: str = Form(...),
    file: UploadFile = File(...)
):
    model = get_model(test_name)

    image = Image.open(BytesIO(await file.read()))
    tensor = preprocess(image)

    preds = model.predict(tensor)[0]
    class_index = int(np.argmax(preds))
    confidence = float(np.max(preds))

    return {
        "test": test_name,
        "class_index": class_index,
        "confidence": round(confidence * 100, 2)
    }
