#ifndef CONFIGH
#define CONFIGH

// ==================== IR ====================
#define IR 34

// ==================== חיישני מרחק תחתון — על ארדואינו ====================
#define DOWN_DOWN_ECHO   7
#define DOWN_DOWN_TRIG   8
#define DOWN_MIDDLE_ECHO A4
#define DOWN_MIDDLE_TRIG A5
#define DOWN_UP_ECHO     10
#define DOWN_UP_TRIG     11

// ==================== חיישני מרחק עליון — על ארדואינו ====================
#define UP_DOWN_ECHO     12
#define UP_DOWN_TRIG     13
#define UP_MIDDLE_ECHO   A0
#define UP_MIDDLE_TRIG   A1
#define UP_UP_ECHO       A2
#define UP_UP_TRIG       A3

// ==================== רובוט — על ארדואינו ====================
#define IN1 2
#define IN2 4
#define IN3 5
#define IN4 6
#define ENA 3   // פין PWM ~ מהירות שמאל
#define ENB 9   // פין PWM ~ מהירות ימין

// מהירות רובוט
#define SPEED_NORMAL 80   // מהירות בטוחה לאדם ללכת
#define SPEED_TURN   65   // מהירות בפנייה

// ==================== לחצנים — על ESP32 ====================
#define DISTRESS   25   // לחצן מצוקה — פסיקה
#define LOCA_SING  26   // לחצן מיקום/שירים — פעם/פעמיים
#define POWER_PIN  21   // לחצן הפעלה/כיבוי — פסיקה

// ==================== LIDAR — על ESP32 ====================
#define LIDAR_RX_PIN 16

// ==================== GPS — על ESP32 ====================
#define GPS_RX_PIN 4
#define GPS_TX_PIN 17

// ==================== מיקרופון INMP441 — על ESP32 ====================
#define I2S_SCK 32
#define I2S_WS  33
#define I2S_SD  27

// ==================== SD Card — על ESP32 ====================
#define SD_CS   5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK  18

// ==================== זמנים ====================
#define DOUBLE_PRESS_MS 500   // חלון זמן לזיהוי לחיצה כפולה
#define DEBOUNCE_MS     50    // זמן התעלמות מרעידות לחצן

// ==================== LIDAR קבועים ====================
#define PACKET_SIZE  47       // גודל חבילה מהיצרן
#define HEADER1      0x54     // בייט פתיחה ראשון
#define HEADER2      0x2C     // בייט פתיחה שני
#define LIDAR_BAUD   230400   // מהירות תקשורת
#define MAX_ANGLES   360      // מספר זוויות

// ==================== WiFi ====================
#define WIFI_SSID     "Rimon-Fiber-2.4G-8b4d75"
#define WIFI_PASSWORD "12345678"

// ==================== שרת ====================
#define SERVER_URL "http://192.168.1.100:3000"  // כתובת שרת Node

// ==================== משתנים גלובליים ====================

// מצב המקל
bool Active = false;

// חיישני מרחק תחתון
int DownDown   = 0;
int DownMiddle = 0;
int DownUp     = 0;

// חיישני מרחק עליון
int UpDown   = 0;
int UpMiddle = 0;
int UpUp     = 0;

// מצב מדרגות: 0=אין 1=יורדות 2=מדרגה/בור 3=עולות
int stairType = 0;

// מצב רובוט: S=עצור F=קדימה L=שמאל R=ימין
char currentMove = 'S';

// לחצנים
volatile bool distress     = false;  // volatile כי פסיקה
volatile bool systemActive = false;  // volatile כי פסיקה
bool musicPlaying          = false;

// לחיצה כפולה
unsigned long lastButton = 0;
int buttonCount          = 0;

// LIDAR
uint8_t packetBuffer[PACKET_SIZE];
int     bufferIndex = 0;
float   lidarData[MAX_ANGLES];

// GPS — מיקום נוכחי
double currentLat = 0.0;
double currentLon = 0.0;

// מסלול — נקודות ציון מגוגל
#define MAX_WAYPOINTS 50
double routeLat[MAX_WAYPOINTS];
double routeLon[MAX_WAYPOINTS];
int    routeSize    = 0;   // כמה נקודות יש במסלול
int    currentPoint = 0;   // באיזו נקודה אנחנו עכשיו

// יעד
double targetLat = 0.0;
double targetLon = 0.0;

#endif
