#ifndef CONFIG_H
#define CONFIG_H

#define DELTAT            30.0

#include "wifi.h"
#define USER_MQTT_SERVER          "mqtt.local"
#define USER_MQTT_PORT            1883

#define USER_DEVICE_NETWORK_ID    "UP1292"

#ifdef ARDUINO_ESP8266_NODEMCU
    #define MQTT_CONNECTED_PIN        2
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define MQTT_CONNECTED_PIN        2
#endif


// RS-3 specific figures
#define ENGINE_RPM_IDLE         315
#define ENGINE_RPM_1            500
#define ENGINE_RPM_2            700
#define ENGINE_RPM_MAX          1000


#endif