#include <Arduino.h>

#include "config.h"

#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

class SoundDriver
{
    bool _bell;

    int _bellCount;
    
public:
    SoundDriver();

    void SetBell(bool on);
    bool GetBell();

    void Setup();
    void ProcessStep();
};


#endif