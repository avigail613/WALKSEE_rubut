# main.py — שרת Python ראשי
# מקבל LIDAR מ-ESP32, מעבד עם PIDNet, שולח פקודה חזרה
# רץ על אותו מחשב עם שרת Node

from flask        import Flask, request, jsonify
from flask_cors   import CORS
import threading
import time

from camera      import camera_setup, read_frame, release_camera
from pidnet_run  import load_model, run_pidnet
from lidar_fusion import find_best_angle, angle_to_command

app  = Flask(__name__)
CORS(app)  # מאפשר בקשות מ-ESP32

# נתוני LIDAR אחרונים שהגיעו מ-ESP32
last_lidar = [0.0] * 360

# ==================== נתיבי API ====================

# ESP32 שולח נתוני LIDAR
# POST /lidar { "lidar": [360 מרחקים] }
@app.route('/lidar', methods=['POST'])
def receive_lidar():
    global last_lidar
    data = request.json
    if 'lidar' in data:
        last_lidar = data['lidar']
    return jsonify({'ok': True})

# ESP32 שואל מה הפקודה הבאה
# GET /command → { "cmd": "F"/"L"/"R"/"S" }
@app.route('/command', methods=['GET'])
def get_command():
    # קריאת פריים מהמצלמה
    frame = read_frame()
    if frame is None:
        return jsonify({'cmd': 'S'})

    # זיהוי מדרכה
    sidewalk_mask = run_pidnet(frame)
    if sidewalk_mask is None:
        return jsonify({'cmd': 'S'})

    # מיזוג LIDAR + מדרכה → בחירת זווית
    best_angle, best_dist = find_best_angle(last_lidar, sidewalk_mask)

    # המרה לפקודה
    cmd = angle_to_command(best_angle)

    print(f"[Main] זווית={best_angle}, מרחק={best_dist:.2f}m, פקודה={cmd}")
    return jsonify({'cmd': cmd})

# ==================== הפעלה ====================

if __name__ == '__main__':
    print("[Main] טוען מודל PIDNet...")
    if not load_model():
        print("[Main] שגיאה בטעינת מודל!")
        exit(1)

    print("[Main] פותח מצלמה...")
    if not camera_setup(camera_index=0):
        print("[Main] שגיאה במצלמה!")
        exit(1)

    print("[Main] שרת Python רץ על http://localhost:5000")
    app.run(host='0.0.0.0', port=5000, debug=False)
