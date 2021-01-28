#include "physics.h"

Physics::Physics(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _engineOn = false;
    _throttle = 0.0;
    _brake = 100.0;
    _reverserDirection = 0;

    _engineRpms = 0.0;
    _smokePercent = 0.0;

    _previousSpeed = 0.0;
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

float Physics::GetSmokePercent()
{
    return _smokePercent;
}



void Physics::SetEngineOn(bool on)
{
    if (_batteryDriver.GetMasterSwitch())
    {
        _engineOn = on;
    }
}

bool Physics::GetEngineOn()
{
    return _engineOn;
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


void Physics::processEngineStep()
{
    // Master switch can kill the engine
    if (_batteryDriver.GetMasterSwitch() == false)
    {
        _engineOn = false;
    }

    // Throttle lag
    float engineSpinup = ENGINE_SPINUP * PHYSICS_DELTAT;
    if (_engineOn)
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
        // Spin down the rpms if engine is off
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
}


void Physics::processResistanceStep()
{
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

        if (_speed < 0.0)       // Resistance can't change the direction of travel
        {
            _speed = 0.0;
        }
    }
    else if (_speed < 0.0)
    {
        _speed += resistance * PHYSICS_DELTAT;

        if (_speed > 0.0)       // Resistance can't change the direction of travel
        {
            _speed = 0.0;
        }
    }
}


void Physics::processReverserStep()
{
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


void Physics::processSmokeStep()
{
    float overThrottleSmokePercent = 0.0;
    float engineWorkSmokePercent = 0.0;
    float speedSmokePercent = 0.0;

    // Smoke from throttle being ahead of engine rpms.  Unburnt fuel.
    if (_engineOn && _throttle > _engineRpms)
    {
        overThrottleSmokePercent = (_throttle - _engineRpms) * SMOKE_OVERTHROTTLE_FACTOR;
    }

    // Smoke from the engine doing work, in the physics sense.  It's similar to the equation above for change in speed.
    if (_engineOn)
    {
        engineWorkSmokePercent = sqrt(_engineRpms) * SMOKE_ENGINE_WORK_FACTOR;
    }

    // Make more smoke when the train is accelerating.
    if (_engineOn && _speed > _previousSpeed)
    {
        speedSmokePercent = (_speed - _previousSpeed) * SMOKE_ACCELERATION_FACTOR;
    }

    _smokePercent = overThrottleSmokePercent +
                    engineWorkSmokePercent +
                    speedSmokePercent;
}


void Physics::clampSpeed()
{
    if (_speed > 100.0)
    {
        _speed = 100.0;
    }
    else if (_speed < -100.0)
    {
        _speed = -100.0;
    }
}


void Physics::clampSmoke()
{
    if (_smokePercent > 100.0)
    {
        _smokePercent = 100.0;
    }
    else if (_smokePercent < 0.0)
    {
        _smokePercent = 0.0;
    }
}

void Physics::ProcessStep()
{
    processEngineStep();
    processResistanceStep();
    processReverserStep();
    clampSpeed();
    processSmokeStep();
    clampSmoke();
    
    _previousSpeed = _speed;
}



