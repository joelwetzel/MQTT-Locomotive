#include "config.h"

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H


#ifdef ARDUINO_ESP8266_NODEMCU
    #define MOTORPOWER_PIN D2
    #define MOTORFORWARD_PIN D4
    #define MOTORBACKWARD_PIN D5
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define MOTORPOWER_PIN D4
    #define MOTORFORWARD_PIN D2
    #define MOTORBACKWARD_PIN D1
#endif

// Values for 16384Hz pwm
#define MIN_SPEED_PWM             560.0
#define MAX_SPEED_PWM             640.0     // 760.0


class MotorDriver
{
public:
    MotorDriver();

    void Setup();

    void SetMotorSpeed(float percent);
};


#endif