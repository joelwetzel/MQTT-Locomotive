#include <Arduino.h>

#include "config.h"

#ifndef BATTERYDRIVER_H
#define BATTERYDRIVER_H

#define TRACK_VOLTAGE_PIN   A0

class BatteryDriver
{

public:
    BatteryDriver();

    void Setup();

    float GetVoltage();
};


#endif