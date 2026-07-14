#include "CONFIG.h"

void lidar_setup() {
  // LIDAR_RX_PIN = 16, TX לא בשימוש (-1)
  Serial2.begin(LIDAR_BAUD, SERIAL_8N1, LIDAR_RX_PIN, -1);//רץ על מהירות גבוהה

  // איפוס כל המרחקים ל-0
  for (int i = 0; i < MAX_ANGLES; i++) {
    lidarData[i] = 0.0;
  }
}

// מקבלת חבילה מלאה של 47 בייטים ומפענחת אותה
// כל חבילה מכילה 12 נקודות מדידה
void parsePacket(uint8_t* packet) {

  // חילוץ זווית התחלה וסיום מהחבילה
  // << 8 = הזזה שמאלה של 8 ביטים (כפול 256)
  // | = OR בינארי — מחבר שני בייטים למספר אחד וזה הזווית
  uint16_t start = (packet[5] << 8) | packet[4];
  uint16_t end   = (packet[43] << 8) | packet[42];
  // המרה למעלות — LIDAR שולח בעשיריות מעלה
  float startAngle = start/ 100.0;
  float endAngle   = end  / 100.0;

  float ang = endAngle - startAngle;//הפרש בין 2 הזויות לחישוב איפה נמצאות 12 הנקודות
  if (ang < 0) ang += 360.0;//אם הפרש קטן מ0 אז להפוך אותו בחזרה ל0-360
  for (int i = 0; i < 12; i++) {//רץ על כל 12 הנתונים בחבילנה
    int idx = 6 + i * 3;//מתחילים מ6 כי שתיים ראשונים זה לראשי 2-5 זה זוויות התחלה וסיום מתחיל מ6 ואז 3 בייטים כל נקודה
    uint16_t dist = (packet[idx + 1] << 8) | packet[idx];//כל 2 בייטים מחושבים יחד לזווית אחת
    if (dist < 30 || dist > 8000) //סינון מרחקים לא תקינים אם גדול מהטווח או שפחות ממנו
      continue;
    float angle = startAngle + i * (ang/ 11.0);//חישוב זווית הנקודה
    if (angle >= 360.0) //אם עברנו את ה360 מעלות אז להוריד את מה שהעלנו מודם
      angle -= 360.0;
    angle=(int)angle;//עיגול למספר שלם
    lidarData[angle] = dist / 1000.0;//תמיר מיטמרים למטרים ותכניס למפת רשת
  }
}

//קריאת נתונים מהלידר והרכבת חבילות
void lidarUpdate() {

  // קריאת כל הבייטים הזמינים
  while (Serial2.available() > 0) {//קורא את כל הנתונים שחזרו מהלידר
    uint8_t byte = Serial2.read();//שמירת הנתון שחזר מהלידר
    if (bufferIndex == 0) {//אם אתה הראשון בחבילה
      if (byte == HEADER1) {//אם אתה שווה בייט התחלה
        packetBuffer[bufferIndex++] = byte;//תשים  במערך של החבילה ותעלה את המונה
      }

    } else if (bufferIndex == 1) {
      if (byte == HEADER2) {
        packetBuffer[bufferIndex++] = byte;
      } else {
        // לא תואם — מתחילים מחדש
        bufferIndex = 0;
        if (byte == HEADER1) {
          packetBuffer[bufferIndex++] = byte;
        }
      }

    // מצב 2 — אוספים את שאר הבייטים
    } else {
      packetBuffer[bufferIndex++] = byte;

      // חבילה מלאה — פענח אותה
      if (bufferIndex >= PACKET_SIZE) {
        parsePacket(packetBuffer);
        bufferIndex = 0;  // איפוס לחבילה הבאה
      }
    }
  }
}

// Getters 

// מחזיר מרחק בזווית ספציפית
// angle = 0-359 מעלות
float getLidarDistance(int angle) {
  if (angle < 0 || angle >= MAX_ANGLES) return 0.0;
  return lidarData[angle];
}

// מחזיר את כל מערך הנתונים
// navigation.ino ישתמש בזה לחישוב כיוון
float* getLidarData() {
  return lidarData;
}
