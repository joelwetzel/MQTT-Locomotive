#include <Arduino.h>

#include "config.h"

#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

class SoundDriver
{
    bool _bell;
    bool _horn;
    int _bellCount;
    int _hornCount;


    
public:
    SoundDriver();

    void SetBell(bool on);
    bool GetBell();

    void SetHorn(bool on);
    bool GetHorn();

    void Setup();
    void ProcessStep();
};


#endif