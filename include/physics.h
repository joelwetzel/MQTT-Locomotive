#include <math.h>
#include "config.h"

#include "batteryDriver.h"

#ifndef PHYSICS_H
#define PHYSICS_H


#define ENGINE_SPINUP                       0.01
#define ROLLING_RESISTANCE_FACTOR           0.0012
#define AIR_RESISTANCE_FACTOR               0.0000012
#define BRAKING_FACTOR                      0.0001
#define ENGINE_POWER                        0.0000017

#define SMOKE_OVERTHROTTLE_FACTOR           1.0
#define SMOKE_ENGINE_WORK_FACTOR            5.0
#define SMOKE_ACCELERATION_FACTOR           500.0

class Physics
{
    BatteryDriver &_batteryDriver;

    bool _engineOn;
    float _throttle;
    float _brake;
    int _reverserDirection;

    float _engineRpms;
    float _smokePercent;

    float _previousSpeed;
    float _speed;

    void processEngineStep();
    void processResistanceStep();
    void processReverserStep();
    void processSmokeStep();
    void clampSpeed();
    void clampSmoke();

public:
    Physics(BatteryDriver &batteryDriver);

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