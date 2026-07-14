//  main.ino — לוגיקה ראשית
//  מחלק עבודה בין 2 ליבות של ESP32:
//  ליבה 0 = תקשורת (WiFi, GPS, Bluetooth)
//  ליבה 1 = בטיחות ותנועה (חיישנים, רובוט, לחצנים)

#include "CONFIG.h"

//הגדרות סרדים 

// TaskHandle = מזהה של סרד — מאפשר לנהל אותו
TaskHandle_t Task_Navigation;    // סרד ניווט — ליבה 1
TaskHandle_t Task_Communication; // סרד תקשורת — ליבה 0

//  סרד ניווט — ליבה 1 

// פונקציה שרצה ללא הפסקה על ליבה 1
// אחראית על: חיישנים, רובוט, לחצנים, ניווט
// pvParameters = פרמטרים (לא בשימוש, חובה לציין)
void navigationTask(void *pvParameters) {

  // אתחול כל הרכיבים של ליבה 1
  sensors_setup();
  robot_setup();
  buttons_setup();
  lidar_setup();

  // לולאה אינסופית — כמו loop() אבל על ליבה 1
  while (true) {

    // אם מצוקה — עוצר הכל ולא ממשיך
    if (getEmergencyActive()) {
      robotStop();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;  // חוזר לתחילת הלולאה
    }

    // אם המקל ישר — לא עובד
    if (!getCaneActive()) {
      robotStop();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    // עדכון כל הרכיבים
    sensors_update();   // קריאת IR + חיישרי מרחק
    lidar_update();     // קריאת LIDAR
    buttons_update();   // בדיקת לחצנים
    navigation_update();// חישוב כיוון + פקודה לרובוט

    // בדיקת מדרגות — אם יש, שלח התרעה באוזניה
    int stair = getStairType();
    if (stair == 1) playAudio("stairs_down.wav");
    if (stair == 2) playAudio("step_down.wav");
    if (stair == 3) playAudio("stairs_up.wav");

    // vTaskDelay = המתנה ללא חסימה
    // 10ms = 100 בדיקות בשנייה
    // portTICK_PERIOD_MS = המרת מילישניות לתקתוקי מערכת
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

//סרד תקשורת — ליבה 0 

// פונקציה שרצה ללא הפסקה על ליבה 0
// אחראית על: WiFi, GPS, Bluetooth, זיהוי דיבור
void communicationTask(void *pvParameters) {

  // אתחול כל רכיבי התקשורת
  wifi_setup();
  gps_setup();
  bluetooth_setup();

  while (true) {

    gps_update();          // קריאת מיקום GPS
    wifi_update();         // שליחת מיקום לשרת
    bluetooth_update();    // ניהול אוזניה

    // תקשורת פחות דחופה — עדכון כל 100ms
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// ==================== setup() — רץ פעם אחת ====================

void setup() {
  // Serial חייב להיות ראשון לפני הכל
  Serial.begin(115200);
  Serial.println(">>> SMART CANE STARTING <<<");

  // יצירת סרד ניווט על ליבה 1
  // xTaskCreatePinnedToCore = יוצר סרד על ליבה ספציפית
  xTaskCreatePinnedToCore(
    navigationTask,     // פונקציה לרוץ
    "Navigation",       // שם לדיבאג
    10000,              // גודל מחסנית בבייטים
    NULL,               // פרמטרים (אין)
    2,                  // עדיפות — 2 = גבוה (בטיחות!)
    &Task_Navigation,   // מזהה הסרד
    1                   // ליבה 1
  );

  // יצירת סרד תקשורת על ליבה 0
  xTaskCreatePinnedToCore(
    communicationTask,
    "Communication",
    10000,
    NULL,
    1,                  // עדיפות — 1 = רגיל
    &Task_Communication,
    0                   // ליבה 0
  );

  Serial.println(">>> שני סרדים פועלים <<<");
}

// ==================== loop() — ריק! ====================

// כשעובדים עם סרדים — loop() נשאר ריק
// כל העבודה נעשית בסרדים
void loop() {
  // ריק בכוונה — הסרדים עושים הכל
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
