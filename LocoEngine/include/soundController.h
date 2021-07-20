#include <Arduino.h>

#include "config.h"

#include "batteryDriver.h"

#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H


class SoundController
{
    BatteryDriver &_batteryDriver;

    bool _bell;
    bool _horn;
    int _bellCount;
    int _hornCount;
    
public:
    SoundController(BatteryDriver &batteryDriver);

    void SetBell(bool on);
    bool GetBell();

    void SetHorn(bool on);
    bool GetHorn();

    void Setup();
    void ProcessStep();
};


#endif