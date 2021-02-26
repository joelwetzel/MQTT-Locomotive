#include "iControlModel.h"

#include <math.h>
#include "config.h"

#include "batteryDriver.h"

#ifndef TOYCONTROLMODEL_H
#define TOYCONTROLMODEL_H


class ToyControlModel : public IControlModel
{
    BatteryDriver &_batteryDriver;

    float _throttle;
    int _reverserDirection;

    float _engineRpms;
    float _speed;
    float _smokePercent;

    void clampSpeed();

public:
    ToyControlModel(BatteryDriver &batteryDriver);
    ~ToyControlModel() { }

    float GetSpeed();
    float GetEngineRpms();
    float GetSmokePercent();

    void SetEngineOn(bool on);
    bool GetEngineOn();
    void SetThrottle(float throttle);
    float GetThrottle();
    void SetBrake(float brake);
    float GetBrake();
    void SetReverser(int direction);
    int GetReverser();

    void ProcessStep();
};


#endif