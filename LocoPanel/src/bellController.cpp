#include "bellController.h"
#include <Arduino.h>

BellController::BellController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastState = 0;
}

void BellController::Setup()
{
    _qwiicGpio.pinMode(BELL_BUTTON_PIN, GPIO_IN);

    _qwiicGpio.pinMode(BELL_LED_PIN, GPIO_OUT);
    _qwiicGpio.digitalWrite(BELL_LED_PIN, 0);
}

void BellController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(BELL_LED_PIN, currentState.Bell);

    int currentButtonState = _qwiicGpio.digitalRead(BELL_BUTTON_PIN);

    //Serial.printf("%d\n", currentButtonState);

    if (currentButtonState == 0 && lastState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf("Pushed bell button %d\n", !currentState.Bell);
        _mqttHandler.SendBellFor(currentState.RoadName, !currentState.Bell);
    }

    lastState = currentButtonState;
}