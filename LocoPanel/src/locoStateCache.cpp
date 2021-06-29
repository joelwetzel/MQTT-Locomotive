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
    //Serial.printf((roadName + "SetMasterSwitchFor %d\n").c_str(), value);

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

void LocoStateCache::SetEngineOnFor(String roadName, bool value)
{
    //Serial.printf((roadName + "SetEngineOnFor %d\n").c_str(), value);

    for (int i = 0; i < cachedStates.size(); i++)
    {
        if (cachedStates[i].RoadName == roadName)
        {
            cachedStates[i].EngineOn = value;
            return;
        }
    }

    // If no match, add a new LocoState to the cache.
    LocoState newState;
    newState.RoadName = roadName;
    newState.EngineOn = value;

    cachedStates.push_back(newState);
}

void LocoStateCache::SetReverserFor(String roadName, int value)
{
    if (value != -1 &&
        value != 0 &&
        value != 1)
    {
        return;
    }

    for (int i = 0; i < cachedStates.size(); i++)
    {
        if (cachedStates[i].RoadName == roadName)
        {
            cachedStates[i].Reverser = value;
            return;
        }
    }

    // If no match, add a new LocoState to the cache.
    LocoState newState;
    newState.RoadName = roadName;
    newState.Reverser = value;

    cachedStates.push_back(newState);
}

void LocoStateCache::SetHeadlightsFor(String roadName, int value)
{
    if (value != 0 &&
        value != 1 &&
        value != 2 &&
        value != 3 &&
        value != 4)
    {
        return;
    }

    for (int i = 0; i < cachedStates.size(); i++)
    {
        if (cachedStates[i].RoadName == roadName)
        {
            cachedStates[i].Headlights = value;
            return;
        }
    }

    // If no match, add a new LocoState to the cache.
    LocoState newState;
    newState.RoadName = roadName;
    newState.Headlights = value;

    cachedStates.push_back(newState);
}