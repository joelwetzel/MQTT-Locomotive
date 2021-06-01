#ifndef CONFIG_H
#define CONFIG_H

// Include file that contains WiFi credentials.  The #defines that need to be in it are:  WIFI_SSID, WIFI_PASSWORD
#include "wifi.h"

#define USER_MQTT_SERVER          "mqtt.local"
#define USER_MQTT_PORT            1883

#ifdef ARDUINO_ESP8266_NODEMCU
    #define MQTT_CONNECTED_PIN        2
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define MQTT_CONNECTED_PIN        2
#endif

#endif