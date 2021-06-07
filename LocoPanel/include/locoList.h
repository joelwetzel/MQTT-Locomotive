#include <WString.h>
#include <stdlib.h>
#include <vector>

#ifndef LOCOLIST_H
#define LOCOLIST_H


class LocoList
{
    std::vector<String> roadNames;

public:
    LocoList();

    int GetLocoCount();

    void Clear();
    
    bool AddLoco(String roadName);

    String GetRoadnameByIndex(int index);

    std::vector<String> GetListStartingAtIndex(int index);
};


#endif