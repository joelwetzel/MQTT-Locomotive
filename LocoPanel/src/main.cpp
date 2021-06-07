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
#include "locoList.h"
#include "locoDisplayController.h"
#include "nextLocoButtonController.h"

/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

Adafruit_SSD1306 locoDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

LocoList locoList;
LocoDisplayController locoDisplayController(locoList, locoDisplay);
NextLocoButtonController nextLocoButtonController(locoDisplayController);
MqttHandler mqttHandler(mqttClient, locoList, locoDisplayController);


/*****************  END GLOBALS SECTION ***********************************/


void print(int index)
{
  Serial.printf("List starting at index %d\n", index);

  std::vector<String> orderedList = locoList.GetListStartingAtIndex(index);
  for (int i = 0; i < orderedList.size(); i++)
  {
    Serial.printf((orderedList[i] + "\n").c_str());
  }
}


void processStep()
{
  nextLocoButtonController.ProcessStep();
  locoDisplayController.ProcessStep();
}


void setup() {
  Serial.begin(9600);

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  locoDisplayController.Setup();    // Do this before mqttHandler.Setup(), so that it displays the loading screen while connecting to wifi.

  mqttHandler.Setup();
  nextLocoButtonController.Setup();

  timer.setInterval(30, processStep);
}


void loop() {
  mqttHandler.Loop();
  timer.run();
}