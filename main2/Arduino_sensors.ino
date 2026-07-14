#include "Arduino_CONFIG.h"

void sensors_setup() {
  pinMode(DOWN_DOWN_ECHO,   INPUT);   pinMode(DOWN_DOWN_TRIG,   OUTPUT);
  pinMode(DOWN_MIDDLE_ECHO, INPUT);   pinMode(DOWN_MIDDLE_TRIG, OUTPUT);
  pinMode(DOWN_UP_ECHO,     INPUT);   pinMode(DOWN_UP_TRIG,     OUTPUT);
  pinMode(UP_DOWN_ECHO,     INPUT);   pinMode(UP_DOWN_TRIG,     OUTPUT);
  pinMode(UP_MIDDLE_ECHO,   INPUT);   pinMode(UP_MIDDLE_TRIG,   OUTPUT);
  pinMode(UP_UP_ECHO,       INPUT);   pinMode(UP_UP_TRIG,       OUTPUT);
}

// מחזיר מרחק בס"מ
int Distance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long d = pulseIn(echoPin, HIGH, 30000);
  if (d == 0) return -1;
  return d * 0.034 / 2;
}

// זיהוי מדרגות
int Stairs() {
  DownDown   = Distance(DOWN_DOWN_TRIG,   DOWN_DOWN_ECHO);
  DownMiddle = Distance(DOWN_MIDDLE_TRIG, DOWN_MIDDLE_ECHO);
  DownUp     = Distance(DOWN_UP_TRIG,     DOWN_UP_ECHO);
  UpDown     = Distance(UP_DOWN_TRIG,     UP_DOWN_ECHO);
  UpMiddle   = Distance(UP_MIDDLE_TRIG,   UP_MIDDLE_ECHO);
  UpUp       = Distance(UP_UP_TRIG,       UP_UP_ECHO);

  int diffLow  = abs(DownMiddle - DownDown);
  int diffHigh = abs(DownUp - DownMiddle);

  if (diffLow > 10 && diffHigh > 10) { stairType = 1; return 1; }
  if (diffLow > 10)                   { stairType = 2; return 2; }

  int diffUpLow  = abs(UpMiddle - UpDown);
  int diffUpHigh = abs(UpUp - UpMiddle);
  if (diffUpLow > 10 && diffUpHigh > 10) { stairType = 3; return 3; }

  stairType = 0;
  return 0;
}
