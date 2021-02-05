#include "soundDriver.h"


SoundDriver::SoundDriver()
{
    _bell = false;
    _horn = false;
    _bellCount = 0;
    _hornCount = 0;
}


void SoundDriver::SetBell(bool on)
{
    _bell = on;
    _bellCount = 0;

    // TODO - depend on master switch
}


bool SoundDriver::GetBell()
{
    return _bell;
}


void SoundDriver::SetHorn(bool on)
{
    _horn = on;
    _hornCount = 0;

    // TODO - depend on master switch
}


bool SoundDriver::GetHorn()
{
    return _horn;
}


void SoundDriver::Setup()
{
    // TODO - setup pins
}


void SoundDriver::ProcessStep()
{
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
