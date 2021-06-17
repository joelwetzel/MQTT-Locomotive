#include "locoStateCache.h"
#include <Arduino.h>

LocoState LocoStateCache::GetStateFor(String roadName)
{
    for (int i = 0; i < cachedStates.size(); i++)
    {
        if (cachedStates[i].RoadName == roadName)
        {
            return cachedStates[i];
        }
    }

    // If no match, return a default state.
    return LocoState();
}

void LocoStateCache::SetMasterSwitchFor(String roadName, bool value)
{
    Serial.printf((roadName + "SetMasterSwitchFor %d\n").c_str(), value);

    for (int i = 0; i < cachedStates.size(); i++)
    {
        if (cachedStates[i].RoadName == roadName)
        {
            cachedStates[i].MasterSwitch = value;
            return;
        }
    }

    // If no match, add a new LocoState to the cache.
    LocoState newState;
    newState.RoadName = roadName;
    newState.MasterSwitch = value;

    cachedStates.push_back(newState);
}