#ifndef CONFIG_H
#define CONFIG_H

//#define SIMULATOR_CONTROL_MODEL
#define TOY_CONTROL_MODEL

#define DIRECT_SPEED_CONTROL
//#define PID_SPEED_CONTROL

//#define PUBLISH_CONTROL_MODEL
//#define PUBLISH_PID_CONTROLLER
#define PUBLISH_TACH
//#define PUBLISH_SOUNDS

#define SPEEDPERCENT_TO_WHEEL_RPMS      5.5
#define WHEEL_RPM_TO_MPH                0.1
#define PHYSICS_DELTAT            30.0




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
#define ENGINE_RPM_MAX          1000



#endif