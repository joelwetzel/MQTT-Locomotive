#ifndef LOCOSTATECACHE_H
#define LOCOSTATECACHE_H

#include "locoState.h"
#include <WString.h>
#include <stdlib.h>
#include <vector>

class LocoStateCache
{
    std::vector<LocoState> cachedStates;

public:
    LocoState GetStateFor(String roadName);

    void SetMasterSwitchFor(String roadName, bool value);
    void SetEngineOnFor(String roadName, bool value);
    void SetReverserFor(String roadName, int value);
};

#endif