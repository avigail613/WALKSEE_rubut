#include "CONFIG.h"

// SoftwareSerial לתקשורת עם ארדואינו
// Serial1 תפוס לGPS, Serial2 תפוס לLIDAR
// החלף את RX_PIN ו-TX_PIN במספרי הפינים האמיתיים שמחברים לארדואינו

char lastCommand = 'S';

#define OBSTACLE_STOP    0.3   // מטרים — עצור
#define SAFE_DISTANCE    1.5   // מטרים — בטוח לנסוע
#define ARRIVAL_DISTANCE 5.0   // מטרים — הגענו ליעד
#define DEADEND_DISTANCE 0.5   // מטרים — מבוי חסום

void navigation_setup() {
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("[NAV] אותחל");
}

// שליחת פקודה לארדואינו
void sendToArduino(char cmd) {
  if (cmd != lastCommand) {
    Serial2.write(cmd);
    lastCommand = cmd;
  }
}

// קריאת הודעות מארדואינו (התרעות מדרגות)
void readFromArduino() {
  if (Serial2.available() > 0) {
    String msg = ardSerial.readStringUntil('\n');
    // ארדואינו שולח T1/T2/T3 = סוג מדרגה
    if (msg.startsWith("T")) {
      stairType = msg.substring(1).toInt();
    }
  }
}

// בחירת כיוון לפי LIDAR
char chooseBestDirection() {
  float front = getLidarDistance(0);    // קדימה
  float left  = getLidarDistance(315);  // שמאל
  float right = getLidarDistance(45);   // ימין

  if (front > SAFE_DISTANCE || front == 0) return 'F';
  if (front < OBSTACLE_STOP)               return 'S';
  if (left > right)                        return 'L';
  return 'R';
}

// בדיקת מבוי חסום
bool isDeadEnd() {
  int closed = 0;
  for (int a = 0; a < 360; a += 10) {
    float d = getLidarDistance(a);
    if (d > 0 && d < DEADEND_DISTANCE) closed++;
  }
  return closed > 30;
}

// בדיקת הגעה ליעד
bool arrivedAtDestination() {
  return calcDistance(currentLat, currentLon,
                      targetLat, targetLon) < ARRIVAL_DISTANCE;
}

// עדכון ניווט — נקרא מ-main
void navigation_update() {
  readFromArduino();  // קריאת התרעות מארדואינו

  if (routeSize == 0) return;

  // הגענו ליעד
  if (arrivedAtDestination()) {
    playAudio("arrived.wav");
    sendToArduino('S');
    routeSize     = 0;
    systemActive  = false;
    return;
  }

  // עדכון נקודת ציון נוכחית
  if (currentPoint < routeSize) {
    double dist = calcDistance(currentLat, currentLon,
                               routeLat[currentPoint],
                               routeLon[currentPoint]);
    if (dist < 10.0) currentPoint++;
  }

  // מבוי חסום — חשב מחדש
  if (isDeadEnd()) {
    Serial.println("[NAV] מבוי חסום");
    calculateRoute(currentLat, currentLon, targetLat, targetLon);
    return;
  }

  // בחר כיוון ושלח לארדואינו
  sendToArduino(chooseBestDirection());
}
