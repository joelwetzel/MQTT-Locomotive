#include "physics.h"

Physics::Physics(float deltaT)
{
    _deltaT = deltaT;

    _throttle = 0.0;
    _brake = 0.0;
    _reverserDirection = 1;

    _engine = 0.0;
    _speed = 0.0;
}


float Physics::GetSpeed()
{
    return _speed;
}

float Physics::GetEngine()
{
    return _engine;
}

void Physics::SetThrottle(float throttle)
{
    _throttle = throttle;
}

float Physics::GetThrottle()
{
    return _throttle;
}

void Physics::SetBrake(float brake)
{
    _brake = brake;
}

float Physics::GetBrake()
{
    return _brake;
}

void Physics::SetReverser(int direction)
{
    _reverserDirection = direction;
}

int Physics::GetReverser()
{
    return _reverserDirection;
}


void Physics::ProcessStep()
{
    // Throttle lag
    float engineDelta = ENGINE_DELTA * _deltaT;
    if (_engine < _throttle - engineDelta/2.0)
    {
        _engine += engineDelta;
    }
    else if (_engine > _throttle + engineDelta/2.0)
    {
        _engine -= engineDelta;
    }

    // Engine clamping
    if (_engine > 100.0)
    {
        _engine = 100.0;
    }
    else if (_engine < 0.0)
    {
        _engine = 0.0;
    }

    float resistance = 0.0;

    // Air resistance
    resistance += AIR_RESISTANCE_FACTOR * _speed * _speed;

    // Rolling resistance
    resistance += ROLLING_RESISTANCE_FACTOR;

    // Braking resistance
    if (_brake > 0.0)
    {
        resistance += BRAKING_FACTOR * _brake;
    }

    // Apply resistance
    if (_speed > 0.0)
    {
        _speed -= resistance * _deltaT;

        if (_speed < 0.0)
        {
            _speed = 0.0;
        }
    }
    else if (_speed < 0.0)
    {
        _speed += resistance * _deltaT;

        if (_speed > 0.0)
        {
            _speed = 0.0;
        }
    }

    // Apply engine power through reverser
    if (_engine > 0.0 &&
            (_speed * _reverserDirection > 0 || fabs(_speed) < 0.5))    // The transmission only allows power to be applied if the reverser is in same direction of travel.
    {
        _speed += _reverserDirection * sqrt(ENGINE_POWER * _engine) * _deltaT;
    }
}



