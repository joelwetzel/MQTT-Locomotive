#include "config.h"

#ifndef SMOKEDRIVER_H
#define SMOKEDRIVER_H

#define SMOKEPOWER_PIN D1
// #define SMOKEFORWARD_PIN D1
// #define SMOKEBACKWARD_PIN D0

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