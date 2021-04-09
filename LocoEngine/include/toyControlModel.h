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
    int _directionOfTravel;

    float _enginePercent;
    float _speedPercent;
    float _smokePercent;

    void clampSpeed();

public:
    ToyControlModel(BatteryDriver &batteryDriver);
    ~ToyControlModel() { }

    int GetControlModelId();
    
    float GetSpeedMph();
    float GetSpeedPercent();

    float GetEngineRpms();
    float GetEnginePercent();
    float GetSmokePercent();
    float GetEstimatedWheelRpms();

    void SetEngineOn(bool on);
    bool GetEngineOn();
    void SetThrottle(float throttle);
    float GetThrottle();
    void SetBrake(float brake);
    float GetBrake();
    void SetReverser(int direction);
    int GetReverser();
    int GetDirectionOfTravel();

    void OverrideEngineRpms(float percent);
    void OverrideEnginePercent(float percent);
    void OverrideWheelRpms(float rpms);

    void ProcessStep();
};


#endif