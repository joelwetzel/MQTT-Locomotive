#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"
#include "mqttHandler.h"
#include "audioDriver.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
AudioDriver audioDriver;
MqttHandler mqttHandler(mqttClient, audioDriver);
SimpleTimer timer;


void timerStep()
{
  mqttHandler.ProcessStep();
}


void setup() {
  //Serial.begin(9600);

  delay(1000);
  LittleFS.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  mqttHandler.Setup();
  audioDriver.Setup();
  timer.setInterval(DELTAT, timerStep);
}


void loop() {
  timer.run();
  mqttHandler.Loop();
  audioDriver.Loop();
}