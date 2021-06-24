#include "reverserController.h"
#include <Arduino.h>

ReverserController::ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastPercentage = -1.0;

    currentReverserState = ReverserState::Unknown;

    currentLocoRoadName = "";
}

void ReverserController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
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
        Serial.println("Loco changed.  Resetting...");
        currentReverserState = ReverserState::Resetting;
        currentLocoRoadName = currentLocoState.RoadName;
    }

    if (currentReverserState == ReverserState::Resetting)
    {
        float desiredPercentage = 0.5;
        switch (currentLocoState.Reverser)
        {
        case -1:
            desiredPercentage = 0.3;
            break;
        case 0:
            desiredPercentage = 0.5;
            break;
        case 1:
            desiredPercentage = 0.7;
            break;
        }

        if (fabs(percentage - desiredPercentage) < 0.02)
        {
            _qwiicMotorDriver.setDrive(0, REVERSER_CLOCKWISE, 0);
            currentReverserState = ReverserState::AcceptingInput;
            Serial.println("Now accepting input.");
        }
        else if (percentage > desiredPercentage)
        {
            _qwiicMotorDriver.setDrive(0, REVERSER_COUNTERCLOCKWISE, 255);
        }
        else
        {
            _qwiicMotorDriver.setDrive(0, REVERSER_CLOCKWISE, 255);
        }
    }
    else if (currentReverserState == ReverserState::AcceptingInput)
    {

    }
    else if (currentReverserState == ReverserState::Unknown)
    {
        ;   // Do nothing
    }

    lastPercentage = percentage;
}