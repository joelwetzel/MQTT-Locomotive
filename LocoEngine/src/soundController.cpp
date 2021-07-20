#include "soundController.h"


SoundController::SoundController(BatteryDriver &batteryDriver)
    : _batteryDriver(batteryDriver)
{
    _bell = false;
    _horn = false;
    _bellCount = 0;
    _hornCount = 0;
}


void SoundController::SetBell(bool on)
{
    _bell = on;
    _bellCount = 0;

    if (!_batteryDriver.GetMasterSwitch())
    {
        _bell = false;
    }
}


bool SoundController::GetBell()
{
    return _bell;
}


void SoundController::SetHorn(bool on)
{
    _horn = on;
    _hornCount = 0;

    if (!_batteryDriver.GetMasterSwitch())
    {
        _horn = false;
    }
}


bool SoundController::GetHorn()
{
    return _horn;
}


void SoundController::Setup()
{
}


void SoundController::ProcessStep()
{
    if (_batteryDriver.GetMasterSwitch() == false)
    {
        _bell = false;
        _horn = false;
    }

    // Turn off the bell after 30 seconds.
    if (_bell)
    {
        _bellCount++;
        if (_bellCount == 3000)
        {
            _bell = false;
            _bellCount = 0;
        }
    }

    // Turn off the bell after 10 seconds.
    if (_horn)
    {
        _hornCount++;
        if (_hornCount == 1000)
        {
            _horn = false;
            _hornCount = 0;
        }
    }
}
