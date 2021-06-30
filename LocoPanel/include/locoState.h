#ifndef LOCOSTATE_H
#define LOCOSTATE_H

#include <WString.h>

struct LocoState
{
    String RoadName;
    bool MasterSwitch;
    bool EngineOn;
    int Reverser;
    int Headlights;

public:
    LocoState()
    {
        MasterSwitch = false;
        EngineOn = false;
        Reverser = 0;
        Headlights = 0;
    }
};

#endif