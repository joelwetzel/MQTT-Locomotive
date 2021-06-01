#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "config.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H



class MqttHandler
{
    PubSubClient &_mqttClient;

    //LightingDriver &_lightingDriver;


    const char* ssid = WIFI_SSID; 
    const char* password = WIFI_PASSWORD;
    const char* mqtt_server = USER_MQTT_SERVER;
    const int mqtt_port = USER_MQTT_PORT;
    const char *mqtt_device_network_id = USER_DEVICE_NETWORK_ID; 

    bool boot = true;

    // bool _lastEngineOn;
    // float _lastEngineRpms;
    // int _lastReverser;

    int _publishCounter;

    // String _masterMasterSwitchTopic;
    // String _masterEngineOnTopic;

    void setup_wifi();
    void reconnect();
    void publish(const char *topic, float value);
    void publish(const char *topic, int value);
    void publish(const char *topic, const char *value);

public:
//    MqttHandler(PubSubClient &mqttClient, IControlModel* ptrControlModel, LightingDriver &lightingDriver, SoundController &soundController, BatteryDriver &batteryDriver, SmokeDriver &smokeDriver, TachDriver &tachDriver, PidController &pidController);
    MqttHandler(PubSubClient &mqttClient);

    void Setup();
    void Loop();
    void ProcessStep();
};








#endif