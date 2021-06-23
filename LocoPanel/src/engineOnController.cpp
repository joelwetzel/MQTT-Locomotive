#include "engineOnController.h"
#include <Arduino.h>

EngineOnController::EngineOnController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastState = 0;
}

void EngineOnController::Setup()
{
    _qwiicGpio.pinMode(ENGINE_ON_BUTTON_PIN, GPIO_IN);

    _qwiicGpio.pinMode(ENGINE_ON_LED_PIN, GPIO_OUT);
    _qwiicGpio.digitalWrite(ENGINE_ON_LED_PIN, 0);
}

void EngineOnController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(ENGINE_ON_LED_PIN, currentState.EngineOn);

    int currentButtonState = _qwiicGpio.digitalRead(ENGINE_ON_BUTTON_PIN);

    //Serial.printf("%d\n", currentButtonState);

    if (currentButtonState == 0 && lastState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        _mqttHandler.SendEngineOnFor(currentState.RoadName, !currentState.EngineOn);
    }

    lastState = currentButtonState;
}