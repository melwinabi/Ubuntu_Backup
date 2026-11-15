import os
import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Dense, GlobalAveragePooling2D
from tensorflow.keras.preprocessing.image import ImageDataGenerator, load_img, img_to_array
from tensorflow.keras.optimizers import Adam
from tensorflow.keras import backend as K

# ---------------- CONFIG ----------------
tests = [
    "gestational_sac","confirm_pregnancy","ectopic_pregnancy","heartbeat_detection",
    "embryo_position","detect_twins","down_syndrome_risk","nuchal_translucency",
    "brain_abnormalities","heart_abnormalities","spine_abnormalities","kidney_abnormalities",
    "limb_abnormalities","amniotic_fluid","baby_growth_size","placenta_health",
    "umbilical_artery_flow","placental_blood_flow","fetal_growth_issues","final_position",
    "breech_detection","placenta_location","pneumonia","respiratory_distress","lung_abnormalities"
]

base_train = r"C:\Users\Melwin Abi\OneDrive\Documents\NBR_ACEW\ML_Model"
base_test  = r"C:\Users\Melwin Abi\OneDrive\Documents\NBR_ACEW\ML_Test"

save_models_root = os.path.join(base_train, "Models")
os.makedirs(save_models_root, exist_ok=True)

datagen = ImageDataGenerator(rescale=1./255)

print("Models will be saved to:", save_models_root)
print("=" * 70)

# ---------------- LOOP ----------------
for test in tests:
    print(f"\n🔹 Processing: {test}")

    train_path = os.path.join(base_train, test, "images")
    test_model_folder = os.path.join(base_test, test, "Model")
    os.makedirs(test_model_folder, exist_ok=True)

    model_path_main = os.path.join(save_models_root, f"{test}.h5")
    model_path_copy = os.path.join(test_model_folder, f"{test}.h5")

    if not os.path.exists(train_path):
        print("❌ Train path not found, skipping")
        continue

    class_folders = os.listdir(train_path)
    print("   Classes found:", class_folders)

    if len(class_folders) < 2:
        print("❌ Need at least 2 class folders, skipping")
        continue

    # -------- LOAD DATA --------
    train_data = datagen.flow_from_directory(
        train_path,
        target_size=(224, 224),
        batch_size=4,
        class_mode='categorical',
        shuffle=True
    )

    if train_data.num_classes < 2:
        print("❌ Invalid dataset, skipping")
        continue

    # -------- BUILD MODEL --------
    base_model = MobileNetV2(
        weights="imagenet",
        include_top=False,
        input_shape=(224,224,3)
    )

    for layer in base_model.layers:
        layer.trainable = False

    x = base_model.output
    x = GlobalAveragePooling2D()(x)
    x = Dense(128, activation="relu")(x)
    outputs = Dense(train_data.num_classes, activation="softmax")(x)

    model = Model(inputs=base_model.input, outputs=outputs)
    model.compile(
        optimizer=Adam(learning_rate=1e-4),
        loss="categorical_crossentropy",
        metrics=["accuracy"]
    )

    # -------- TRAIN --------
    history = model.fit(train_data, epochs=5, verbose=1)

    # -------- SAVE MODEL --------
    model.save(model_path_main)
    model.save(model_path_copy)
    print("✅ Model saved")

    classes = list(train_data.class_indices.keys())
    print("   Class labels:", classes)

    # -------- TRAINING GRAPH --------
    plt.figure(figsize=(12,5))

    plt.subplot(1,2,1)
    plt.plot(history.history['accuracy'])
    plt.title(f"{test} - Accuracy")
    plt.xlabel("Epoch")
    plt.ylabel("Accuracy")

    plt.subplot(1,2,2)
    plt.plot(history.history['loss'])
    plt.title(f"{test} - Loss")
    plt.xlabel("Epoch")
    plt.ylabel("Loss")

    plt.tight_layout()
    plt.savefig(os.path.join(test_model_folder, f"{test}_training.png"))
    plt.close()

    # -------- SAMPLE IMAGES --------
    train_data.reset()
    x_batch, y_batch = next(train_data)

    plt.figure(figsize=(10,10))
    for i in range(min(9, len(x_batch))):
        plt.subplot(3,3,i+1)
        plt.imshow(x_batch[i])
        plt.title(classes[np.argmax(y_batch[i])])
        plt.axis("off")

    plt.suptitle(f"Samples - {test}")
    plt.savefig(os.path.join(test_model_folder, f"{test}_samples.png"))
    plt.close()

    # -------- OPTIONAL TEST IMAGE PREDICTION --------
    for img_file in os.listdir(test_model_folder):
        if img_file.lower().endswith(('.png','.jpg','.jpeg')):
            img_path = os.path.join(test_model_folder, img_file)
            img = load_img(img_path, target_size=(224,224))
            arr = img_to_array(img) / 255.0
            arr = np.expand_dims(arr, 0)

            pred = model.predict(arr, verbose=0)
            predicted_class = classes[np.argmax(pred)]
            print(f"   🖼 {img_file} → {predicted_class}")

    # -------- CLEAR MEMORY --------
    K.clear_session()
    print("✔ Completed:", test)

print("\n🎉 ALL TESTS PROCESSED SUCCESSFULLY")
