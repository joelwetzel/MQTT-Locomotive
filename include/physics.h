#include <math.h>
#include "config.h"

#ifndef PHYSICS_H
#define PHYSICS_H


#define ENGINE_DELTA                        0.01
#define ROLLING_RESISTANCE_FACTOR           0.0012
#define AIR_RESISTANCE_FACTOR               0.0000012
#define ENGINE_POWER                        0.0000017
#define BRAKING_FACTOR                      0.0001


class Physics
{
    float _deltaT;

    float _throttle;
    float _brake;
    int _reverserDirection;

    float _engine;
    float _speed;


public:
    Physics(float deltaT);

    float GetSpeed();
    float GetEngine();

    void SetThrottle(float throttle);
    float GetThrottle();
    void SetBrake(float brake);
    float GetBrake();
    void SetReverser(int direction);
    int GetReverser();

    void ProcessStep();
};


#endif