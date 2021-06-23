#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_TCA9534.h>

#include "config.h"

#include "mqttHandler.h"
#include "locoList.h"
#include "locoStateCache.h"
#include "locoDisplayController.h"
#include "nextLocoButtonController.h"
#include "masterSwitchController.h"
#include "engineOnController.h"

/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

Adafruit_SSD1306 locoDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TCA9534 qwiicGpio;

LocoList locoList;
LocoStateCache locoStateCache;
LocoDisplayController locoDisplayController(locoList, locoDisplay);
MqttHandler mqttHandler(mqttClient, locoList, locoStateCache, locoDisplayController);

NextLocoButtonController nextLocoButtonController(locoDisplayController);
MasterSwitchController masterSwitchController(mqttHandler, qwiicGpio);
EngineOnController engineOnController(mqttHandler, qwiicGpio);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  nextLocoButtonController.ProcessStep();
  locoDisplayController.ProcessStep();

  LocoState currentState = locoStateCache.GetStateFor(locoDisplayController.GetSelectedRoadname());

  masterSwitchController.ProcessStep(currentState);
  engineOnController.ProcessStep(currentState);

  mqttHandler.ProcessStep();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Looking for Qwiic GPIO...");
  if (qwiicGpio.begin() == false) {
    Serial.println("Check connections.  No Qwiic GPIO detected.");
    while (1);
  }
  Serial.println("Found Qwiic GPIO.");

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  masterSwitchController.Setup();
  engineOnController.Setup();
  locoDisplayController.Setup(); // Do this before mqttHandler.Setup(), so that it displays the loading screen while connecting to wifi.

  mqttHandler.Setup();

  nextLocoButtonController.Setup();

  timer.setInterval(30, processStep);
}


void loop() {
  mqttHandler.Loop();
  timer.run();
}