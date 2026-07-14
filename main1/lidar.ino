#include "CONFIG.h"

// אתחול LIDAR
void lidar_setup() {
  // Serial2 = ערוץ תקשורת שני של ESP32
  // פין 16 = RX, -1 = TX לא בשימוש
  Serial2.begin(LIDAR_BAUD, SERIAL_8N1, LIDAR_RX_PIN, -1);

  // איפוס מערך מרחקים
  for (int i = 0; i < MAX_ANGLES; i++) {
    lidarData[i] = 0.0;
  }
  Serial.println("[LIDAR] אותחל");
}

// פענוח חבילה של 47 בייטים
// כל חבילה = 12 נקודות מדידה
void parsePacket(uint8_t* packet) {
  // חילוץ זוויות התחלה וסיום
  // << 8 = הזזה שמאלה, | = חיבור שני בייטים למספר אחד
  uint16_t start = (packet[5] << 8) | packet[4];
  uint16_t end   = (packet[43] << 8) | packet[42];

  // המרה למעלות — LIDAR שולח בעשיריות מעלה
  float startAngle = start / 100.0;
  float endAngle   = end   / 100.0;

  // הפרש בין 2 הזוויות לחישוב איפה נמצאות 12 הנקודות
  float ang = endAngle - startAngle;

  // אם הפרש שלילי אז עברנו את 360 מעלות, מחזירים לטווח תקין
  if (ang < 0) ang += 360.0;

  // עיבוד 12 הנקודות בחבילה
  for (int i = 0; i < 12; i++) {
    // מתחיל מבייט 6, כל נקודה = 3 בייטים
    int idx = 6 + i * 3;

    // חילוץ מרחק מ-2 בייטים
    uint16_t dist = (packet[idx + 1] << 8) | packet[idx];

    // סינון מרחקים לא תקינים
    if (dist < 30 || dist > 8000) continue;

    // חישוב זווית הנקודה בתוך הטווח
    float angle = startAngle + i * (ang / 11.0);
    if (angle >= 360.0) angle -= 360.0;

    // שמירה במערך כמטרים
    int angleInt = (int)angle;
    lidarData[angleInt] = dist / 1000.0;
  }
}

// קריאת בייטים מ-LIDAR ומרכיבת חבילות
// Non-Blocking — לא חוסם את הלולאה
void lidar_update() {
  while (Serial2.available() > 0) {
    uint8_t byte = Serial2.read();

    // מחפשים בייט פתיחה ראשון
    if (bufferIndex == 0) {
      if (byte == HEADER1) packetBuffer[bufferIndex++] = byte;

    // מחפשים בייט פתיחה שני
    } else if (bufferIndex == 1) {
      if (byte == HEADER2) {
        packetBuffer[bufferIndex++] = byte;
      } else {
        // לא תואם — מתחילים מחדש
        bufferIndex = 0;
        if (byte == HEADER1) packetBuffer[bufferIndex++] = byte;
      }

    // אוספים את שאר הבייטים
    } else {
      packetBuffer[bufferIndex++] = byte;
      if (bufferIndex >= PACKET_SIZE) {
        parsePacket(packetBuffer);
        bufferIndex = 0;
      }
    }
  }
}

// מחזיר מרחק בזווית ספציפית
float getLidarDistance(int angle) {
  if (angle < 0 || angle >= MAX_ANGLES) return 0.0;
  return lidarData[angle];
}

// מחזיר את כל מערך הנתונים לשימוש ב-navigation
float* getLidarData() {
  return lidarData;
}
