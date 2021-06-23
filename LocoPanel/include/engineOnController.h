#ifndef ENGINEONCONTROLLER_H
#define ENGINEONCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define ENGINE_ON_BUTTON_PIN 2
#define ENGINE_ON_LED_PIN 3

class EngineOnController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastState;

public:
    EngineOnController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif