#include "throttleController.h"
#include <Arduino.h>

ThrottleController::ThrottleController(MqttHandler &mqttHandler, SCMD &qwiicMotorDriver)
    : _mqttHandler(mqttHandler), _qwiicMotorDriver(qwiicMotorDriver)
{
    lastPercentage = -1.0;

    currentThrottleMode = ControllerMode::Unknown;

    currentLocoRoadName = "";

    counter = 0;
}

void ThrottleController::Setup()
{
    //_qwiicAdc.setGain(ADS1015_CONFIG_PGA_1);        // This gain will get us readings from 0..1660
}


void ThrottleController::ProcessStep(LocoState currentLocoState)
{
    if (counter < 100)
    {
        _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_UP, 255);
        Serial.println("Up...");
    }
    else
    {
        _qwiicMotorDriver.setDrive(THROTTLE_MOTOR_NUM, THROTTLE_DOWN, 255);
        Serial.println("Down...");
    }

    counter++;

    if (counter == 200)
    {
        counter = 0;
    }
}