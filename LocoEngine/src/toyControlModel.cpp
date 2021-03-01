#include "toyControlModel.h"

ToyControlModel::ToyControlModel(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _throttle = 0.0;
    _reverserDirection = 1;

    _enginePercent = 0.0;
    _engineRpms = 0.0;
    _smokePercent = 0.0;
}


float ToyControlModel::GetSpeed()
{
    return _speed;
}

float ToyControlModel::GetEngineRpms()
{
    return _engineRpms;
}

float ToyControlModel::GetEnginePercent()
{
    return _enginePercent;
}

float ToyControlModel::GetSmokePercent()
{
    return _smokePercent;
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
    if (_speed > 100.0)
    {
        _speed = 100.0;
    }
    else if (_speed < -100.0)
    {
        _speed = -100.0;
    }
}


void ToyControlModel::ProcessStep()
{
    _enginePercent = _throttle;
    _engineRpms = ENGINE_RPM_IDLE + (ENGINE_RPM_MAX - ENGINE_RPM_IDLE) * (_enginePercent / 100.0);
    _speed = _engineRpms * _reverserDirection;
    _smokePercent = _engineRpms;

    clampSpeed();
}



