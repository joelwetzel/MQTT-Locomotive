#include "masterSwitchController.h"
#include <Arduino.h>

MasterSwitchController::MasterSwitchController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastState = 0;
}

void MasterSwitchController::Setup()
{
    //pinMode(MASTER_SWITCH_BUTTON_PIN, INPUT);
    _qwiicGpio.pinMode(MASTER_SWITCH_BUTTON_PIN, GPIO_IN);

    //pinMode(MASTER_SWITCH_LED_PIN, OUTPUT);
    _qwiicGpio.pinMode(MASTER_SWITCH_LED_PIN, GPIO_OUT);
    //digitalWrite(MASTER_SWITCH_LED_PIN, 0);
    _qwiicGpio.digitalWrite(MASTER_SWITCH_LED_PIN, 0);
}

void MasterSwitchController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(MASTER_SWITCH_LED_PIN, currentState.MasterSwitch);

    int currentButtonState = _qwiicGpio.digitalRead(MASTER_SWITCH_BUTTON_PIN);

    //Serial.printf("%d\n", currentButtonState);

    if (currentButtonState == 0 && lastState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf((currentState.RoadName + "Button pushed. %d\n").c_str(), currentState.MasterSwitch);
        _mqttHandler.SendMasterSwitchFor(currentState.RoadName, !currentState.MasterSwitch);
    }

    lastState = currentButtonState;
}