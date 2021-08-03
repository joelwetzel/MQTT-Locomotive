#ifndef REVERSERCONTROLLER_H
#define REVERSERCONTROLLER_H

#include <WString.h>

#include "locoState.h"
#include "mqttHandler.h"
#include "controllerMode.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define REVERSER_ADC_PIN 1
#define REVERSER_MOTOR_NUM 1

#define REVERSER_MAX_VOLTAGE_READING 1660.0

#define REVERSER_CLOCKWISE 1
#define REVERSER_COUNTERCLOCKWISE 0

#define REVERSER_DETENT_MINUS_ONE 0.1
#define REVERSER_DETENT_ZERO 0.5
#define REVERSER_DETENT_ONE 0.9


class ReverserController
{
    MqttHandler &_mqttHandler;
    ADS1015 &_qwiicAdc;
    SCMD &_qwiicMotorDriver;

    float lastPercentage;
    int lastKnownReverser;

    ControllerMode currentReverserMode;

    String currentLocoRoadName;

    float getDetentPercentageForReverser(int reverser);

public:
    ReverserController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentLocoState);
};

#endif