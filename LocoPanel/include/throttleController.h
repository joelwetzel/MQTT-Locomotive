#ifndef THROTTLECONTROLLER_H
#define THROTTLECONTROLLER_H

#include <WString.h>

#include "locoState.h"
#include "mqttHandler.h"
#include "controllerMode.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define THROTTLE_ADC_PIN -1
#define THROTTLE_MOTOR_NUM 0

#define THROTTLE_MAX_VOLTAGE_READING 1660.0

#define THROTTLE_UP 1
#define THROTTLE_DOWN 0

class ThrottleController
{
    MqttHandler &_mqttHandler;
    SCMD &_qwiicMotorDriver;

    float lastPercentage;

    ControllerMode currentThrottleMode;

    String currentLocoRoadName;

    int counter;

public:
    ThrottleController(MqttHandler &mqttHandler, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentLocoState);
};

#endif