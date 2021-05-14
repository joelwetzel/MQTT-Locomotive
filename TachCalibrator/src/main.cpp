#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"

#include "mqttHandler.h"
#include "tachDriver.h"


/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

TachDriver tachDriver;

MqttHandler mqttHandler(mqttClient, tachDriver);

/*****************  END GLOBALS SECTION ***********************************/

void processStep()
{
  tachDriver.ProcessStep();

  mqttHandler.ProcessStep();
}


void setup()
{
  Serial.begin(9600);

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  // Flash the LED for just a moment before trying to connect to MQTT.  The LED will come back on permanently when connected.
  pinMode(MQTT_CONNECTED_PIN, 0);
  delay(500);
  pinMode(MQTT_CONNECTED_PIN, 1);

  tachDriver.Setup();
  mqttHandler.Setup();
  
  delay(10);
  
  timer.setInterval(PHYSICS_DELTAT, processStep);   
}


void loop()
{
  mqttHandler.Loop();

  timer.run();
}
