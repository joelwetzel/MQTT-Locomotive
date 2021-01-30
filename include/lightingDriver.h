#include <Arduino.h>

#include "config.h"
#include "physics.h"
#include "batteryDriver.h"

#ifndef LIGHTINGDRIVER_H
#define LIGHTINGDRIVER_H


#define CAB_PIN D6
#define FRONT_PIN D7
#define REAR_PIN D8

enum HeadlightModes
{
    Off,
    Front,
    Rear,
    Both,
    Auto
};


class LightingDriver
{
    Physics &_physics;
    BatteryDriver &_batteryDriver;


    bool _cabLightsOn;
    HeadlightModes _headlightMode;

public:
    LightingDriver(Physics &physics, BatteryDriver &batteryDriver);

    void SetCabLights(bool on);
    bool GetCabLights();
    void SetHeadlights(int mode);
    HeadlightModes GetHeadlights();

    void Setup();
    void ProcessStep();
};


#endif