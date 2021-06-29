#ifndef HEADLIGHTSCONTROLLER_H
#define HEADLIGHTSCONTROLLER_H

#include <WString.h>

#include "locoState.h"
#include "mqttHandler.h"
#include "controllerMode.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define HEADLIGHTS_ADC_PIN 0
#define HEADLIGHTS_MOTOR_NUM 0

#define HEADLIGHTS_MAX_VOLTAGE_READING 1660.0

#define HEADLIGHTS_CLOCKWISE 1
#define HEADLIGHTS_COUNTERCLOCKWISE 0

#define HEADLIGHTS_DETENT_ZERO 0.1
#define HEADLIGHTS_DETENT_ONE 0.3
#define HEADLIGHTS_DETENT_TWO 0.5
#define HEADLIGHTS_DETENT_THREE 0.7
#define HEADLIGHTS_DETENT_FOUR 0.9


class HeadlightsController
{
    MqttHandler &_mqttHandler;
    ADS1015 &_qwiicAdc;
    SCMD &_qwiicMotorDriver;

    float lastPercentage;
    int lastKnownHeadlights;

    ControllerMode currentControllerMode;

    String currentLocoRoadName;

    float getDetentPercentageForHeadlights(int headlights);

public:
    HeadlightsController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentLocoState);
};

#endif