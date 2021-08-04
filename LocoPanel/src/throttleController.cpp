#include "throttleController.h"
#include <Arduino.h>

ThrottleController::ThrottleController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastThrottle = -1.0;
    lastSentThrottle = -2.0;

    currentThrottleMode = ControllerMode::Unknown;

    currentLocoRoadName = "";
}


void ThrottleController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}


void ThrottleController::ProcessStep(LocoState currentLocoState)
{
    // Measure the slider position.
    uint16_t voltage = _qwiicAdc.getAnalogData(THROTTLE_ADC_PIN);
    float measuredThrottle = ((float)voltage) / THROTTLE_MAX_VOLTAGE_READING * 100.0;         // Between 0.0 - 100.0
    //Serial.printf("%f\n", measuredThrottle);

    // Did the selected loco change?  If so, reset the throttle to match the new loco.
    if (currentLocoState.RoadName != currentLocoRoadName)
    {
        //Serial.println("Loco changed.  Resetting...");
        //Serial.println(currentLocoState.RoadName.c_str());
        //Serial.println(currentLocoRoadName.c_str());
        currentThrottleMode = ControllerMode::Resetting;
        currentLocoRoadName = currentLocoState.RoadName;
    }

    // Behavior changes, depending on what mode we're in.
    if (currentThrottleMode == ControllerMode::Resetting)
    {
        float desiredThrottle = currentLocoState.Throttle;

        if (fabs(measuredThrottle - desiredThrottle) < 0.02)
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, 0);
            currentThrottleMode = ControllerMode::AcceptingInput;
            lastSentThrottle = desiredThrottle;
        }
        else if (desiredThrottle > measuredThrottle)
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, 255);
        }
        else
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_DOWN, 255);
        }
    }
    else if (currentThrottleMode == ControllerMode::AcceptingInput)
    {
        if (fabs(measuredThrottle - lastSentThrottle) > 0.5)
        {
            _mqttHandler.SendThrottleFor(currentLocoRoadName, measuredThrottle);
            lastSentThrottle = measuredThrottle;
        }
    }
    else if (currentThrottleMode == ControllerMode::Unknown)
    {
        ;   // Do nothing
    }
}