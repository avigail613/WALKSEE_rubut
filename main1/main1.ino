#include "CONFIG.h"

TaskHandle_t Task_Navigation;
TaskHandle_t Task_Communication;

// ==================== ברירות מחדל ====================

// ברירת מחדל GPS — סמינר בנות בת שבע, מודיעין עילית
#define DEFAULT_LAT 31.9326
#define DEFAULT_LON 35.0456

// ברירת מחדל יעד — קניון מלחה, ירושלים
#define DEFAULT_TARGET_LAT 31.7469
#define DEFAULT_TARGET_LON 35.1889

// ==================== ליבה 1 — ניווט ====================

void navigationTask(void *pvParameters) {
  sensors_setup();
  lidar_setup();
  buttons_setup();
  navigation_setup();

  while (true) {

    // מצוקה
    if (distress) {
      sendToArduino('S');
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    // מערכת כבויה
    if (!systemActive) {
      sendToArduino('S');
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    // מקל ישר
    if (!getCaneActive()) {
      sendToArduino('S');
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    sensors_update();
    lidar_update();
    buttons_update();

    // התרעות מדרגות
    if (stairType == 1) playAudio("stairs_down.wav");
    if (stairType == 2) playAudio("step_down.wav");
    if (stairType == 3) playAudio("stairs_up.wav");
    if (stairType > 0)  stairType = 0;  // איפוס אחרי התרעה

    navigation_update();

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ==================== ליבה 0 — תקשורת ====================

void communicationTask(void *pvParameters) {
  wifi_setup();
  gps_setup();
  bluetooth_setup();
  voice_setup();

  // Serial1 לארדואינו — Serial2 תפוס לLIDAR
  // תקשורת עם ארדואינו דרך SoftwareSerial ב-navigation.ino

  bool waitingForDest = false;

  while (true) {

    // מצוקה — שולח מייל
    if (distress) {
      sendEmergencyEmail();
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      continue;
    }

    // עדכון GPS
    gps_update();

    // ברירת מחדל GPS אם אין מיקום תקין
    if (!hasValidLocation()) {
      currentLat = DEFAULT_LAT;
      currentLon = DEFAULT_LON;
    }

    wifi_update();
    bluetooth_update();

    // מערכת הופעלה — קבל יעד
    if (systemActive && !waitingForDest && routeSize == 0) {
      waitingForDest = true;
      playAudio("say_destination.wav");
      vTaskDelay(500 / portTICK_PERIOD_MS);

      String result = recognizeDestination();

      if (result.length() > 5) {
        // TODO: המרת טקסט לקואורדינטות
        // כרגע ברירת מחדל — קניון מלחה
        targetLat = DEFAULT_TARGET_LAT;
        targetLon = DEFAULT_TARGET_LON;

        if (calculateRoute(currentLat, currentLon,
                           targetLat, targetLon)) {
          playAudio("route_ready.wav");
        } else {
          playAudio("route_error.wav");
          waitingForDest = false;
        }
      } else {
        // לא זוהה קלט — ברירת מחדל קניון מלחה
        Serial.println("[Main] לא זוהה יעד — ברירת מחדל קניון מלחה");
        targetLat = DEFAULT_TARGET_LAT;
        targetLon = DEFAULT_TARGET_LON;
        calculateRoute(currentLat, currentLon, targetLat, targetLon);
        playAudio("default_target.wav");
      }
    }

    if (!systemActive) waitingForDest = false;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// ==================== setup() ====================

void setup() {
  Serial.begin(115200);
  Serial.println(">>> SMART CANE STARTING <<<");

  xTaskCreatePinnedToCore(
    navigationTask, "Navigation",
    10000, NULL, 2, &Task_Navigation, 1
  );

  xTaskCreatePinnedToCore(
    communicationTask, "Communication",
    10000, NULL, 1, &Task_Communication, 0
  );
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
