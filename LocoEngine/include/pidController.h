#include "config.h"

#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

// 1.2
#define P_FACTOR            2.8 / SPEEDPERCENT_TO_WHEEL_RPMS

#define T_i                 100000000.0
#define T_d                 0.03           

#define I_FACTOR            P_FACTOR / T_i
#define D_FACTOR            P_FACTOR * T_d

#define MAX_I_TERM          80.0
#define MIN_I_TERM          -80.0
#define MIN_ACTIVE_CONTROL_VALUE    5.0

class PidController
{
    bool _hasBeenInitialized;
    unsigned long _previousMicros;
    float _previousError;

    float _pTerm;
    float _iTerm;
    float _dTerm;

    float _controlValue;

public:
    PidController();

    void Update(float setpoint, float processVariableValue, unsigned long currentMicros);

    float GetPTerm();
    float GetITerm();
    float GetDTerm();

    float GetControlValue();
};


#endif