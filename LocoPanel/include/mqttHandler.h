#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "config.h"

#include "locoList.h"
#include "locoStateCache.h"
#include "locoDisplayController.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H


class MqttHandler
{
    PubSubClient &_mqttClient;

    LocoList &_locoList;
    LocoStateCache &_locoStateCache;
    LocoDisplayController &_locoDisplayController;

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

    void subscribeToLoco(String roadName);

public:
    MqttHandler(PubSubClient &mqttClient, LocoList &locoList, LocoStateCache &locoStateCache, LocoDisplayController &locoDisplayController);

    void Setup();
    void Loop();
    void ProcessStep();

    void SendMasterSwitchFor(String roadname, bool value);
    void SendEngineOnFor(String roadname, bool value);
    void SendReverserFor(String roadname, int value);
    void SendHeadlightsFor(String roadname, int value);
    void SendCablightsFor(String roadname, bool value);
};

#endif