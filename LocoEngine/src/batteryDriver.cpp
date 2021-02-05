#include "batteryDriver.h"



BatteryDriver::BatteryDriver()
{
    _voltage = 0.0;
    _masterSwitch = false;
}

void BatteryDriver::Setup()
{
    pinMode(TRACK_VOLTAGE_PIN, INPUT);
}

void BatteryDriver::ProcessStep()
{
    _voltage = analogRead(TRACK_VOLTAGE_PIN) / 1024.0 * 18.5;
}

float BatteryDriver::GetVoltage()
{
    return _voltage;
}

void BatteryDriver::SetMasterSwitch(bool on)
{
    _masterSwitch = on;
}

bool BatteryDriver::GetMasterSwitch()
{
    return _masterSwitch;
}