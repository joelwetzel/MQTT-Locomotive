#include "reverserController.h"
#include <Arduino.h>

ReverserController::ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicAdc(qwiicAdc), _qwiicMotorDriver(qwiicMotorDriver)
{
    //lastState = 0;
}

void ReverserController::Setup()
{
    _qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}

void ReverserController::ProcessStep(LocoState currentState)
{
    uint16_t voltage = _qwiicAdc.getAnalogData(REVERSER_ADC_PIN);
    float percentage = ((float)voltage) / REVERSER_MAX_VOLTAGE_READING;         // Between 0.0 - 1.0

    //Serial.printf("%f\n", percentage);

    if (fabs(percentage - 0.5) < 0.01)
    {
        _qwiicMotorDriver.setDrive(0, REVERSER_CLOCKWISE, 0);
    }
    else if (percentage > 0.5)
    {
        _qwiicMotorDriver.setDrive(0, REVERSER_COUNTERCLOCKWISE, 255);
    }
    else
    {
        _qwiicMotorDriver.setDrive(0, REVERSER_CLOCKWISE, 255);
    }
}