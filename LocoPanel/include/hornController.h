#ifndef HORNCONTROLLER_H
#define HORNCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define HORN_BUTTON_PIN 3
#define HORN_LED_PIN 2

class HornController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastState;

public:
    HornController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif