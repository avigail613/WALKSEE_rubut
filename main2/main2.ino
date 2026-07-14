#include "Arduino_CONFIG.h"

unsigned long lastStairCheck = 0;

void setup() {
  // Serial לתקשורת עם ESP32
  // לא מדפיסים לדיבאג על Serial כי ESP32 קורא אותו!
  Serial.begin(9600);
  robot_setup();
  sensors_setup();
}

void loop() {

  // קבלת פקודת תנועה מ-ESP32
  // F=קדימה, L=שמאל, R=ימין, S=עצור
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    travel(cmd);
  }

  // בדיקת מדרגות כל 200ms
  // לא כל סיבוב כי pulseIn לוקח זמן
  if (millis() - lastStairCheck > 200) {
    lastStairCheck = millis();
    int stair = Stairs();

    // שליחת סוג מדרגה ל-ESP32
    // ESP32 יפעיל התרעה קולית
    if (stair > 0) {
      Serial.print('T');
      Serial.println(stair);  // T1, T2, או T3
    }
  }
}
