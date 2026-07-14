#include "CONFIG.h"

void robot_setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

// נסיעה קדימה
void moveForward() {
  analogWrite(ENA, SPEED_NORMAL);  // מהירות מנוע שמאל
  analogWrite(ENB, SPEED_NORMAL);  // מהירות מנוע ימין
  digitalWrite(IN1, HIGH);         // מנוע שמאל קדימה
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);         // מנוע ימין קדימה
  digitalWrite(IN4, LOW);
  currentMove = 'F';//נסיעה ישר
}

// פנייה שמאלה
void moveLeft() {
  analogWrite(ENA, 0);             // מנוע שמאל עצור
  analogWrite(ENB, SPEED_TURN);    // מנוע ימין בתנועה
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  currentMove = 'L';//נסיעה שמאל
}

// פנייה ימינה
// מנוע שמאל קדימה + מנוע ימין עצור = פנייה ימינה
void moveRight() {
  analogWrite(ENA, SPEED_TURN);    // מנוע שמאל בתנועה
  analogWrite(ENB, 0);             // מנוע ימין עצור
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentMove = 'R';//נסיעה ימין
}

//עצירה
void robotStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentMove = 'S';
}

void travel(char cmd){
  switch (cmd) {
    case 'F': moveForward(); break;
    case 'L': moveLeft();    break;
    case 'R': moveRight();   break;
    case 'S': robotStop();   break;
  }
}