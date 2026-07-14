// ============================================================
//  camera_stream.ino — ESP32-CAM
//  מצלם תמונות ושולח ל-Python דרך WiFi
//  ESP32-CAM = לוח נפרד עם מצלמה מובנית
// ============================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// ==================== WiFi ====================
const char* ssid     = "Rimon-Fiber-2.4G-8b4d75";
const char* password = "12345678";

// כתובת שרת Python — שנה לIP של המחשב
#define PYTHON_URL "http://192.168.1.100:5000"

// ==================== פינים ESP32-CAM ====================
// אלה הפינים הקבועים של ESP32-CAM — לא לשנות!
#define PWDN_GPIO_NUM   32
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM    0
#define SIOD_GPIO_NUM   26
#define SIOC_GPIO_NUM   27
#define Y9_GPIO_NUM     35
#define Y8_GPIO_NUM     34
#define Y7_GPIO_NUM     39
#define Y6_GPIO_NUM     36
#define Y5_GPIO_NUM     21
#define Y4_GPIO_NUM     19
#define Y3_GPIO_NUM     18
#define Y2_GPIO_NUM      5
#define VSYNC_GPIO_NUM  25
#define HREF_GPIO_NUM   23
#define PCLK_GPIO_NUM   22

// ==================== אתחול מצלמה ====================

bool camera_setup() {
  camera_config_t config;

  // חיבור פינים
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;

  // תדר שעון המצלמה
  config.xclk_freq_hz = 20000000;

  // פורמט JPEG — קטן ומהיר לשליחה
  config.pixel_format = PIXFORMAT_JPEG;

  // גודל VGA = 640x480
  // אם איטי מדי — שנה ל-FRAMESIZE_QVGA (320x240)
  config.frame_size   = FRAMESIZE_VGA;

  // איכות JPEG — 12 = איזון טוב
  // 10=איכות גבוהה יותר, 20=מהיר יותר
  config.jpeg_quality = 12;

  config.fb_count = 1;  // בפר אחד מספיק

  // אתחול המצלמה
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CAM] שגיאה בהפעלה: 0x%x\n", err);
    return false;
  }

  Serial.println("[CAM] מצלמה אותחלה בהצלחה");
  return true;
}

// ==================== שליחת תמונה ====================

// מצלם פריים אחד ושולח ל-Python כ-JPEG
void sendFrameToPython() {

  // צילום פריים מהמצלמה
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[CAM] שגיאה בצילום פריים");
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    esp_camera_fb_return(fb);  // שחרור זיכרון תמיד!
    return;
  }

  HTTPClient http;
  http.begin(String(PYTHON_URL) + "/frame");
  http.addHeader("Content-Type", "image/jpeg");

  // שליחת הבייטים של התמונה
  // fb->buf = מצביע לנתונים, fb->len = גודל בבייטים
  http.POST(fb->buf, fb->len);
  http.end();

  // שחרור זיכרון הפריים — חובה! אחרת יגמר הזיכרון
  esp_camera_fb_return(fb);
}

// ==================== setup() ====================

void setup() {
  Serial.begin(115200);

  // אתחול מצלמה — אם נכשל, מאתחל מחדש
  if (!camera_setup()) {
    delay(1000);
    ESP.restart();
  }

  // חיבור WiFi
  WiFi.begin(ssid, password);
  Serial.print("[WiFi] מתחבר");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] מחובר!");
  Serial.print("[WiFi] כתובת IP: ");
  Serial.println(WiFi.localIP());
}

// ==================== loop() ====================

void loop() {
  // שולח פריים כל 200ms = 5 תמונות בשנייה
  // מספיק לניווט בהליכה רגילה
  sendFrameToPython();
  delay(200);
}
