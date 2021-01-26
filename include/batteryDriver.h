#include <Arduino.h>

#include "config.h"

#ifndef BATTERYDRIVER_H
#define BATTERYDRIVER_H

#define TRACK_VOLTAGE_PIN   A0

class BatteryDriver
{
    bool _masterSwitch;
    float _voltage;

public:
    BatteryDriver();

    void Setup();
    void ProcessStep();

    float GetVoltage();

    void SetMasterSwitch(bool on);
    bool GetMasterSwitch();
};


#endif