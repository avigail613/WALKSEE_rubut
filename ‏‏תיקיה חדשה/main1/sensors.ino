#include "CONFIG.h"

//אתחול
void sensors_setup()
//הגדרת החיישנים כקלט
  pinMode(IR, INPUT);
  pinMode(DOWN_DOWN_ECHO, INPUT);
  pinMode(DOWN_DOWN_TRIG, OUTPUT);
  pinMode(DOWN_MIDDLE_ECHO, INPUT);
  pinMode(DOWN_MIDDLE_TRIG, OUTPUT);
  pinMode(DOWN_UP_ECHO, INPUT);
  pinMode(DOWN_UP_TRIG, OUTPUT);
  pinMode(UP_DOWN_ECHO, INPUT);
  pinMode(UP_DOWN_TRIG, OUTPUT);
  pinMode(UP_MIDDLE_ECHO, INPUT);
  pinMode(UP_MIDDLE_TRIG, OUTPUT);
  pinMode(UP_UP_ECHO, INPUT);
  pinMode(UP_UP_TRIG, OUTPUT);
}

//בדיקה אם IR מוטה או לא
bool StateIR() {
  //פעיל-מוטה LOW מכובה-ישר -HIGH
  Active = (digitalRead(IR) == LOW);
  return Active;
}

//מחזיר מרחק מחיישן מרחק
int Distance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);//מנקה את הפין-מכבה אותו
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);//שליחת קול
  delayMicroseconds(10);//נהשארתו באויר 10 שניות
  digitalWrite(trigPin, LOW);//כיבוי הרגל
  long distance=pulseIn(echoPin, HIGH);//כמה זמן לקח לקול להגיע לרגל ECHO
  distance*= (0.034 / 2);//מהירות הקול לשניה כפול המרחק לחלק לפעמים של הלוך וחזור
  return distance;
}
//זיהוי מדרגות
int Stairs() {
  //קראית נתוני חיישני מרחק
  DownDown = Distance(DOWN_DOWN_TRIG,DOWN_DOWN_ECHO);
  DownMiddle = Distance(DOWN_MIDDLE_TRIG,DOWN_MIDDLE_ECHO);
  DownUp = Distance(DOWN_UP_TRIG,DOWN_UP_ECHO);
  UpDown = Distance(UP_DOWN_TRIG,UP_DOWN_ECHO);
  UpMiddle = Distance(UP_MIDDLE_TRIG,UP_MIDDLE_ECHO);
  UpUp = Distance(UP_UP_TRIG,UP_UP_ECHO);
  // חישוב הפרשים בין חיישנים תחתונים
  //זיהוי מדרגות יורדות
  if((DownUp-DownMiddle)>10&&(DownMiddle-DownDown)>10){
    stairType = 1;
    return 1;//מדרגות יורדות
  }
  //זיהוי מדרגה אחת יורדת/בור
  if ((DownMiddle-DownDown)>10) {
    stairType = 2;  
    return 2;//מדרגה יורדת
  }
  //זיהוי 
  if ((UpUp-UpMiddle)>10&&(UpMiddle-UpDown) >10){
    stairType = 3;  
    return 3;// מדרגות עולות
  }
  stairType = 0;
  return 0;//אין מדרגה
}