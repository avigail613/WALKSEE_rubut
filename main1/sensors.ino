#include "CONFIG.h"

// אתחול חיישן IR בלבד — חיישני מרחק על ארדואינו
void sensors_setup() {
  // פין 34 הוא input-only, לא תומך INPUT_PULLUP
  pinMode(IR, INPUT);
}

// בדיקת מצב המקל
// LOW = מוטה = פעיל, HIGH = ישר = כבוי
bool StateIR() {
  Active = (digitalRead(IR) == LOW);
  return Active;
}

// עדכון — נקרא מ-main בכל סיבוב
void sensors_update() {
  StateIR();
}

bool getCaneActive() { return Active; }
int  getStairType()  { return stairType; }
