#include "masterSwitchController.h"
#include <Arduino.h>

MasterSwitchController::MasterSwitchController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastOnButtonState = 0;
    lastOffButtonState = 0;
}

void MasterSwitchController::Setup()
{
    _qwiicGpio.pinMode(MASTER_SWITCH_ON_BUTTON_PIN, GPIO_IN);
    _qwiicGpio.pinMode(MASTER_SWITCH_OFF_BUTTON_PIN, GPIO_IN);

    _qwiicGpio.pinMode(MASTER_SWITCH_LED_PIN, GPIO_OUT);
    _qwiicGpio.digitalWrite(MASTER_SWITCH_LED_PIN, 0);
}

void MasterSwitchController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(MASTER_SWITCH_LED_PIN, currentState.MasterSwitch);

    int currentOnButtonState = _qwiicGpio.digitalRead(MASTER_SWITCH_ON_BUTTON_PIN);
    //Serial.printf("OnButtonState: %d\n", currentOnButtonState);

    int currentOffButtonState = _qwiicGpio.digitalRead(MASTER_SWITCH_OFF_BUTTON_PIN);
    //Serial.printf("OffButtonState: %d\n", currentOffButtonState);

    if (currentOnButtonState == 0 && lastOnButtonState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf((currentState.RoadName + "On Button pushed.\n").c_str());
        _mqttHandler.SendMasterSwitchFor(currentState.RoadName, true);
    }
    lastOnButtonState = currentOnButtonState;

    if (currentOffButtonState == 0 && lastOffButtonState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf((currentState.RoadName + "Off Button pushed.\n").c_str());
        _mqttHandler.SendMasterSwitchFor(currentState.RoadName, false);
    }
    lastOffButtonState = currentOffButtonState;
}