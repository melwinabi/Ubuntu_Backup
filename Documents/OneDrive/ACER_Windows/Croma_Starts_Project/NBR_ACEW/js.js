// js.js - complete script with popup + step loading

/* ---------------------------
   Period → Tests mapping
--------------------------- */
const periodTests = {
    "Period_1": ["gestational_sac", "confirm_pregnancy", "ectopic_pregnancy"],
    "Period_2": ["heartbeat_detection", "embryo_position", "detect_twins"],
    "Period_3": ["down_syndrome_risk", "nuchal_translucency"],
    "Period_4": ["brain_abnormalities", "heart_abnormalities", "spine_abnormalities", "kidney_abnormalities", "limb_abnormalities"],
    "Period_5": ["amniotic_fluid", "baby_growth_size", "placenta_health"],
    "Period_6": ["umbilical_artery_flow", "placental_blood_flow", "fetal_growth_issues"],
    "Period_7": ["final_position", "breech_detection", "placenta_location"],
    "Period_8": ["pneumonia", "respiratory_distress", "lung_abnormalities"]
};

/* ---------------------------
   Clean test names
--------------------------- */
const testNames = {
    gestational_sac: "Gestational Sac",
    confirm_pregnancy: "Confirm Pregnancy",
    ectopic_pregnancy: "Detect Ectopic Pregnancy",
    heartbeat_detection: "Heartbeat Detection",
    embryo_position: "Embryo Position",
    detect_twins: "Detect Twins",
    down_syndrome_risk: "Down Syndrome Risk",
    nuchal_translucency: "Nuchal Translucency Thickness",
    brain_abnormalities: "Brain Abnormalities",
    heart_abnormalities: "Heart Abnormalities",
    spine_abnormalities: "Spine Abnormalities",
    kidney_abnormalities: "Kidney Abnormalities",
    limb_abnormalities: "Limb Abnormalities",
    amniotic_fluid: "Amniotic Fluid Measurement",
    baby_growth_size: "Baby Growth Size",
    placenta_health: "Placenta Health Evaluation",
    umbilical_artery_flow: "Umbilical Artery Blood Flow",
    placental_blood_flow: "Placental Blood Flow",
    fetal_growth_issues: "Detect Fetal Growth Issues",
    final_position: "Baby Final Position",
    breech_detection: "Breech Detection",
    placenta_location: "Placenta Final Location",
    pneumonia: "Pneumonia (X-Ray)",
    respiratory_distress: "Respiratory Distress",
    lung_abnormalities: "Lung Abnormalities"
};

document.addEventListener("DOMContentLoaded", () => {

    const periodSelect = document.getElementById("Period_select");
    const modelSelect  = document.getElementById("model_Select");
    const imageUpload  = document.getElementById("imageUpload");
    const preview      = document.getElementById("preview");
    const predictBtn   = document.getElementById("predictBtn");
    const resultBox    = document.getElementById("result");

    /* ---------------------------
       Populate tests on period change
    --------------------------- */
    periodSelect.addEventListener("change", () => {
        const tests = periodTests[periodSelect.value] || [];
        modelSelect.innerHTML = `<option disabled selected>Select a test available</option>`;
        tests.forEach(t => {
            const opt = document.createElement("option");
            opt.value = t;
            opt.textContent = testNames[t] || t;
            modelSelect.appendChild(opt);
        });
    });

    /* ---------------------------
       Image preview
    --------------------------- */
    imageUpload.addEventListener("change", () => {
        const f = imageUpload.files[0];
        if (f) {
            preview.src = URL.createObjectURL(f);
            preview.style.display = "block";
        }
    });

    /* ---------------------------
       Helper: get clientX
    --------------------------- */
    function getClientX(e) {
        if (e.touches) return e.touches[0].clientX;
        if (e.changedTouches) return e.changedTouches[0].clientX;
        return e.clientX;
    }

    /* ---------------------------
       Loader UI
    --------------------------- */
    function showLoader(text) {
        resultBox.style.display = "flex";
        resultBox.innerHTML = `
            <div class="spinner"></div>
            <div style="font-weight:600">${text}</div>
        `;
    }

    function hideLoader() {
        resultBox.style.display = "none";
        resultBox.innerHTML = "";
    }

    /* ---------------------------
       Popup UI
    --------------------------- */
function showPopup(text, isPositive) {

    // remove existing popup if any
    const oldPopup = document.getElementById("resultPopup");
    if (oldPopup) oldPopup.remove();

    const popup = document.createElement("div");
    popup.id = "resultPopup";
    popup.style.cssText = `
        position: fixed;
        inset: 0;
        background: rgba(0,0,0,0.6);
        display: flex;
        align-items: center;
        justify-content: center;
        z-index: 9999;
    `;

    popup.innerHTML = `
        <div id="popupBox" style="
            background: white;
            padding: 30px;
            border-radius: 12px;
            text-align: center;
            width: 280px;
        ">
            <h2 style="color:${isPositive ? '#03b146' : '#c62828'}">
                ${text}
            </h2>
            <button id="closePopupBtn" style="
                margin-top: 20px;
                background: #022a65;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 6px;
                cursor: pointer;
            ">Close</button>
        </div>
    `;

    document.body.appendChild(popup);

    /* ✅ Close when clicking button */
    document.getElementById("closePopupBtn").onclick = () => {
        popup.remove();
    };

    /* ✅ Optional: close when clicking outside popup */
    popup.addEventListener("click", (e) => {
        if (e.target === popup) popup.remove();
    });
}


    /* ---------------------------
       Main Prediction Flow
    --------------------------- */
    function runPrediction(isRightSide) {

        showLoader("Detecting path...");
        setTimeout(() => showLoader("Detecting model..."), 4000);
        setTimeout(() => showLoader("Analysing model with test image..."), 8000);
        setTimeout(() => showLoader("Proving result..."), 12000);

        setTimeout(() => {
            hideLoader();
            if (isRightSide) {
                showPopup("✅ Your test is POSITIVE", true);
            } else {
                showPopup("❌ Your test is NEGATIVE", false);
            }
        }, 16000);
    }
//.............Real Flow............

// async function runPrediction() {

//     if (!imageUpload.files[0] || !modelSelect.value) {
//         alert("Please select test and upload image");
//         return;
//     }

//     showLoader("Uploading image to AI...");

//     const formData = new FormData();
//     formData.append("file", imageUpload.files[0]);
//     formData.append("test_name", modelSelect.value);

//     try {
//         const response = await fetch("http://127.0.0.1:8000/predict", {
//             method: "POST",
//             body: formData
//         });

//         const data = await response.json();
//         hideLoader();

//         console.log("AI Result:", data);

//         if (data.confidence >= 70) {
//             showPopup("✅ POSITIVE DETECTION", true);
//         } else {
//             showPopup("❌ NO ABNORMALITY DETECTED", false);
//         }

//     } catch (err) {
//         hideLoader();
//         alert("AI server not reachable");
//         console.error(err);
//     }
// }

    /* ---------------------------
       Button left / right detection
    --------------------------- */
    function handlePredict(e) {
        e.preventDefault();

        if (!modelSelect.value) {
            alert("Please select a test model first");
            return;
        }

        const rect = predictBtn.getBoundingClientRect();
        const x = getClientX(e) - rect.left;
        const isRightSide = x > rect.width / 2;

        runPrediction(isRightSide);
    }

    predictBtn.addEventListener("pointerdown", handlePredict);
    predictBtn.addEventListener("click", handlePredict);
});

/* ---------------------------
   Loader UI
--------------------------- */
function showLoader(text) {
    resultBox.style.display = "flex";
    resultBox.style.alignItems = "center";
    resultBox.style.justifyContent = "center";
    resultBox.style.gap = "12px";
    resultBox.innerHTML = `
        <div class="loader"></div>
        <div style="font-weight:600; color:#022a65;">${text}</div>
    `;
}

/* Spinner CSS (add in your css.css file if not already) */


