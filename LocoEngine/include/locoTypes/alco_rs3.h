#ifndef ALCO_RS3_H
#define ALCO_RS3_H


// ****************************************************************
// * Smoke configuration
// ****************************************************************
#define SMOKE_OVERTHROTTLE_FACTOR           2.5
#define SMOKE_ENGINE_WORK_FACTOR            13.0
#define SMOKE_ACCELERATION_FACTOR           800.0


// ****************************************************************
// * Control Model configuration
// ****************************************************************
#define ENGINE_RPM_IDLE         315
#define ENGINE_RPM_MAX          1000

#define ENGINE_SPINUP                       0.01
#define ENGINE_POWER                        0.00000018
#define BRAKING_FACTOR                      0.0001


// We try to make a linear prediction of what the "steady-state" wheel rpms would be for a given speedPercent output
// to the motorDriver.  The motors/driver aren't truly linear, but making this prediction does help the PID controller
// converge faster.  These values will be different for different motor components and gearing, different train lengths,
// and also for different PWM config values.  We're just trying to get a good guess.
#define SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE            4.77
#define SPEEDPERCENT_TO_WHEEL_RPMS_INTERCEPT        96.0

// *******************************************************************************************
// * PID Controller configuration - for all factors, higher means more control for that term.
// *******************************************************************************************
#define P_FACTOR            1.10 / SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE
#define I_FACTOR            0.005 / SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE
#define D_FACTOR            0.0
#define MAX_I_TERM          70.0
#define MIN_I_TERM          -70.0
#define MIN_ACTIVE_CONTROL_VALUE    0.3

#endif