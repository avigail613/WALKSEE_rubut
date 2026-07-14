# lidar_fusion.py — מיזוג נתוני LIDAR עם זיהוי מדרכה
# בוחר את הזווית הכי בטוחה לנסיעה

import numpy as np

# שדה ראייה של המצלמה — מעלות
CAMERA_FOV = 90.0

# מרחק מינימלי שנחשב בטוח — מטרים
MIN_SAFE_DIST = 1.5

def find_best_angle(lidar_data, sidewalk_mask):
    # lidar_data   = רשימה של 360 מרחקים (אינדקס = זווית)
    # sidewalk_mask = תמונה בינארית 1=מדרכה

    img_h, img_w = sidewalk_mask.shape
    half_fov     = CAMERA_FOV / 2.0

    best_angle    = None
    best_distance = -1.0

    # עובר על כל הזוויות
    for angle in range(360):
        distance = lidar_data[angle]
        if distance <= 0:
            continue

        # המרת זווית לפיקסל בתמונה
        # רק זוויות בתוך שדה הראייה של המצלמה
        norm_angle = angle if angle <= 180 else angle - 360
        if not (-half_fov <= norm_angle <= half_fov):
            continue

        # מיקום הפיקסל בתמונה
        x_pixel = int((norm_angle + half_fov) / CAMERA_FOV * img_w)
        x_pixel = max(0, min(img_w - 1, x_pixel))

        # בדיקה אם הזווית על מדרכה
        y_center = img_h // 2
        if sidewalk_mask[y_center, x_pixel] != 1:
            continue  # לא על מדרכה — דלג

        # בדיקה שכל הדרך עד המכשול על מדרכה
        path_clear = True
        for y in range(y_center, img_h):
            if sidewalk_mask[y, x_pixel] != 1:
                path_clear = False
                break

        # אם הדרך פנויה ורחוקה יותר מהקודמת — שמור
        if path_clear and distance > best_distance:
            best_distance = distance
            best_angle    = norm_angle

    return best_angle, best_distance

def angle_to_command(angle):
    # המרת זווית לפקודת תנועה
    if angle is None:
        return 'S'  # עצור אם אין זווית טובה

    if -15 <= angle <= 15:
        return 'F'  # ישר קדימה
    elif angle > 15:
        return 'R'  # ימינה
    else:
        return 'L'  # שמאלה
