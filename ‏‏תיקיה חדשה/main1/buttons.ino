#include "CONFIG.h"

void buttons_setup() {
  pinMode(DISTRESS, INPUT);
  pinMode(LOCA_SING, INPUT);
  pinMode(POWER_PIN, INPUT);

  attachInterrupt(//פסיקה לחצן מצוקה
    digitalPinToInterrupt(DISTRESS),//ממיר לפין של םסיקה
    distressFuncthin, //שליחה לפונקציה לטיפול בפסיקה
    FALLING//ברגע שיש ירידת מתח-לחיצה
  );

void IRAM_ATTR distressFuncthin() {//פונקציה שעוצרת הכל ומטפלת בפסיקת לחצן מצוקה
  emergencyActive = true;   // מדליק דגל מצוקה
  robotStop();              // עוצר מנועים מייד
}

attachInterrupt(//פסיקה הפעלה או כיבוי
  digitalPinToInterrupt(POWER_PIN),//ממיר לפין של פסיקה
  onPower,
  RISING//מתחיל לטפל בפסיקה רק כאשר עוזב את היד מהלחצן
);

void IRAM_ATTR onPower() {//פונקציה שעוצרת הכל ומטפלת בפסיקה
  systemActive = !systemActive;
  if (!systemActive) {
    robotStop();  // כיבוי — עוצר מייד
  }
}

void checkButton() {//בדיקת לחיצה לשירים והשמעת מיקום וכמה זמן נותר להגעה ליעד
  if (digitalRead(LOCA_SING) == HIGH) {//אם לחצו עלייך
    lastButton = millis();//כמה זמן עבר מהלחיצה
    buttonCount++;//סמן לחיצה נוספת

    vTaskDelay(DOUBLE_PRESS_MS / portTICK_PERIOD_MS);//תתחכה  לפי מה שהוגדר בDOUBLE_PRESS_MS
    if (infoPressCount == 1) //המשתמש לא לחץ שוב בזמן הנתון
!!!      Location();//תשלח לפונקציה איפה אתה נמצע
    else 
    if (infoPressCount = 2) {//המשתמש לחץ 2 לחיצות
      if (musicPlaying) {//בדיקה אם השירים דלוקים אם כן
!!!        stopMusic();//תכבה
        musicPlaying = false;//תעדכן שכבוי
      } 
      else {
!!!        playMusic();//תדליק מוזיקה
        musicPlaying = true;//תעדכן
      }
    }
    infoCount = 0;//איפוס מונה הלחציות
  }
}

void buttons_update() {//בדיקת לחצנים
  if (distress) {//אם יש פסיקה
    sendEmergencyEmail();  // שולח מייל
    stopMusic();          
    musicPlaying = false;
    return;                
  }
  checkPowerButton();   // ← חסר! לחצן הפעלה/כיבוי
  checkInfoButton();  // בודק לחיצה בודדת/כפולה
}