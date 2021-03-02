#include "simulatorControlModel.h"

SimulatorControlModel::SimulatorControlModel(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _engineOn = false;
    _throttle = 0.0;
    _brake = 0.0;
    _reverserDirection = 0;

    _enginePercent = 0.0;
    _smokePercent = 0.0;

    _previousSpeedPercent = 0.0;
    _speedPercent = 0.0;
}


int SimulatorControlModel::GetControlModelId()
{
    return 1;
}


float SimulatorControlModel::GetSpeedMph()
{
    return _speedPercent * SPEEDPERCENT_TO_WHEEL_RPMS * WHEEL_RPM_TO_MPH;
}

float SimulatorControlModel::GetSpeedPercent()
{
    return _speedPercent;
}

float SimulatorControlModel::GetEnginePercent()
{
    return _enginePercent;
}

float SimulatorControlModel::GetEngineRpms()
{
    // Convert to RPMs
    if (_engineOn)
    {
        return ENGINE_RPM_IDLE + (ENGINE_RPM_MAX - ENGINE_RPM_IDLE) * (_enginePercent / 100.0);
    }
    else
    {
        // TODO - this doesn't have any spindown.
        return 0.0;
    }
}

float SimulatorControlModel::GetSmokePercent()
{
    return _smokePercent;
}



void SimulatorControlModel::SetEngineOn(bool on)
{
    _engineOn = (on && _batteryDriver.GetMasterSwitch());
}

bool SimulatorControlModel::GetEngineOn()
{
    return _engineOn;
}


void SimulatorControlModel::SetThrottle(float throttle)
{
    _throttle = throttle;
}

float SimulatorControlModel::GetThrottle()
{
    return _throttle;
}

void SimulatorControlModel::SetBrake(float brake)
{
    _brake = brake;
}

float SimulatorControlModel::GetBrake()
{
    return _brake;
}

void SimulatorControlModel::SetReverser(int direction)
{
    _reverserDirection = direction;
}

int SimulatorControlModel::GetReverser()
{
    return _reverserDirection;
}


void SimulatorControlModel::processEngineStep()
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
        if (_enginePercent < _throttle - engineSpinup/2.0)
        {
            _enginePercent += engineSpinup;
        }
        else if (_enginePercent > _throttle + engineSpinup/2.0)
        {
            _enginePercent -= engineSpinup;
        }
    }
    else
    {
        // Spin down the rpms if engine is off
        _enginePercent -= engineSpinup;
    }    

    // Engine clamping
    if (_enginePercent > 100.0)
    {
        _enginePercent = 100.0;
    }
    else if (_enginePercent < 0.0)
    {
        _enginePercent = 0.0;
    }    
}


void SimulatorControlModel::processResistanceStep()
{
    float resistance = 0.0;

    // Air resistance
    resistance += AIR_RESISTANCE_FACTOR * _speedPercent * _speedPercent;

    // Rolling resistance
    resistance += ROLLING_RESISTANCE_FACTOR;

    // Braking resistance
    if (_brake > 0.0)
    {
        resistance += BRAKING_FACTOR * _brake;
    }

    // Apply resistance
    if (_speedPercent > 0.0)
    {
        _speedPercent -= resistance * PHYSICS_DELTAT;

        if (_speedPercent < 0.0)       // Resistance can't change the direction of travel
        {
            _speedPercent = 0.0;
        }
    }
    else if (_speedPercent < 0.0)
    {
        _speedPercent += resistance * PHYSICS_DELTAT;

        if (_speedPercent > 0.0)       // Resistance can't change the direction of travel
        {
            _speedPercent = 0.0;
        }
    }
}


void SimulatorControlModel::processReverserStep()
{
    // Apply engine power through reverser
    if (_batteryDriver.GetMasterSwitch() == true)
    {
        if (_enginePercent > 0.0 &&
                (_speedPercent * _reverserDirection > 0 || fabs(_speedPercent) < 0.5))    // The transmission only allows power to be applied if the reverser is in same direction of travel.
        {
            _speedPercent += _reverserDirection * sqrt(ENGINE_POWER * _enginePercent) * PHYSICS_DELTAT;
        }
    }
}


void SimulatorControlModel::clampSpeed()
{
    if (_speedPercent > 100.0)
    {
        _speedPercent = 100.0;
    }
    else if (_speedPercent < -100.0)
    {
        _speedPercent = -100.0;
    }
}


void SimulatorControlModel::processSmokeStep()
{
    float overThrottleSmokePercent = 0.0;
    float engineWorkSmokePercent = 0.0;
    float speedSmokePercent = 0.0;

    // Smoke from throttle being ahead of engine rpms.  Unburnt fuel.
    if (_engineOn && _throttle > _enginePercent)
    {
        overThrottleSmokePercent = (_throttle - _enginePercent) * SMOKE_OVERTHROTTLE_FACTOR;
    }

    // Smoke from the engine doing work, in the physics sense.  It's similar to the equation above for change in speed.
    if (_engineOn)
    {
        engineWorkSmokePercent = sqrt(_enginePercent) * SMOKE_ENGINE_WORK_FACTOR;
    }

    // Make more smoke when the train is accelerating.
    // if (_engineOn && _speed > _previousSpeed)
    // {
    //     speedSmokePercent = (_speed - _previousSpeed) * SMOKE_ACCELERATION_FACTOR;
    // }

    _smokePercent = overThrottleSmokePercent +
                    engineWorkSmokePercent +
                    speedSmokePercent;

    // Clamp
    if (_smokePercent > 100.0)
    {
        _smokePercent = 100.0;
    }
    else if (_smokePercent < 0.0)
    {
        _smokePercent = 0.0;
    }
}


void SimulatorControlModel::ProcessStep()
{
    processEngineStep();
    processResistanceStep();
    processReverserStep();
    clampSpeed();
    processSmokeStep();
    
    _previousSpeedPercent = _speedPercent;
}



