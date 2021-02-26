#include "config.h"

#ifndef SMOKEDRIVER_H
#define SMOKEDRIVER_H


#ifdef ARDUINO_ESP8266_NODEMCU
    #define SMOKEPOWER_PIN D1
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define SMOKEPOWER_PIN D5
#endif


#define MIN_SMOKE_PWM             100.0
#define MAX_SMOKE_PWM             1000.0

class SmokeDriver
{
    int _counter;

    bool _smokeDisabled;

public:
    SmokeDriver();

    void Setup();
    void SetSmokePercent(float percent);

    void SetSmokeDisabled(bool disabled);
    bool GetSmokeDisabled();
};


#endif