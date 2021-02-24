#include <Arduino.h>

#include "config.h"

#ifndef BATTERYDRIVER_H
#define BATTERYDRIVER_H


#ifdef ARDUINO_ESP8266_NODEMCU
    #define TRACK_VOLTAGE_PIN   A0
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define TRACK_VOLTAGE_PIN   A0
#endif


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