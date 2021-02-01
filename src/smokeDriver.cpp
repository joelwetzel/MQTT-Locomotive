#include "Arduino.h"
#include "smokeDriver.h"

SmokeDriver::SmokeDriver()
{
    _counter = 0;

    _smokeDisabled = true;
}


void SmokeDriver::Setup()
{
    pinMode(SMOKEPOWER_PIN, OUTPUT);
    // pinMode(SMOKEFORWARD_PIN, OUTPUT);
    // pinMode(SMOKEBACKWARD_PIN, OUTPUT);  

    // digitalWrite(SMOKEFORWARD_PIN, HIGH);
    // digitalWrite(SMOKEBACKWARD_PIN, LOW);
}


void SmokeDriver::SetSmokePercent(float percent)
{
    if (_smokeDisabled)
    {
        analogWrite(SMOKEPOWER_PIN, 0);
        return;
    }

    float cappedPercent = percent;
    if (cappedPercent < 0.0)
    {
        cappedPercent = 0.0;
    }
    else if (cappedPercent > 100.0)
    {
        cappedPercent = 100.0;
    }

    if (_counter > 10)
    {
        if (cappedPercent > 10)
        {
            analogWrite(SMOKEPOWER_PIN, (MIN_SMOKE_PWM + fabs(cappedPercent) / 100.0 * (MAX_SMOKE_PWM - MIN_SMOKE_PWM)));
        }
        else
        {
            analogWrite(SMOKEPOWER_PIN, 0);
        }
        
        _counter = 0;
    }
    _counter++;
}


void SmokeDriver::SetSmokeDisabled(bool disabled)
{
    _smokeDisabled = disabled;
}


bool SmokeDriver::GetSmokeDisabled()
{
    return _smokeDisabled;
}