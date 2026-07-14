#ifndef CONFIG_H
#define CONFIG_H

//IR
#define IR 34

//מרחק תחתון
#define DOWN_DOWN_ECHO  7   
#define DOWN_DOWN_TRIG  8   
#define DOWN_MIDDLE_ECHO A4  
#define DOWN_MIDDLE_TRIG A5
#define DOWN_UP_ECHO  10   
#define DOWN_UP_TRIG  11  

//מרחק עליון
#define UP_DOWN_ECHO 12  
#define UP_DOWN_TRIG 13   
#define UP_MIDDLE_ECHO A0   
#define UP_MIDDLE_TRIG A1   
#define UP_UP_ECHO   A2   
#define UP_UP_TRIG   A3   

//פינים של הרובוט
#define IN1 2    // כיוון מנוע שמאל — HIGH/LOW
#define IN2 4    // כיוון מנוע שמאל — HIGH/LOW
#define IN3 5    // כיוון מנוע ימין  — HIGH/LOW
#define IN4 6    // כיוון מנוע ימין  — HIGH/LOW
#define ENA 3 // מהירות מנוע שמאל — PWM 0-255
#define ENB 9  // מהירות מנוע ימין  — PWM 0-255

//מהירות רובוט
#define SPEED_NORMAL  80    // מהירות בטוחה לאדם ללכת
#define SPEED_TURN    65    // מהירות בפנייה — איטי יותר לדיוק

#define DISTRESS 21   // לחצן מצוקה — Interrupt
#define LOCA_SING 25   // לחצן שירים/מיקום — פעם/פעמיים  
#define POWER_PIN 26   // לחצן הפעלה/כיבוי מערכת

// חלון זמן לזיהוי לחיצה כפולה — 500 מילישניות
#define DOUBLE_PRESS_MS 500

//פין LIDAR
#define LIDAR_RX_PIN 16

//משתני LIDAR
#define PACKET_SIZE  47      // גודל חבילת LIDAR בבייטים מהיצרן
#define HEADER1      0x54    // בייט פתיחה ראשון קבוע
#define HEADER2      0x2C    // בייט פתיחה שני קבוע
#define LIDAR_BAUD   230400  // מהירות תקשורת LIDAR עם esp32
#define MAX_ANGLES   360     // מספר זוויות מקסימלי

uint8_t packetBuffer[PACKET_SIZE];//מספר שלם חיובי 8 ביטים מערך בגודל 47

int bufferIndex = 0;//מונה בייטים
float lidarData[MAX_ANGLES];//מפת רשת 

// מצב המקל ישר או מוטה
bool Active = false;

//מרחק כל חיישן
int DownDown;   
int DownMiddle;   
int DownUp;   
int UpDown;  
int UpMiddle ;  
int UpUp; 

//מצב מדרגות
// כ0כלום השטח ריק 1 מדרגות יורדות 2 מדרגה אחת יורדת או בור 3 מדרגות עולות 
int stairType = 0;

//מצב רובוט
char currentMove = 'S';//S עוצר f נסיעה קדימה L שמאל R ימין

//פסיקה-לחצן מצוקה
volatile bool distress = false;//בפסיקה צריך להגיד למעבד להסתכל בRAM ולא בריגסטר 

// מצב שירים
bool musicPlaying = false;

// זמן לחיצה אחרונה — לזיהוי לחיצה כפולה
unsigned long lastButton = 0;

// כמה פעמים נלחץ הלחצן בפרק זמן קצר
int buttonCount = 0;

bool systemActive = false;

#endif