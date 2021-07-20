#include "cablightsController.h"
#include <Arduino.h>

CablightsController::CablightsController(MqttHandler &mqttHandler, TCA9534 &qwiicGpio)
    : _mqttHandler(mqttHandler), _qwiicGpio(qwiicGpio)
{
    lastState = 0;
}

void CablightsController::Setup()
{
    _qwiicGpio.pinMode(CABLIGHTS_BUTTON_PIN, GPIO_IN);

    _qwiicGpio.pinMode(CABLIGHTS_LED_PIN, GPIO_OUT);
    _qwiicGpio.digitalWrite(CABLIGHTS_LED_PIN, 0);
}

void CablightsController::ProcessStep(LocoState currentState)
{
    _qwiicGpio.digitalWrite(CABLIGHTS_LED_PIN, currentState.Cablights);

    int currentButtonState = _qwiicGpio.digitalRead(CABLIGHTS_BUTTON_PIN);

    //Serial.printf("%d\n", currentButtonState);

    if (currentButtonState == 0 && lastState == 1) // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf("Cablights %d\n", currentButtonState);
        _mqttHandler.SendCablightsFor(currentState.RoadName, !currentState.Cablights);
    }

    lastState = currentButtonState;
}