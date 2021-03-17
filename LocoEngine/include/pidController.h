#include "config.h"

#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

// 1.2
#define P_FACTOR            0.4 / SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE

#define T_i                 100.0
#define T_d                 0.012     

#define I_FACTOR            P_FACTOR / T_i
#define D_FACTOR            P_FACTOR * T_d

#define MAX_I_TERM          70.0
#define MIN_I_TERM          -70.0
#define MIN_ACTIVE_CONTROL_VALUE    1.0

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

    float GetControlValue();
};


#endif