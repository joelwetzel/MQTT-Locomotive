#include "muSlaveControlModel.h"

MuSlaveControlModel::MuSlaveControlModel(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _reverserDirection = 0;

    _engineOn = false;
    _engineRpms = 0.0;
    _enginePercent = 0.0;
    _speedPercent = 0.0;
    _wheelRpms = 0.0;
}


int MuSlaveControlModel::GetControlModelId()
{
    return 3;
}


float MuSlaveControlModel::GetSpeedMph()
{
    return GetEstimatedWheelRpms() * WHEEL_RPM_TO_SCALE_MPH;
}

float MuSlaveControlModel::GetSpeedPercent()
{
    return _speedPercent;
}

float MuSlaveControlModel::GetEngineRpms()
{
    return _engineRpms;
}

float MuSlaveControlModel::GetEnginePercent()
{
    return _enginePercent;
}

float MuSlaveControlModel::GetSmokePercent()
{
    return _enginePercent;
}

float MuSlaveControlModel::GetEstimatedWheelRpms()
{
    return _wheelRpms;
}

void MuSlaveControlModel::SetEngineOn(bool on)
{
    _engineOn = on;
}

bool MuSlaveControlModel::GetEngineOn()
{
    return _engineOn;
}


void MuSlaveControlModel::SetThrottle(float throttle)
{
    // Do nothing
}

float MuSlaveControlModel::GetThrottle()
{
    return 0.0;
}

void MuSlaveControlModel::SetBrake(float brake)
{
    // Do nothing
}

float MuSlaveControlModel::GetBrake()
{
    return 0;
}

void MuSlaveControlModel::SetReverser(int direction)
{
    _reverserDirection = direction;
}

int MuSlaveControlModel::GetReverser()
{
    return _reverserDirection;
}

int MuSlaveControlModel::GetDirectionOfTravel()
{
    return 0;
}

void MuSlaveControlModel::OverrideEngineRpms(float rpms)
{
    _engineRpms = rpms;
}

void MuSlaveControlModel::OverrideEnginePercent(float percent)
{
    _enginePercent = percent;
}

void MuSlaveControlModel::OverrideWheelRpms(float rpms)
{
    _wheelRpms = rpms;

    // Workbackwards from the commanded wheel rpms to what speedPercent would be estimated at.
    _speedPercent = (rpms - SPEEDPERCENT_TO_WHEEL_RPMS_INTERCEPT) / SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE;

    if (_speedPercent < 0.0)
    {
        _speedPercent = 0.0;
    }

    // Apply the reverser
    _speedPercent *= _reverserDirection;
}

void MuSlaveControlModel::clampSpeed()
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


void MuSlaveControlModel::ProcessStep()
{
}



