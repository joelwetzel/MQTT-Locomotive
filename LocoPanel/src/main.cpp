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

/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

Adafruit_SSD1306 locoDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TCA9534 qwiicGpio;
ADS1015 qwiicAdc;
SCMD qwiicMotorDriver;

LocoList locoList;
LocoStateCache locoStateCache;
LocoDisplayController locoDisplayController(locoList, locoDisplay);
MqttHandler mqttHandler(mqttClient, locoList, locoStateCache, locoDisplayController);

NextLocoButtonController nextLocoButtonController(locoDisplayController);
MasterSwitchController masterSwitchController(mqttHandler, qwiicGpio);
EngineOnController engineOnController(mqttHandler, qwiicGpio);
ReverserController reverserController(mqttHandler, qwiicAdc, qwiicMotorDriver);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  nextLocoButtonController.ProcessStep();
  locoDisplayController.ProcessStep();

  LocoState currentState = locoStateCache.GetStateFor(locoDisplayController.GetSelectedRoadname());

  masterSwitchController.ProcessStep(currentState);
  engineOnController.ProcessStep(currentState);
  reverserController.ProcessStep(currentState);

  mqttHandler.ProcessStep();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  delay(1000);

  // Initialize Qwiic GPIO
  Serial.println("Looking for Qwiic GPIO...");
  if (qwiicGpio.begin() == false) {
    Serial.println("Check connections.  No Qwiic GPIO detected.");
    while (1);
  }
  Serial.println("Found Qwiic GPIO.");

  // Initialize Qwiic ADC
  Serial.println("Looking for Qwiic ADC...");
  if (qwiicAdc.begin() == false) {
    Serial.println("Check connections.  No Qwiic ADC detected.");
    while (1);
  }
  Serial.println("Found Qwiic ADC.");

  // Initialize Qwiic Motor Driver
  qwiicMotorDriver.settings.commInterface = I2C_MODE;
  qwiicMotorDriver.settings.I2CAddress = 0x5D;
  Serial.println("Looking for Qwiic Motor Driver...");
  while (qwiicMotorDriver.begin() != 0xA9)
  {
    Serial.println("Motor Driver ID mismatch, trying again.");
    delay(500);
  }
  while (qwiicMotorDriver.ready() == false)
  {
    ;
  }
  while (qwiicMotorDriver.busy())
  {
    ;
  }
  qwiicMotorDriver.enable();
  Serial.println("Found Qwiic Motor Driver.");

  pinMode(MQTT_CONNECTED_PIN, OUTPUT);

  masterSwitchController.Setup();
  engineOnController.Setup();
  reverserController.Setup();

  locoDisplayController.Setup(); // Do this before mqttHandler.Setup(), so that it displays the loading screen while connecting to wifi.

  mqttHandler.Setup();

  nextLocoButtonController.Setup();

  timer.setInterval(30, processStep);
}


void loop() {
  mqttHandler.Loop();
  timer.run();
}