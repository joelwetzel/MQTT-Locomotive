#include "locoList.h"
#include <algorithm>


LocoList::LocoList()
{
}


int LocoList::GetLocoCount()
{
    return roadNames.size();
}


void LocoList::Clear()
{
    roadNames.clear();
}


bool LocoList::AddLoco(String roadName)
{
    if (std::find(roadNames.begin(), roadNames.end(), roadName) != roadNames.end())
    {
        // roadName already in list.  Don't add.
        return false;
    }
    else
    {
        // It's a new roadname.
        roadNames.push_back(roadName);

        return true;
    }
}


String LocoList::GetRoadnameByIndex(int index)
{
    if (index >= roadNames.size())
    {
        return String();
    }

    return roadNames[index];
}


std::vector<String> LocoList::GetListStartingAtIndex(int index)
{
    std::vector<String> result;

    while (result.size() < roadNames.size())
    {
        if (index >= roadNames.size())
        {
            index = 0;
        }

        result.push_back(String(roadNames[index]));

        index++;
    }

    return result;
}