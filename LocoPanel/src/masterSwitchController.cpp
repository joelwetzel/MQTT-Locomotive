#include "masterSwitchController.h"
#include <Arduino.h>

MasterSwitchController::MasterSwitchController(MqttHandler &mqttHandler)
    : _mqttHandler(mqttHandler)
{
    lastState = 0;
}

void MasterSwitchController::Setup()
{
    pinMode(MASTER_SWITCH_BUTTON_PIN, INPUT);

    pinMode(MASTER_SWITCH_LED_PIN, OUTPUT);
    digitalWrite(MASTER_SWITCH_LED_PIN, 0);
}


void MasterSwitchController::ProcessStep(LocoState currentState)
{
    digitalWrite(MASTER_SWITCH_LED_PIN, currentState.MasterSwitch);

    int currentButtonState = digitalRead(MASTER_SWITCH_BUTTON_PIN);

    //Serial.printf("%d\n", currentButtonState);

    if (currentButtonState == 0 && lastState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        Serial.printf((currentState.RoadName + "Button pushed. %d\n").c_str(), currentState.MasterSwitch);
        _mqttHandler.SendMasterSwitchFor(currentState.RoadName, !currentState.MasterSwitch);
    }

    lastState = currentButtonState;
}