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


class Physics
{
    BatteryDriver &_batteryDriver;

    float _throttle;
    float _brake;
    int _reverserDirection;

    float _engineRpms;

    float _speed;

public:
    Physics(BatteryDriver &batteryDriver);

    float GetSpeed();
    float GetEngineRpms();

    void SetThrottle(float throttle);
    float GetThrottle();
    void SetBrake(float brake);
    float GetBrake();
    void SetReverser(int direction);
    int GetReverser();

    void ProcessStep();
};


#endif