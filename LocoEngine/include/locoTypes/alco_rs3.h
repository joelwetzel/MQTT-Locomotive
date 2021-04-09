#ifndef ALCO_RS3_H
#define ALCO_RS3_H



// ****************************************************************
// * MotorDriver configuration - Values are for 16384Hz PWM
// ****************************************************************
#define MIN_SPEED_PWM             575.0
#define MAX_SPEED_PWM             660.0


// ****************************************************************
// * Control Model configuration
// ****************************************************************
#define ENGINE_RPM_IDLE         315
#define ENGINE_RPM_MAX          1000

#define ENGINE_SPINUP                       0.01
#define ENGINE_POWER                        0.00000034
#define BRAKING_FACTOR                      0.0001


// We try to make a linear prediction of what the "steady-state" wheel rpms would be for a given speedPercent output
// to the motorDriver.  The motors/driver aren't truly linear, but making this prediction does help the PID controller
// converge faster.  These values will be different for different motor components and gearing, different train lengths,
// and also for different PWM config values.  We're just trying to get a good guess.
#define SPEEDPERCENT_TO_WHEEL_RPMS_SLOPE            4.77
#define SPEEDPERCENT_TO_WHEEL_RPMS_INTERCEPT        96.0



#endif