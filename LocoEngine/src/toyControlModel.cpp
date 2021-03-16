#include "toyControlModel.h"

ToyControlModel::ToyControlModel(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _throttle = 0.0;
    _reverserDirection = 1;

    _enginePercent = 0.0;
    _smokePercent = 0.0;
    _speedPercent = 0.0;
}


int ToyControlModel::GetControlModelId()
{
    return 2;
}


float ToyControlModel::GetSpeedMph()
{
    return GetEstimatedWheelRpms() * WHEEL_RPM_TO_MPH;
}

float ToyControlModel::GetSpeedPercent()
{
    return _speedPercent;
}

float ToyControlModel::GetEngineRpms()
{
    return ENGINE_RPM_IDLE + (ENGINE_RPM_MAX - ENGINE_RPM_IDLE) * (_enginePercent / 100.0);
}

float ToyControlModel::GetEnginePercent()
{
    return _enginePercent;
}

float ToyControlModel::GetSmokePercent()
{
    return _smokePercent;
}

float ToyControlModel::GetEstimatedWheelRpms()
{
    if (_speedPercent > 0.1)
    {
        return _speedPercent * SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE + SPEEDPERCENT_TO_WHEEL_RPMS_INTERCEPT;
    }
    else
    {
        return 0.0;
    }
}



void ToyControlModel::SetEngineOn(bool on)
{
    // Do nothing
}

bool ToyControlModel::GetEngineOn()
{
    return true;
}


void ToyControlModel::SetThrottle(float throttle)
{
    _throttle = throttle;
}

float ToyControlModel::GetThrottle()
{
    return _throttle;
}

void ToyControlModel::SetBrake(float brake)
{
    // Do nothing
}

float ToyControlModel::GetBrake()
{
    return 0;
}

void ToyControlModel::SetReverser(int direction)
{
    _reverserDirection = direction;
}

int ToyControlModel::GetReverser()
{
    return _reverserDirection;
}


void ToyControlModel::clampSpeed()
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


void ToyControlModel::ProcessStep()
{
    _enginePercent = _throttle;
    _speedPercent = _enginePercent * _reverserDirection;
    _smokePercent = _enginePercent;

    clampSpeed();
}



