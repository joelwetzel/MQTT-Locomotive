#ifndef CONFIG_H
#define CONFIG_H

#define PUBLISH_TACH


#define PHYSICS_DELTAT            15.0

// Include file that contains WiFi credentials.  The #defines that need to be in it are:  WIFI_SSID, WIFI_PASSWORD
#include "wifi.h"

#define USER_MQTT_SERVER          "mqtt.local"
#define USER_MQTT_PORT            1883


#define MQTT_CONNECTED_PIN        2


#endif