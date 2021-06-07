#include <WString.h>
#include <stdlib.h>
#include <vector>

#include "config.h"
#include "locoList.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifndef LOCODISPLAYCONTROLLER_H
#define LOCODISPLAYCONTROLLER_H

enum LocoDisplayState
{
    Booting,
    ConnectingToWifi,
    SearchingAndDisplayingLocos
};

class LocoDisplayController
{
    Adafruit_SSD1306 &_display;

    LocoList &_locoList;

    int currentIndex;

    LocoDisplayState currentState;

public:
    LocoDisplayController(LocoList &locoList, Adafruit_SSD1306 &display);

    void Setup();
    void ProcessStep();

    String GetSelectedRoadname();

    void MoveToNextLoco();

    void HandleDisconnected();
    void HandleConnectedToMqtt();
};

#endif