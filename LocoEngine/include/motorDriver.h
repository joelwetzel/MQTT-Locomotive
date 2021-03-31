#include "config.h"

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H


#ifdef ARDUINO_ESP8266_NODEMCU
    #define MOTORPOWER_PIN D2
    #define MOTORFORWARD_PIN D4
    #define MOTORBACKWARD_PIN D5
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define MOTORFORWARD_PIN D1
    #define MOTORBACKWARD_PIN D2
#endif


class MotorDriver
{
public:
    MotorDriver();

    void Setup();

    void SetMotorSpeed(float percent);
};


#endif