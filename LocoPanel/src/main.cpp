#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

#include "mqttHandler.h"

/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;


//MqttHandler mqttHandler(mqttClient, ptrControlModel, lightingDriver, soundController, batteryDriver, smokeDriver, tachDriver, pidController);
MqttHandler mqttHandler(mqttClient);

/*****************  END GLOBALS SECTION ***********************************/


void setup() {
  Serial.begin(9600);

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  // Flash the LED for just a moment before trying to connect to MQTT.  The LED will come back on permanently when connected.
  pinMode(MQTT_CONNECTED_PIN, 0);
  delay(500);
  pinMode(MQTT_CONNECTED_PIN, 1);

  mqttHandler.Setup();
}

void loop() {
  mqttHandler.Loop();
}