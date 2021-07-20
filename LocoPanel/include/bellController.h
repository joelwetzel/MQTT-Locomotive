#ifndef BELLCONTROLLER_H
#define BELLCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define BELL_BUTTON_PIN 7
#define BELL_LED_PIN 6

class BellController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastState;

public:
    BellController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif