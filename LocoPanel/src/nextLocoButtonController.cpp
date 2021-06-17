#include "nextLocoButtonController.h"


NextLocoButtonController::NextLocoButtonController(LocoDisplayController &locoDisplayController)
    : _locoDisplayController(locoDisplayController)
{
    lastState = 0;
}

void NextLocoButtonController::Setup()
{
    pinMode(NEXTLOCOBUTTON_PIN, INPUT_PULLUP);      // Pull up the input.  The button will connect the input to ground.
}

void NextLocoButtonController::ProcessStep()
{
    int currentButtonState = digitalRead(NEXTLOCOBUTTON_PIN);

    if (currentButtonState == 0 && lastState == 1)      // Trigger on push down of the button.
    {
        // Button pushed
        //Serial.printf("Button pushed.");
        _locoDisplayController.MoveToNextLoco();
    }

    lastState = currentButtonState;
    //Serial.printf("%d\n", currentButtonState);
}