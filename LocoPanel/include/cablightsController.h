#ifndef CABLIGHTSCONTROLLER_H
#define CABLIGHTSCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define CABLIGHTS_BUTTON_PIN 4
#define CABLIGHTS_LED_PIN 5

class CablightsController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastState;

public:
    CablightsController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif