#ifndef REVERSERCONTROLLER_H
#define REVERSERCONTROLLER_H

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define REVERSER_ADC_PIN 0

#define REVERSER_MAX_VOLTAGE_READING 1660.0

#define REVERSER_CLOCKWISE 0
#define REVERSER_COUNTERCLOCKWISE 1

class ReverserController
{
    MqttHandler &_mqttHandler;
    ADS1015 &_qwiicAdc;
    SCMD &_qwiicMotorDriver;

    //int lastState;

public:
    ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentState);
};

#endif