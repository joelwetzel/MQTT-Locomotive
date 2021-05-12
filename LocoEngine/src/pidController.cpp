#include "pidController.h"

#include "config.h"
#include <math.h>

PidController::PidController()
{
    _hasBeenInitialized = false;
    _previousMicros = 0;
    _previousError = 0.0;

    _bTerm = 0.0;
    _pTerm = 0.0;
    _iTerm = 0.0;
    _dTerm = 0.0;

    _controlValue = 0.0;
}


void PidController::Update(float setpoint, float processVariableValue, unsigned long currentMicros)
{
    if (!_hasBeenInitialized)
    {
        _previousMicros = currentMicros - 10 * 1000;
        _hasBeenInitialized = true;
    }

    // Units of setpoint and processVariableValue are wheel RPMs.
    // Units of controlValue are motorPercent.
    float error = setpoint - processVariableValue;
    unsigned long dt = currentMicros - _previousMicros;

    // Predict a bias term
    _bTerm = (setpoint - SPEEDPERCENT_TO_WHEEL_RPMS_INTERCEPT) / SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE;
    if (_bTerm < 0.0)
    {
        _bTerm = 0.0;
    }

    // P term is proportional to the error.
    _pTerm = P_FACTOR * error;
    
    _iTerm += I_FACTOR * error; // * ((float)dt / 1000.0 / PHYSICS_DELTAT);

    // Clamp the I term.
    if (_iTerm > MAX_I_TERM)
    {
        _iTerm = MAX_I_TERM;
    }
    else if (_iTerm < MIN_I_TERM)
    {
        _iTerm = MIN_I_TERM;
    }

    // Reset I term if we stop.
    if (setpoint < 0.1)
    {
        _iTerm = 0.0;
    }

    if (dt > 0)
    {
        _dTerm = D_FACTOR * (error - _previousError) / ((float)dt / 1000000.0);
    }
    else
    {   
        _dTerm = 0.0;
    }   

    _controlValue = _bTerm + _pTerm + _iTerm + _dTerm;

    // Clamp the motor percentage.
    if (_controlValue < 0.0)
    {
        _controlValue = 0.0;
    }
    if (_controlValue > 100.0)
    {
        _controlValue = 100.0;
    }

    // Set a minimum power.  If the setpoint is positive, we should never be at zero power.
    // TODO - latching on this.  Must have already been moving.
    if (setpoint > 1.0 && _controlValue < MIN_ACTIVE_CONTROL_VALUE)
    {
        _controlValue = MIN_ACTIVE_CONTROL_VALUE;

        // Reset the I term if we hit the minimum active control value
        // but have positive error and negative I term.   (Meaning we need to speed up.)
        if (error > 0.0 && _iTerm < 0.0)
        {
            _iTerm = 0.0;
        }
    }



    _previousMicros = currentMicros;
    _previousError = error;
}

float PidController::GetBTerm()
{
    return _bTerm;
}

float PidController::GetPTerm()
{
    return _pTerm;
}

float PidController::GetITerm()
{
    return _iTerm;
}

float PidController::GetDTerm()
{
    return _dTerm;
}


float PidController::GetControlValue()
{
    return _controlValue;
}