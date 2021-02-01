#include "config.h"

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H


#define MOTORPOWER_PIN D2
#define MOTORFORWARD_PIN D4
#define MOTORBACKWARD_PIN D5

// Values for 16384Hz pwm
#define MIN_SPEED_PWM             580.0
#define MAX_SPEED_PWM             760.0

// Values for 12000
//#define MIN_SPEED_PWM             580.0
//#define MAX_SPEED_PWM             760.0


class MotorDriver
{
public:
    MotorDriver();

    void Setup();

    void SetMotorSpeed(float percent);
};


#endif