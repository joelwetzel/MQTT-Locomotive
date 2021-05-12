#ifndef CONFIG_H
#define CONFIG_H

//#define SIMULATOR_CONTROL_MODEL
#define TOY_CONTROL_MODEL
//#define MU_SLAVE_CONTROL_MODEL

//#define DIRECT_SPEED_CONTROL
#define PID_SPEED_CONTROL

#define PUBLISH_CONTROL_MODEL
#define PUBLISH_PID_CONTROLLER
#define PUBLISH_TACH
#define PUBLISH_SOUNDS


#define WHEEL_RPM_TO_SCALE_MPH                0.135
#define PHYSICS_DELTAT            15.0

// Include file that contains WiFi credentials.  The #defines that need to be in it are:  WIFI_SSID, WIFI_PASSWORD
#include "wifi.h"

#define USER_MQTT_SERVER          "mqtt.local"
#define USER_MQTT_PORT            1883


#ifdef ARDUINO_ESP8266_NODEMCU
    #define MQTT_CONNECTED_PIN        2
#elif ARDUINO_ESP8266_WEMOS_D1MINI
    #define MQTT_CONNECTED_PIN        2
#endif


#define ALCO_RS3                1

#if LOCO_TYPE == ALCO_RS3
    #include "locoTypes/alco_rs3.h"
#endif



#endif