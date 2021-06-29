#include "reverserController.h"
#include <Arduino.h>

ReverserController::ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastPercentage = -1.0;
    lastKnownReverser = -2;

    currentReverserMode = ControllerMode::Unknown;

    currentLocoRoadName = "";
}


void ReverserController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}


float ReverserController::getDetentPercentageForReverser(int reverser)
{
    float detentPercentage = 0.5;

    switch (reverser)
    {
    case -1:
        detentPercentage = REVERSER_DETENT_MINUS_ONE;
        break;
    case 0:
        detentPercentage = REVERSER_DETENT_ZERO;
        break;
    case 1:
        detentPercentage = REVERSER_DETENT_ONE;
        break;
    }

    return detentPercentage;
}


void ReverserController::ProcessStep(LocoState currentLocoState)
{
    // Measure the rotary position.
    uint16_t voltage = _qwiicAdc.getAnalogData(REVERSER_ADC_PIN);
    float percentage = ((float)voltage) / REVERSER_MAX_VOLTAGE_READING;         // Between 0.0 - 1.0
    //Serial.printf("%f\n", percentage);

    // Did the selected loco change?  If so, reset to its reverser position.
    if (currentLocoState.RoadName != currentLocoRoadName)
    {
        //Serial.println("Loco changed.  Resetting...");
        //Serial.println(currentLocoState.RoadName.c_str());
        //Serial.println(currentLocoRoadName.c_str());
        currentReverserMode = ControllerMode::Resetting;
        currentLocoRoadName = currentLocoState.RoadName;
    }

    // Behavior changes, depending on what mode we're in.
    if (currentReverserMode == ControllerMode::Resetting)
    {
        //Serial.println("Resetting");
        float desiredPercentage = getDetentPercentageForReverser(currentLocoState.Reverser);

        if (fabs(percentage - desiredPercentage) < 0.02)
        {
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_CLOCKWISE, 0);
            currentReverserMode = ControllerMode::AcceptingInput;
            lastKnownReverser = currentLocoState.Reverser;
            Serial.println("Now accepting input.");
        }
        else if (percentage > desiredPercentage)
        {
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_COUNTERCLOCKWISE, 255);
        }
        else
        {
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_CLOCKWISE, 255);
        }
    }
    else if (currentReverserMode == ControllerMode::AcceptingInput)
    {
        //Serial.println("Accepting input");
        // Which detent are we closest to?
        int closestDetentsReverser = 0;
        if (percentage < (REVERSER_DETENT_ZERO + REVERSER_DETENT_MINUS_ONE)/2.0)
        {
            closestDetentsReverser = -1;
        }
        else if (percentage > (REVERSER_DETENT_ONE + REVERSER_DETENT_ZERO)/2.0)
        {
            closestDetentsReverser = 1;
        }
        else
        {
            closestDetentsReverser = 0;
        }

        //Serial.printf("closestDetentsReverser: %d\n", closestDetentsReverser);

        // Are we close enough to the detent to be "there?"
        int newReverser = lastKnownReverser;    // Default to last known, so that if this code block doesn't change anything, we don't think we're in a new state.
        float percentageForClosestDetent = getDetentPercentageForReverser(closestDetentsReverser);

        if (fabs(percentage - percentageForClosestDetent) < 0.02)
        {
            newReverser = closestDetentsReverser;       // We arrived at a detent
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_CLOCKWISE, 0);
            _mqttHandler.SendReverserFor(currentLocoState.RoadName, newReverser);
        }
        else if (percentage > percentageForClosestDetent)       // Snap to the closest detent
        {
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_COUNTERCLOCKWISE, 255);
        }
        else                                                    // Snap to the closest detent
        {
            _qwiicMotorDriver.setDrive(REVERSER_MOTOR_NUM, REVERSER_CLOCKWISE, 255);
        }
    }
    else if (currentReverserMode == ControllerMode::Unknown)
    {
        ;   // Do nothing
    }

    lastPercentage = percentage;
}