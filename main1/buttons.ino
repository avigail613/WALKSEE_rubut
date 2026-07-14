#include "CONFIG.h"

unsigned long lastInfoPress = 0;
int           infoPressCount = 0;

void buttons_setup() {
  // נגד חיצוני PULLDOWN — לחיצה = HIGH
  pinMode(DISTRESS,  INPUT);
  pinMode(LOCA_SING, INPUT);
  pinMode(POWER_PIN, INPUT);

  // פסיקת מצוקה
  attachInterrupt(digitalPinToInterrupt(DISTRESS), distressFunction, RISING);

  // פסיקת הפעלה/כיבוי
  attachInterrupt(digitalPinToInterrupt(POWER_PIN), onPower, RISING);

  Serial.println("[BUTTONS] אותחל");
}

// פסיקת מצוקה — רצה מייד, חייב להיות קצרה מאוד
// IRAM_ATTR = שומר בזיכרון מהיר של המעבד
void IRAM_ATTR distressFuncthin() {
  distress = true;
  // לא קוראים robotStop כאן כי הרובוט על ארדואינו
  // הסרד יזהה distress=true וישלח 'S' לארדואינו
}

// פסיקת הפעלה/כיבוי
void IRAM_ATTR onPower() {
  systemActive = !systemActive;
  // כיבוי — הסרד יזהה ויעצור את הרובוט
}

// בדיקת לחצן מיקום/שירים
// לחיצה אחת = הקראת מיקום, כפולה = שירים
void checkButton() {
  if (digitalRead(LOCA_SING) == HIGH) {

    // debounce — מונע קריאות כפולות מרעידות
    if (millis() - lastInfoPress < DEBOUNCE_MS) return;
    lastInfoPress = millis();
    infoPressCount++;

    // ממתין לראות אם תגיע לחיצה נוספת
    vTaskDelay(DOUBLE_PRESS_MS / portTICK_PERIOD_MS);

    if (infoPressCount == 1) {
      announceLocation();   // מוגדר ב-bluetooth.ino

    } else if (infoPressCount >= 2) {
      if (musicPlaying) {
        stopMusic();
        musicPlaying = false;
      } else {
        playMusic();
        musicPlaying = true;
      }
    }

    infoPressCount = 0;
  }
}

// נקראת מ-main בכל סיבוב
void buttons_update() {
  if (distress) {
    sendEmergencyEmail();  // מוגדר ב-wifi_comm.ino
    if (musicPlaying) {
      stopMusic();
      musicPlaying = false;
    }
    return;
  }

  if (systemActive) checkButton();
}

bool getDistress()     { return distress;     }
bool getSystemActive() { return systemActive; }
void resetDistress()   { distress = false;    }
