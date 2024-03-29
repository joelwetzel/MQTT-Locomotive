#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>


#include "tachDriver.h"

#include "config.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H




class MqttHandler
{
    PubSubClient &_mqttClient;

    TachDriver &_tachDriver;


    const char* ssid = WIFI_SSID; 
    const char* password = WIFI_PASSWORD;
    const char* mqtt_server = USER_MQTT_SERVER;
    const int mqtt_port = USER_MQTT_PORT;
    const char *mqtt_device_network_id = USER_DEVICE_NETWORK_ID; 

    bool boot = true;

    float _lastWheelRpms;
    
    int _publishCounter;

    void setup_wifi();
    void reconnect();
    void republishCommands();
    void publish(const char *topic, float value);
    void publish(const char *topic, int value);
    void publish(const char *topic, const char *value);

public:
    MqttHandler(PubSubClient &mqttClient, TachDriver &tachDriver);

    void Setup();
    void Loop();
    void ProcessStep();
};


#endif