#ifndef CONFIG_H
#define CONFIG_H

// רובוט 
#define IN1 2
#define IN2 4
#define IN3 5
#define IN4 6
#define ENA 3   // PWM ~ מהירות שמאל
#define ENB 9   // PWM ~ מהירות ימין

#define SPEED_NORMAL 80
#define SPEED_TURN   65

//חיישני מרחק תחתון 
#define DOWN_DOWN_ECHO   7
#define DOWN_DOWN_TRIG   8
#define DOWN_MIDDLE_ECHO A4
#define DOWN_MIDDLE_TRIG A5
#define DOWN_UP_ECHO     10
#define DOWN_UP_TRIG     11

// חיישני מרחק עליון 
#define UP_DOWN_ECHO     12
#define UP_DOWN_TRIG     13
#define UP_MIDDLE_ECHO   A0
#define UP_MIDDLE_TRIG   A1
#define UP_UP_ECHO       A2
#define UP_UP_TRIG       A3

//  משתנים גלובליים 

char currentMove = 'S';

int DownDown   = 0;
int DownMiddle = 0;
int DownUp     = 0;
int UpDown     = 0;
int UpMiddle   = 0;
int UpUp       = 0;

int stairType = 0;

#endif
