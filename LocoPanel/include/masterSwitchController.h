#ifndef MASTERSWITCHCONTROLLER_H
#define MASTERSWITCHCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#define MASTER_SWITCH_BUTTON_PIN D3
#define MASTER_SWITCH_LED_PIN D0

class MasterSwitchController
{
    MqttHandler &_mqttHandler;

    int lastState;

public:
    MasterSwitchController(MqttHandler &mqttHandler);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif