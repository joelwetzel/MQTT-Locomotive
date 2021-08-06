#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_TCA9534.h>                   // QWIIC GPIO
#include <SparkFun_ADS1015_Arduino_Library.h>   // QWIIC ADC
#include <SCMD.h>                               // QWIIC motor driver
#include <SCMD_config.h>

#include "config.h"

#include "mqttHandler.h"
#include "locoList.h"
#include "locoStateCache.h"
#include "locoDisplayController.h"
#include "nextLocoButtonController.h"
#include "masterSwitchController.h"
#include "engineOnController.h"
#include "reverserController.h"
#include "headlightsController.h"
#include "throttleController.h"
#include "cablightsController.h"
#include "bellController.h"
#include "hornController.h"

/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

Adafruit_SSD1306 locoDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TCA9534 qwiicGpio1;
TCA9534 qwiicGpio2;
ADS1015 qwiicAdc;
SCMD qwiicMotorDriver1;
SCMD qwiicMotorDriver2;

LocoList locoList;
LocoStateCache locoStateCache;
LocoDisplayController locoDisplayController(locoList, locoDisplay);
MqttHandler mqttHandler(mqttClient, locoList, locoStateCache, locoDisplayController);

NextLocoButtonController nextLocoButtonController(locoDisplayController);
MasterSwitchController masterSwitchController(mqttHandler, qwiicGpio2);
EngineOnController engineOnController(mqttHandler, qwiicGpio1);
ReverserController reverserController(mqttHandler, qwiicAdc, qwiicMotorDriver1);
HeadlightsController headlightsController(mqttHandler, qwiicAdc, qwiicMotorDriver1);
CablightsController cablightsController(mqttHandler, qwiicGpio1);
ThrottleController throttleController(mqttHandler, qwiicAdc, qwiicMotorDriver2);
BellController bellController(mqttHandler, qwiicGpio1);
HornController hornController(mqttHandler, qwiicGpio2);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  nextLocoButtonController.ProcessStep();
  locoDisplayController.ProcessStep();

  LocoState currentState = locoStateCache.GetStateFor(locoDisplayController.GetSelectedRoadname());

  masterSwitchController.ProcessStep(currentState);
  engineOnController.ProcessStep(currentState);
  reverserController.ProcessStep(currentState);
  headlightsController.ProcessStep(currentState);
  cablightsController.ProcessStep(currentState);
  throttleController.ProcessStep(currentState);
  bellController.ProcessStep(currentState);
  hornController.ProcessStep(currentState);

  mqttHandler.ProcessStep();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  delay(1000);

  // Initialize Qwiic GPIO
  Serial.println("Looking for Qwiic GPIO 1...");
  if (qwiicGpio1.begin(Wire, 0x27) == false) {
    Serial.println("Check connections.  No Qwiic GPIO 1 detected.");
    while (1);
  }
  Serial.println("Found Qwiic GPIO 1.");

  Serial.println("Looking for Qwiic GPIO 2...");
  if (qwiicGpio2.begin(Wire, 0x26) == false) {
    Serial.println("Check connections.  No Qwiic GPIO 2 detected.");
    while (1);
  }
  Serial.println("Found Qwiic GPIO 2.");

  // Initialize Qwiic ADC
  Serial.println("Looking for Qwiic ADC...");
  if (qwiicAdc.begin() == false) {
    Serial.println("Check connections.  No Qwiic ADC detected.");
    while (1);
  }
  Serial.println("Found Qwiic ADC.");

  // Initialize Qwiic Motor Driver 1
  qwiicMotorDriver1.settings.commInterface = I2C_MODE;
  qwiicMotorDriver1.settings.I2CAddress = 0x5D;
  Serial.println("Looking for Qwiic Motor Driver 1...");
  while (qwiicMotorDriver1.begin() != 0xA9)
  {
    Serial.println("Motor Driver (1) ID mismatch, trying again.");
    delay(500);
  }
  while (qwiicMotorDriver1.ready() == false)
  {
    ;
  }
  while (qwiicMotorDriver1.busy())
  {
    ;
  }
  qwiicMotorDriver1.enable();
  Serial.println("Found Qwiic Motor Driver 1.");

  // Initialize Qwiic Motor Driver 2
  qwiicMotorDriver2.settings.commInterface = I2C_MODE;
  qwiicMotorDriver2.settings.I2CAddress = 0x5E;
  Serial.println("Looking for Qwiic Motor Driver 2...");
  while (qwiicMotorDriver2.begin() != 0xA9)
  {
    Serial.println("Motor Driver (2) ID mismatch, trying again.");
    delay(500);
  }
  while (qwiicMotorDriver2.ready() == false)
  {
    ;
  }
  while (qwiicMotorDriver2.busy())
  {
    ;
  }
  qwiicMotorDriver2.enable();
  Serial.println("Found Qwiic Motor Driver 2.");

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  masterSwitchController.Setup();
  engineOnController.Setup();
  reverserController.Setup();
  headlightsController.Setup();
  cablightsController.Setup();
  throttleController.Setup();
  bellController.Setup();
  hornController.Setup();

  locoDisplayController.Setup(); // Do this before mqttHandler.Setup(), so that it displays the loading screen while connecting to wifi.

  mqttHandler.Setup();

  nextLocoButtonController.Setup();

  timer.setInterval(15, processStep);
}


void loop() {
  mqttHandler.Loop();
  timer.run();
}