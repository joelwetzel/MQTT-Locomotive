#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "config.h"

#include "audioDriver.h"

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H


class MqttHandler
{
    PubSubClient &_mqttClient;

    AudioDriver &_audioDriver;

    const char* ssid = WIFI_SSID; 
    const char* password = WIFI_PASSWORD;
    const char* mqtt_server = USER_MQTT_SERVER;
    const int mqtt_port = USER_MQTT_PORT;
    const char *mqtt_device_network_id = ""USER_DEVICE_NETWORK_ID"AUDIO"; 

    bool _boot;
    int _publishCounter;

    void setup_wifi();
    void reconnect();
    void publish(const char *topic, float value);
    void publish(const char *topic, int value);

public:
    MqttHandler(PubSubClient &mqttClient, AudioDriver &audioDriver);

    void Setup();
    void Loop();
    void ProcessStep();
};


#endif