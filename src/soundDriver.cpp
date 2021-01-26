#include "soundDriver.h"


SoundDriver::SoundDriver()
{
    _bell = false;
    _bellCount = 0;
}


void SoundDriver::SetBell(bool on)
{
    _bell = on;
}


bool SoundDriver::GetBell()
{
    return _bell;
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
}
