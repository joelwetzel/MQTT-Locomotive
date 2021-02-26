#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//#include <ArduinoOTA.h>     // https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

#include "iControlModel.h"
#include "lightingDriver.h"
#include "soundController.h"
#include "batteryDriver.h"
#include "smokeDriver.h"
#include "config.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H


class MqttHandler
{
    PubSubClient &_mqttClient;

    IControlModel &_controlModel;
    LightingDriver &_lightingDriver;
    SoundController &_soundController;
    BatteryDriver &_batteryDriver;
    SmokeDriver &_smokeDriver;

    const char* ssid = WIFI_SSID; 
    const char* password = WIFI_PASSWORD;
    const char* mqtt_server = USER_MQTT_SERVER;
    const int mqtt_port = USER_MQTT_PORT;
    const char *mqtt_device_network_id = USER_DEVICE_NETWORK_ID; 

    bool boot = true;

    bool _lastEngineOn;
    float _lastEngineRpms;
    int _lastReverser;
    float _lastSmokePercent;
    float _lastSpeed;
    bool _lastBell;
    bool _lastHorn;
    
    int _publishCounter;

    void setup_wifi();
    void reconnect();
    void republishCommands();
    void publish(const char *topic, float value);
    void publish(const char *topic, int value);

public:
    MqttHandler(PubSubClient &mqttClient, IControlModel &controlModel, LightingDriver &lightingDriver, SoundController &soundController, BatteryDriver &batteryDriver, SmokeDriver &smokeDriver);

    void Setup();
    void Loop();
    void ProcessStep();
};


#endif