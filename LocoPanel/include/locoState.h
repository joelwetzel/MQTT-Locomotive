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
    bool Cablights;

public:
    LocoState()
    {
        MasterSwitch = false;
        EngineOn = false;
        Reverser = 0;
        Headlights = 0;
        Cablights = false;
    }
};

#endif