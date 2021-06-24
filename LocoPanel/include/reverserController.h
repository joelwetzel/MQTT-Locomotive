#ifndef REVERSERCONTROLLER_H
#define REVERSERCONTROLLER_H

#include <WString.h>

#include "locoState.h"
#include "mqttHandler.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define REVERSER_ADC_PIN 0

#define REVERSER_MAX_VOLTAGE_READING 1660.0

#define REVERSER_CLOCKWISE 0
#define REVERSER_COUNTERCLOCKWISE 1

enum ReverserState
{
    Unknown,
    Resetting,
    AcceptingInput
};


class ReverserController
{
    MqttHandler &_mqttHandler;
    ADS1015 &_qwiicAdc;
    SCMD &_qwiicMotorDriver;

    float lastPercentage;

    ReverserState currentReverserState;

    String currentLocoRoadName;

public:
    ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentLocoState);
};

#endif