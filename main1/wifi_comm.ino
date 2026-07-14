#include "CONFIG.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// OpenAI API key לזיהוי דיבור וחישוב מסלול
const char* openAiKey = "YOUR_OPENAI_KEY";

// כתובת מייל חירום
const char* emergencyEmail = "family@gmail.com";

// אתחול WiFi
void wifi_setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("[WiFi] מתחבר");
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(300 / portTICK_PERIOD_MS);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] מחובר!");
}

// שליחת מיקום GPS לשרת Node כל כמה שניות
void wifi_update() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (!hasValidLocation()) return;

  // בונה JSON עם מיקום נוכחי
  HTTPClient http;
  http.begin(String(SERVER_URL) + "/location");
  http.addHeader("Content-Type", "application/json");

  // בניית הודעת JSON
  String body = "{\"lat\":" + String(currentLat, 6) +
                ",\"lon\":" + String(currentLon, 6) + "}";

  http.POST(body);
  http.end();
}

// חישוב מסלול מנקודת מוצא ליעד דרך OSRM
// שומר נקודות במערך routeLat/routeLon
bool calculateRoute(double startLat, double startLon,
                    double endLat,   double endLon) {

  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;

  // OSRM = שירות ניווט חינמי
  String url = "http://router.project-osrm.org/route/v1/walking/";
  url += String(startLon, 6) + "," + String(startLat, 6);
  url += ";";
  url += String(endLon, 6) + "," + String(endLat, 6);
  url += "?overview=simplified&geometries=geojson";

  http.begin(url);
  int code = http.GET();

  if (code != 200) {
    Serial.println("[Route] שגיאה בחישוב מסלול");
    http.end();
    return false;
  }

  String response = http.getString();
  http.end();

  // פענוח JSON
  DynamicJsonDocument doc(8192);
  if (deserializeJson(doc, response) != DeserializationError::Ok) {
    Serial.println("[Route] שגיאה בפענוח JSON");
    return false;
  }

  // שמירת נקודות מסלול
  JsonArray coords = doc["routes"][0]["geometry"]["coordinates"];
  routeSize    = 0;
  currentPoint = 0;

  for (int i = 0; i < coords.size() && i < MAX_WAYPOINTS; i++) {
    routeLon[i] = coords[i][0];   // אורך
    routeLat[i] = coords[i][1];   // רוחב
    routeSize++;
  }

  Serial.print("[Route] נמצאו ");
  Serial.print(routeSize);
  Serial.println(" נקודות מסלול");
  return true;
}

// שליחת מייל חירום כשלוחצים מצוקה
// משתמשת ב-SMTP דרך שרת Node
void sendEmergencyEmail() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(String(SERVER_URL) + "/emergency");
  http.addHeader("Content-Type", "application/json");

  String body = "{\"lat\":" + String(currentLat, 6) +
                ",\"lon\":" + String(currentLon, 6) +
                ",\"email\":\"" + emergencyEmail + "\"}";

  http.POST(body);
  http.end();
  Serial.println("[Emergency] מייל נשלח!");
}
