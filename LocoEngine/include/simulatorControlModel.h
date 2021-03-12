#include "iControlModel.h"

#include <math.h>
#include "config.h"

#include "batteryDriver.h"

#ifndef SIMULATORCONTROLMODEL_H
#define SIMULATORCONTROLMODEL_H


#define ENGINE_SPINUP                       0.01
#define ROLLING_RESISTANCE_FACTOR           0.0012
#define AIR_RESISTANCE_FACTOR               0.0000012
#define BRAKING_FACTOR                      0.0001
#define ENGINE_POWER                        0.0000017

#define SMOKE_OVERTHROTTLE_FACTOR           1.5
#define SMOKE_ENGINE_WORK_FACTOR            7.0
#define SMOKE_ACCELERATION_FACTOR           700.0

class SimulatorControlModel : public IControlModel
{
    BatteryDriver &_batteryDriver;

    bool _engineOn;
    float _throttle;
    float _brake;
    int _reverserDirection;

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

    void ProcessStep();
};


#endif