#include "physics.h"

Physics::Physics(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _throttle = 0.0;
    _brake = 100.0;
    _reverserDirection = 0;

    _engineRpms = 0.0;

    _speed = 0.0;
}


float Physics::GetSpeed()
{
    return _speed;
}

float Physics::GetEngineRpms()
{
    return _engineRpms;
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
    float engineSpinup = ENGINE_SPINUP * PHYSICS_DELTAT;
    if (_batteryDriver.GetMasterSwitch() == true)
    {
        if (_engineRpms < _throttle - engineSpinup/2.0)
        {
            _engineRpms += engineSpinup;
        }
        else if (_engineRpms > _throttle + engineSpinup/2.0)
        {
            _engineRpms -= engineSpinup;
        }
    }
    else
    {
        // Spin down the engine if master switch is off
        _engineRpms -= engineSpinup;
    }    

    // Engine clamping
    if (_engineRpms > 100.0)
    {
        _engineRpms = 100.0;
    }
    else if (_engineRpms < 0.0)
    {
        _engineRpms = 0.0;
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
        _speed -= resistance * PHYSICS_DELTAT;

        if (_speed < 0.0)
        {
            _speed = 0.0;
        }
    }
    else if (_speed < 0.0)
    {
        _speed += resistance * PHYSICS_DELTAT;

        if (_speed > 0.0)
        {
            _speed = 0.0;
        }
    }

    // Apply engine power through reverser
    if (_batteryDriver.GetMasterSwitch() == true)
    {
        if (_engineRpms > 0.0 &&
                (_speed * _reverserDirection > 0 || fabs(_speed) < 0.5))    // The transmission only allows power to be applied if the reverser is in same direction of travel.
        {
            _speed += _reverserDirection * sqrt(ENGINE_POWER * _engineRpms) * PHYSICS_DELTAT;
        }
    }
}



