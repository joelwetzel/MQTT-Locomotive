#include "iControlModel.h"

#include <math.h>
#include "config.h"

#include "batteryDriver.h"

#ifndef MUSLAVECONTROLMODEL_H
#define MUSLAVECONTROLMODEL_H


class MuSlaveControlModel : public IControlModel
{
    BatteryDriver &_batteryDriver;

    int _reverserDirection;
    int _directionOfTravel;

    bool _engineOn;
    float _engineRpms;
    float _enginePercent;
    float _wheelRpms;
    float _speedPercent;

    void clampSpeed();

public:
    MuSlaveControlModel(BatteryDriver &batteryDriver);
    ~MuSlaveControlModel() {}

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