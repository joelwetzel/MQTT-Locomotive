#include "nextLocoButtonController.h"


NextLocoButtonController::NextLocoButtonController(LocoDisplayController &locoDisplayController)
    : _locoDisplayController(locoDisplayController)
{
    lastState = 0;
}

void NextLocoButtonController::Setup()
{
    pinMode(NEXTLOCOBUTTON_PIN, INPUT);
}

void NextLocoButtonController::ProcessStep()
{
    int currentButtonState = digitalRead(NEXTLOCOBUTTON_PIN);

    if (currentButtonState == 1 && lastState == 0)
    {
        // Button pushed
        _locoDisplayController.MoveToNextLoco();
    }

    lastState = currentButtonState;
    //Serial.printf("%d\n", currentButtonState);
}