#include "hornController.h"
#include <Arduino.h>

HornController::HornController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastState = 0;
}

void HornController::Setup()
{
    _qwiicGpio.pinMode(HORN_BUTTON_PIN, GPIO_IN);

    _qwiicGpio.pinMode(HORN_LED_PIN, GPIO_OUT);
    _qwiicGpio.digitalWrite(HORN_LED_PIN, 0);
}

void HornController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(HORN_LED_PIN, currentState.Horn);

    int currentButtonState = _qwiicGpio.digitalRead(HORN_BUTTON_PIN);

    if (currentButtonState != lastState) // Trigger on push down of the button.
    {
        int commandToSend = !currentButtonState;

        //Serial.printf("Sending horn %d\n", commandToSend);
        _mqttHandler.SendHornFor(currentState.RoadName, commandToSend);
    }

    lastState = currentButtonState;
}