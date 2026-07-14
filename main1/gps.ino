#include "CONFIG.h"
#include <TinyGPS++.h>

// אובייקט GPS שמפענח את נתוני NMEA
TinyGPSPlus gps;

// ערוץ תקשורת עם מודול GPS
HardwareSerial gpsSerial(1);   // Serial1

// כמה זמן עבר מהבדיקה האחרונה שיש נתונים
unsigned long lastGpsCheck = 0;

// אתחול GPS
void gps_setup() {
  // Serial1 על פינים 4 (RX) ו-17 (TX)
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("[GPS] אותחל");
}

// קריאת נתונים מה-GPS ועדכון מיקום
// Non-Blocking — קורא רק מה שזמין
void gps_update() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());   // מפענח תו אחד
  }

  // אם יש עדכון תקין — שמור מיקום
  if (gps.location.isUpdated() && gps.location.isValid()) {
    currentLat = gps.location.lat();
    currentLon = gps.location.lng();
  }

  // בדיקה כל 5 שניות שיש נתונים
  if (millis() - lastGpsCheck > 5000) {
    if (gps.charsProcessed() < 10) {
      Serial.println("[GPS] אזהרה: אין נתונים מה-GPS");
    }
    lastGpsCheck = millis();
  }
}

// מחזיר רוחב גיאוגרפי נוכחי
double getLatitude()  { return currentLat; }

// מחזיר אורך גיאוגרפי נוכחי
double getLongitude() { return currentLon; }

// בודק אם יש מיקום תקין
bool hasValidLocation() {
  return gps.location.isValid();
}

// חישוב מרחק בין 2 נקודות GPS — במטרים
// נוסחת Haversine
double calcDistance(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000.0;   // רדיוס כדור הארץ במטרים

  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);

  double a = sin(dLat/2) * sin(dLat/2) +
             cos(radians(lat1)) * cos(radians(lat2)) *
             sin(dLon/2) * sin(dLon/2);

  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  return R * c;
}

// חישוב כיוון מנקודה א לנקודה ב — במעלות (0=צפון)
double calcBearing(double lat1, double lon1, double lat2, double lon2) {
  double dLon = radians(lon2 - lon1);
  double y = sin(dLon) * cos(radians(lat2));
  double x = cos(radians(lat1)) * sin(radians(lat2)) -
             sin(radians(lat1)) * cos(radians(lat2)) * cos(dLon);

  double bearing = degrees(atan2(y, x));
  return fmod((bearing + 360), 360);   // תמיד 0-360
}
