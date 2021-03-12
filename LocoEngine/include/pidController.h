#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#define P_FACTOR            0.2
#define I_FACTOR            0.01
#define D_FACTOR            0.02

#define MAX_I_TERM          80.0
#define MIN_I_TERM          -20.0
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