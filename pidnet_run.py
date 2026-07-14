# pidnet_run.py — זיהוי מדרכה עם PIDNet
# משתמש בקבצים שכבר יש לך: pidnet.py, mask.py, convert.py, config.py

import sys
import os
import torch
import numpy as np

# הוספת תיקיית PIDNet לנתיב
# שנה את הנתיב לפי מיקום הקבצים שלך
PIDNET_PATH = os.path.join(os.path.dirname(__file__), "PIDnet")
sys.path.insert(0, PIDNET_PATH)

from config  import MODEL_PATH, NUM_CLASSES, SIDEWALK_CLASS_ID
from convert import preprocess
from mask    import get_segmentation_map, get_sidewalk_mask
from models.pidnet import get_pred_model

# טעינת המודל — פעם אחת בלבד בהתחלה
model  = None
device = None

def load_model():
    global model, device
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"[PIDNet] רץ על: {device}")

    model = get_pred_model(name="pidnet_s", num_classes=NUM_CLASSES)
    model.eval()

    if not os.path.exists(MODEL_PATH):
        print(f"[PIDNet] שגיאה: משקולות לא נמצאו ב-{MODEL_PATH}")
        return False

    checkpoint = torch.load(MODEL_PATH, map_location=device)
    if isinstance(checkpoint, dict) and "state_dict" in checkpoint:
        state_dict = checkpoint["state_dict"]
    else:
        state_dict = checkpoint

    # הסרת קידומת model. אם קיימת
    cleaned = {}
    for k, v in state_dict.items():
        cleaned[k[6:] if k.startswith("model.") else k] = v

    model.load_state_dict(cleaned, strict=False)
    model.to(device)
    print("[PIDNet] מודל נטען בהצלחה")
    return True

def run_pidnet(frame):
    # מקבל תמונה BGR מהמצלמה
    # מחזיר מסכה בינארית: 1=מדרכה, 0=שאר
    global model, device

    if model is None:
        return None

    # עיבוד מקדים
    tensor = preprocess(frame).to(device)

    with torch.no_grad():
        output = model(tensor)

    if isinstance(output, (list, tuple)):
        output = output[0]

    # מפת מחלקות
    seg_map = get_segmentation_map(output)

    # מסכה בינארית של מדרכה בלבד
    sidewalk_mask = get_sidewalk_mask(seg_map)

    # שינוי גודל להתאמה לתמונה המקורית
    import cv2
    h, w = frame.shape[:2]
    sidewalk_mask = cv2.resize(
        sidewalk_mask, (w, h),
        interpolation=cv2.INTER_NEAREST
    )

    return sidewalk_mask
