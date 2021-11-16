#include <Arduino.h>

#include "config.h"

#ifndef TACHDRIVER_H
#define TACHDRIVER_H


#define LOOP_DELAY                10
#define NUM_MAGNETS               4.0
#define LOW_PASS_FILTER_ALPHA     0.075
#define MAX_NONANOMALOUS_JUMP     100
#define MAX_SANE_RPM              1000


#ifdef ARDUINO_ESP8266_NODEMCU
    #define TACH_PIN D6
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define TACH_PIN D6
#endif


class TachDriver
{
    bool hasEverReceivedPulse;

    volatile int measurementState;

    volatile unsigned long lastPulseMicros;
    volatile unsigned long lastPulseDelta;
    volatile bool receivedPulse;

    float rpm;
    int counterSinceReceivedPulse;
    int numAnomalousReadings;

public:
    TachDriver();

    float GetWheelRpm();

    void HandleInterrupt();

    void Setup();
    void ProcessStep();
};


#endif