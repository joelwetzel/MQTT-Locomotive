#include "throttleController.h"
#include <Arduino.h>

ThrottleController::ThrottleController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastKnownDetent = -1;

    resetCounter = 0;
    currentThrottleMode = ControllerMode::Unknown;

    currentLocoRoadName = "";
}


void ThrottleController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}


int ThrottleController::convertThrottleToPowerDetent(float throttle)
{
    int detent = map(throttle, 0, 100, 0, POWER_LEVELS);
    detent = constrain(detent, 0, POWER_LEVELS);
    return detent;
}


float ThrottleController::convertPowerDetentToThrottle(int powerDetent)
{
    float throttle = map(powerDetent, 0, POWER_LEVELS, 0, 100);
    throttle = constrain(throttle, 0.0, 100.0);
    return throttle;
}


float ThrottleController::measureThrottle()
{
    // Measure the slider position.
    uint16_t voltage = _qwiicAdc.getAnalogData(THROTTLE_ADC_PIN);
    //Serial.printf("voltage: %d\n", voltage);

    float measuredThrottle = ((float)voltage) / THROTTLE_MAX_VOLTAGE_READING * 100.0; // Between 0.0 - 100.0
    measuredThrottle = constrain(measuredThrottle, 0.0, 100.0);

    return measuredThrottle;
}


void ThrottleController::ProcessStep(LocoState currentLocoState)
{
    // Measure the slider position.
    float measuredThrottle = measureThrottle();

    // Did the selected loco change?  If so, reset the throttle to match the new loco.
    if (currentLocoState.RoadName != currentLocoRoadName)
    {
        Serial.println("Loco changed.  Resetting...");
        //Serial.println(currentLocoState.RoadName.c_str());
        //Serial.println(currentLocoRoadName.c_str());
        currentThrottleMode = ControllerMode::Resetting;
        currentLocoRoadName = currentLocoState.RoadName;
        resetCounter = 0;
    }

    // Behavior changes, depending on what mode we're in.
    if (currentThrottleMode == ControllerMode::Resetting)
    {
        // Serial.println("Resetting...");

        float desiredThrottle = currentLocoState.Throttle;
        float delta = fabs(measuredThrottle - desiredThrottle);
        uint8_t closingSpeed = 255;
        if (delta < 20.0)
        {
            closingSpeed = 210;
        }

        // Serial.printf("%f %f\n", desiredThrottle, measuredThrottle);

        if (delta < 2.0 || resetCounter > 50)
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, 0);
            currentThrottleMode = ControllerMode::AcceptingInput;
            Serial.println("Accepting input...");
            //Serial.printf("%f\n", measuredThrottle);
            lastKnownDetent = convertThrottleToPowerDetent(desiredThrottle);
        }
        else if (desiredThrottle > measuredThrottle)
        {
            // Serial.printf("+ %f %f\n", desiredThrottle, measuredThrottle);
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, closingSpeed);
        }
        else if (desiredThrottle < measuredThrottle)
        {
            // Serial.printf("- %f %f\n", desiredThrottle, measuredThrottle);
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_DOWN, closingSpeed);
        }

        resetCounter++;
    }
    else if (currentThrottleMode == ControllerMode::AcceptingInput)
    {
        // Snap to a power level detent
        int closestDetent = convertThrottleToPowerDetent(measuredThrottle);
        float throttleOfClosestDetent = convertPowerDetentToThrottle(closestDetent);

//        Serial.printf("%f, %d, %f \n", measuredThrottle, closestPowerLevel, throttleOfClosestPowerLevel);
        
        // Are we close enough to the detent to be "there"?
        float offset = fabs(measuredThrottle - throttleOfClosestDetent);
        int detentPower = map((int)offset, 0, 6, 220, 235);

        if (offset < 1.5)
        {
            // We're at a detent
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, 0);
            // Serial.println("detent");

            // If it's a different detent than before
            if (closestDetent != lastKnownDetent)
            {
                _mqttHandler.SendThrottleFor(currentLocoRoadName, throttleOfClosestDetent);
                lastKnownDetent = closestDetent;
            }
        }
        else if (measuredThrottle > throttleOfClosestDetent)
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_DOWN, detentPower);
            // Serial.printf("down %d\n", detentPower);
        }
        else
        {
            _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, detentPower);
            // Serial.printf("up %d\n", detentPower);
        }
    }
    else if (currentThrottleMode == ControllerMode::Unknown)
    {
        // Serial.println("Unknown...");
        ; // Do nothing
    }
}