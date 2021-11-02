#include "iControlModel.h"

#include <math.h>
#include "config.h"

#include "batteryDriver.h"

#ifndef SIMULATORCONTROLMODEL_H
#define SIMULATORCONTROLMODEL_H


class SimulatorControlModel : public IControlModel
{
    BatteryDriver &_batteryDriver;

    bool _engineOn;
    float _throttle;
    float _brake;
    int _reverserDirection;
    int _directionOfTravel;

    float _enginePercent;
    float _smokePercent;

    float _previousSpeedPercent;
    float _speedPercent;

    void processEngineStep();
    void processResistanceStep();
    void processReverserStep();
    void processSmokeStep();
    void clampSpeed();

public:
    SimulatorControlModel(BatteryDriver &batteryDriver);
    ~SimulatorControlModel() { }

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