# main.py — שרת Python ראשי
# מקבל תמונות מESP32-CAM ו-LIDAR מESP32
# מעבד עם PIDNet ושולח פקודה חזרה

from flask      import Flask, request, jsonify
from flask_cors import CORS
import numpy as np
import cv2

from pidnet_run   import load_model, run_pidnet
from lidar_fusion import find_best_angle, angle_to_command

app  = Flask(__name__)
CORS(app)

# נתוני LIDAR אחרונים — 360 מרחקים
last_lidar = [0.0] * 360

# תמונה אחרונה מ-ESP32-CAM
last_frame = None

# ==================== קבלת תמונה מ-ESP32-CAM ====================

# ESP32-CAM שולח תמונה JPEG
# POST /frame (body = בייטים של JPEG)
@app.route('/frame', methods=['POST'])
def receive_frame():
    global last_frame

    # קבלת הבייטים
    jpg_bytes = request.data
    if not jpg_bytes:
        return jsonify({'ok': False})

    # המרת JPEG לתמונה OpenCV
    np_arr    = np.frombuffer(jpg_bytes, dtype=np.uint8)
    last_frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

    return jsonify({'ok': True})

# ==================== קבלת LIDAR מ-ESP32 ====================

# ESP32 שולח מערך 360 מרחקים
# POST /lidar { "lidar": [...] }
@app.route('/lidar', methods=['POST'])
def receive_lidar():
    global last_lidar
    data = request.json
    if 'lidar' in data:
        last_lidar = data['lidar']
    return jsonify({'ok': True})

# ==================== שליחת פקודה ל-ESP32 ====================

# ESP32 שואל מה לעשות
# GET /command → { "cmd": "F"/"L"/"R"/"S" }
@app.route('/command', methods=['GET'])
def get_command():
    global last_frame, last_lidar

    # אם אין תמונה עדיין — עצור
    if last_frame is None:
        return jsonify({'cmd': 'S'})

    # זיהוי מדרכה עם PIDNet
    sidewalk_mask = run_pidnet(last_frame)
    if sidewalk_mask is None:
        return jsonify({'cmd': 'S'})

    # מיזוג LIDAR + מדרכה → בחירת זווית הכי טובה
    best_angle, best_dist = find_best_angle(last_lidar, sidewalk_mask)

    # המרה לפקודה
    cmd = angle_to_command(best_angle)

    print(f"[Main] זווית={best_angle}, מרחק={best_dist:.2f}m, פקודה={cmd}")
    return jsonify({'cmd': cmd})

# ==================== הפעלה ====================

if __name__ == '__main__':
    print("[Main] טוען PIDNet...")
    if not load_model():
        print("[Main] שגיאה — מודל לא נטען!")
        exit(1)

    print("[Main] שרת Python רץ על http://0.0.0.0:5000")
    print("[Main] ממתין לתמונות מESP32-CAM ולLIDAR מESP32...")
    app.run(host='0.0.0.0', port=5000, debug=False)
