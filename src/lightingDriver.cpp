#include "lightingDriver.h"

LightingDriver::LightingDriver(Physics &physics, BatteryDriver &batteryDriver)
    : _physics(physics), _batteryDriver(batteryDriver)
{
    _headlightMode = HeadlightModes::Off;
    _cabLightsOn = false;
}


void LightingDriver::SetCabLights(bool on)
{
    _cabLightsOn = on;  
}


bool LightingDriver::GetCabLights()
{
    return _cabLightsOn;
}


void LightingDriver::SetHeadlights(int mode)
{
    switch (mode)
    {
        case 0:
            _headlightMode = HeadlightModes::Off;
            break;
        case 1:
            _headlightMode = HeadlightModes::Front;
            break;
        case 2:
            _headlightMode = HeadlightModes::Rear;
            break;
        case 3:
            _headlightMode = HeadlightModes::Both;
            break;
        case 4:
            _headlightMode = HeadlightModes::Auto;
            break;
    }
}


HeadlightModes LightingDriver::GetHeadlights()
{
    return _headlightMode;
}


void LightingDriver::Setup()
{
    pinMode(CAB_PIN, OUTPUT);
    pinMode(FRONT_PIN, OUTPUT);
    pinMode(REAR_PIN, OUTPUT);
}


void LightingDriver::ProcessStep()
{
    if (_batteryDriver.GetMasterSwitch() == false)
    {
        digitalWrite(CAB_PIN, 0);
        digitalWrite(FRONT_PIN, 0);
        digitalWrite(REAR_PIN, 0);
        return;
    }

    digitalWrite(CAB_PIN, _cabLightsOn);
    
    switch (_headlightMode)
    {
        case HeadlightModes::Off:
            digitalWrite(FRONT_PIN, 0);
            digitalWrite(REAR_PIN, 0);
            break;
        case HeadlightModes::Front:
            digitalWrite(FRONT_PIN, 1);
            digitalWrite(REAR_PIN, 0);
            break;
        case HeadlightModes::Rear:
            digitalWrite(FRONT_PIN, 0);
            digitalWrite(REAR_PIN, 1);
            break;
        case HeadlightModes::Both:
            digitalWrite(FRONT_PIN, 1);
            digitalWrite(REAR_PIN, 1);
            break;
        case HeadlightModes::Auto:
            digitalWrite(FRONT_PIN, _physics.GetReverser() > 0);
            digitalWrite(REAR_PIN, _physics.GetReverser() < 0);
            break;
    }
}