#include "headlightsController.h"
#include <Arduino.h>

HeadlightsController::HeadlightsController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastPercentage = -1.0;
    lastKnownHeadlights = -1;

    currentControllerMode = ControllerMode::Unknown;

    currentLocoRoadName = "";
}


void HeadlightsController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}


float HeadlightsController::getDetentPercentageForHeadlights(int headlights)
{
    float detentPercentage = 0.0;

    switch (headlights)
    {
    case 0:
        detentPercentage = HEADLIGHTS_DETENT_ZERO;
        break;
    case 1:
        detentPercentage = HEADLIGHTS_DETENT_ONE;
        break;
    case 2:
        detentPercentage = HEADLIGHTS_DETENT_TWO;
        break;
    case 3:
        detentPercentage = HEADLIGHTS_DETENT_THREE;
        break;
    case 4:
        detentPercentage = HEADLIGHTS_DETENT_FOUR;
        break;
    }

    return detentPercentage;
}


void HeadlightsController::ProcessStep(LocoState currentLocoState)
{
    // Measure the rotary position.
    uint16_t voltage = _qwiicAdc.getAnalogData(HEADLIGHTS_ADC_PIN);
    float percentage = ((float)voltage) / HEADLIGHTS_MAX_VOLTAGE_READING;         // Between 0.0 - 1.0
    //Serial.printf("%f\n", percentage);

    // Did the selected loco change?  If so, reset to its reverser position.
    if (currentLocoState.RoadName != currentLocoRoadName)
    {
        //Serial.println("Loco changed.  Resetting...");
        //Serial.println(currentLocoState.RoadName.c_str());
        //Serial.println(currentLocoRoadName.c_str());
        currentControllerMode = ControllerMode::Resetting;
        currentLocoRoadName = currentLocoState.RoadName;
    }

    // Behavior changes, depending on what mode we're in.
    if (currentControllerMode == ControllerMode::Resetting)
    {
        //Serial.printf("Resetting: %d", currentLocoState.Headlights);
        float desiredPercentage = getDetentPercentageForHeadlights(currentLocoState.Headlights);

        if (fabs(percentage - desiredPercentage) < 0.002)
        {
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_CLOCKWISE, 0);
            currentControllerMode = ControllerMode::AcceptingInput;
            lastKnownHeadlights = currentLocoState.Headlights;
            //Serial.println("Now accepting input.");
        }
        else if (percentage > desiredPercentage)
        {
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_COUNTERCLOCKWISE, 255);
        }
        else
        {
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_CLOCKWISE, 255);
        }
    }
    else if (currentControllerMode == ControllerMode::AcceptingInput)
    {
        //Serial.println("Accepting input");
        // Which detent are we closest to?
        int closestDetentsHeadlights = 0;
        if (percentage < (HEADLIGHTS_DETENT_ZERO + HEADLIGHTS_DETENT_ONE)/2.0)
        {
            closestDetentsHeadlights = 0;
        }
        else if (percentage < (HEADLIGHTS_DETENT_ONE + HEADLIGHTS_DETENT_TWO)/2.0)
        {
            closestDetentsHeadlights = 1;
        }
        else if (percentage < (HEADLIGHTS_DETENT_TWO + HEADLIGHTS_DETENT_THREE)/2.0)
        {
            closestDetentsHeadlights = 2;
        }
        else if (percentage < (HEADLIGHTS_DETENT_THREE + HEADLIGHTS_DETENT_FOUR)/2.0)
        {
            closestDetentsHeadlights = 3;
        }
        else
        {
            closestDetentsHeadlights = 4;
        }

        //Serial.printf("closestDetentsReverser: %d\n", closestDetentsReverser);

        // Are we close enough to the detent to be "there?"
        int newHeadlights = lastKnownHeadlights;    // Default to last known, so that if this code block doesn't change anything, we don't think we're in a new state.
        float percentageForClosestDetent = getDetentPercentageForHeadlights(closestDetentsHeadlights);

        if (fabs(percentage - percentageForClosestDetent) < 0.02)
        {
            newHeadlights = closestDetentsHeadlights;       // We arrived at a detent
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_CLOCKWISE, 0);
            _mqttHandler.SendHeadlightsFor(currentLocoState.RoadName, newHeadlights);
        }
        else if (percentage > percentageForClosestDetent)       // Snap to the closest detent
        {
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_COUNTERCLOCKWISE, 255);
        }
        else                                                    // Snap to the closest detent
        {
            _qwiicMotorDriver.setDrive(HEADLIGHTS_MOTOR_NUM, HEADLIGHTS_CLOCKWISE, 255);
        }
    }
    else if (currentControllerMode == ControllerMode::Unknown)
    {
        ;   // Do nothing
    }

    lastPercentage = percentage;
}