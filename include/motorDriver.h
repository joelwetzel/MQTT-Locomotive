#include "config.h"

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H


#define ENA D2
#define IN1 D3
#define IN2 D4

#define MIN_SPEED_PWM             580.0
#define MAX_SPEED_PWM             800.0

class MotorDriver
{
public:
    MotorDriver();

    void SetMotorSpeed(float percent);
};


#endif