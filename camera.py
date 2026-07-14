# camera.py — קריאת תמונות מהמצלמה
# מצלמה USB רגילה מחוברת למחשב

import cv2

# אובייקט מצלמה גלובלי
cap = None

def camera_setup(camera_index=0):
    # camera_index=0 = מצלמה ראשונה במחשב
    # אם יש כמה מצלמות — שנה ל-1, 2 וכו
    global cap
    cap = cv2.VideoCapture(camera_index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH,  640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    if not cap.isOpened():
        print("[Camera] שגיאה: לא ניתן לפתוח מצלמה")
        return False

    print("[Camera] מצלמה אותחלה")
    return True

def read_frame():
    # קריאת פריים אחד מהמצלמה
    # מחזיר תמונה BGR או None אם נכשל
    global cap
    if cap is None:
        return None

    ret, frame = cap.read()
    if not ret:
        return None

    return frame

def release_camera():
    global cap
    if cap:
        cap.release()
