#include "config.h"

#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

class PidController
{
    bool _hasBeenInitialized;
    unsigned long _previousMicros;
    float _previousError;

    float _bTerm;
    float _pTerm;
    float _iTerm;
    float _dTerm;

    float _controlValue;

public:
    PidController();

    void Update(float setpoint, float processVariableValue, unsigned long currentMicros);

    float GetBTerm();
    float GetPTerm();
    float GetITerm();
    float GetDTerm();

    float GetError();
    float GetControlValue();
};


#endif