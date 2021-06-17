#include "locoDisplayController.h"


#ifndef NEXTLOCOBUTTONCONTROLLER_H
#define NEXTLOCOBUTTONCONTROLLER_H


#define NEXTLOCOBUTTON_PIN D7

class NextLocoButtonController
{
    LocoDisplayController &_locoDisplayController;

    int lastState;

public:
    NextLocoButtonController(LocoDisplayController &locoDisplayController);

    void Setup();
    void ProcessStep();
};

#endif