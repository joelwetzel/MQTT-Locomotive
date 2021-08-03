#ifndef MASTERSWITCHCONTROLLER_H
#define MASTERSWITCHCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_TCA9534.h>

#define MASTER_SWITCH_ON_BUTTON_PIN 3
#define MASTER_SWITCH_OFF_BUTTON_PIN 1
#define MASTER_SWITCH_LED_PIN 2

class MasterSwitchController
{
    MqttHandler &_mqttHandler;
    TCA9534 &_qwiicGpio;

    int lastOnButtonState;
    int lastOffButtonState;

public:
    MasterSwitchController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif