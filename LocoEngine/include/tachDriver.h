#include <Arduino.h>

#include "config.h"

#ifndef TACHDRIVER_H
#define TACHDRIVER_H


#define LOOP_DELAY                10
#define NUM_MAGNETS               4.0
#define LOW_PASS_FILTER_ALPHA     0.02


#ifdef ARDUINO_ESP8266_NODEMCU
    #define TACH_PIN D6
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define TACH_PIN D6
#endif


class TachDriver
{
    bool hasEverReceivedPulse;

    volatile int measurementState;

    volatile long lastPulseMillis;
    volatile long lastPulseDelta;
    volatile bool receivedPulse;

    float rpm;
    int counterSinceReceivedPulse;

public:
    TachDriver();

    float GetWheelRpm();

    void HandleInterrupt();

    void Setup();
    void ProcessStep();
};


#endif