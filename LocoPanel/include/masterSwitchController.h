#ifndef MASTERSWITCHCONTROLLER_H
#define MASTERSWITCHCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define MASTER_SWITCH_BUTTON_PIN 0
#define MASTER_SWITCH_LED_PIN 1

class MasterSwitchController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastState;

public:
    MasterSwitchController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif