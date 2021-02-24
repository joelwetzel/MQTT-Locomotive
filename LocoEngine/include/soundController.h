#include <Arduino.h>

#include "config.h"

#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H


class SoundController
{
    bool _bell;
    bool _horn;
    int _bellCount;
    int _hornCount;
    
public:
    SoundController();

    void SetBell(bool on);
    bool GetBell();

    void SetHorn(bool on);
    bool GetHorn();

    void Setup();
    void ProcessStep();
};


#endif