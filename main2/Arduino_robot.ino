#include "Arduino_CONFIG.h"

void robot_setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  robotStop();   // וודא עצירה בהתחלה
}

// נסיעה קדימה
void moveForward() {
  analogWrite(ENA, SPEED_NORMAL);
  analogWrite(ENB, SPEED_NORMAL);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  currentMove = 'F';
}

// פנייה שמאלה — מנוע שמאל עצור, ימין קדימה
void moveLeft() {
  analogWrite(ENA, 0);
  analogWrite(ENB, SPEED_TURN);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  currentMove = 'L';
}

// פנייה ימינה — מנוע שמאל קדימה, ימין עצור
void moveRight() {
  analogWrite(ENA, SPEED_TURN);
  analogWrite(ENB, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentMove = 'R';
}

// עצירה מלאה
void robotStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentMove = 'S';
}

// ביצוע פקודה שהגיעה מ-ESP32
void travel(char cmd) {
  switch (cmd) {
    case 'F': moveForward(); break;
    case 'L': moveLeft();    break;
    case 'R': moveRight();   break;
    case 'S': robotStop();   break;
  }
}
