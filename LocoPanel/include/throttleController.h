#ifndef THROTTLECONTROLLER_H
#define THROTTLECONTROLLER_H

#include <WString.h>

#include "locoState.h"
#include "mqttHandler.h"
#include "controllerMode.h"

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SCMD.h>
#include <SCMD_config.h>

#define THROTTLE_ADC_PIN 2
#define THROTTLE_MOTOR_NUM 0

#define THROTTLE_MAX_VOLTAGE_READING 1641.0

#define THROTTLE_UP 0
#define THROTTLE_DOWN 1

#define POWER_LEVELS 8

class ThrottleController
{
    MqttHandler &_mqttHandler;
    ADS1015 &_qwiicAdc;
    SCMD &_qwiicMotorDriver;

    int lastKnownDetent;

    int resetCounter;
    ControllerMode currentThrottleMode;

    String currentLocoRoadName;

    float measureThrottle();

    int convertThrottleToPowerDetent(float throttle);
    float convertPowerDetentToThrottle(int powerDetent);

public:
    ThrottleController(MqttHandler &mqttHandler, ADS1015 &qwiicAdc, SCMD &qwiicMotorDriver);

    void Setup();
    void ProcessStep(LocoState currentLocoState);
};

#endif