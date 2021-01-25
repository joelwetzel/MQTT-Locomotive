#include <PubSubClient.h>
#include <ESP8266WiFi.h>    // if you get an error here you need to install the ESP8266 board manager 
#include <ESP8266mDNS.h>    // if you get an error here you need to install the ESP8266 board manager 
#include <ArduinoOTA.h>     // https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

#include "physics.h"
#include "config.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H


class MqttHandler
{
    PubSubClient &_mqttClient;

    Physics &_physics;

    const char* ssid = WIFI_SSID; 
    const char* password = WIFI_PASSWORD;
    const char* mqtt_server = USER_MQTT_SERVER;
    const int mqtt_port = USER_MQTT_PORT;
    const char *mqtt_device_network_id = USER_DEVICE_NETWORK_ID; 

    bool boot = true;
    float _lastEngine;
    float _lastSpeed;
    int _publishCounter;

    void setup_wifi();
    void reconnect();
    void republishCommands();
    void publish(const char *topic, float value);

public:
    MqttHandler(PubSubClient &mqttClient, Physics &physics);

    void Setup();
    void Loop();
    void ProcessStep();
};


#endif