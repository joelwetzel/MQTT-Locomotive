#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"
#include "physics.h"
#include "motorDriver.h"
#include "mqttHandler.h"
#include "lightingDriver.h"
#include "soundDriver.h"
#include "batteryDriver.h"


/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

BatteryDriver batteryDriver;
Physics physics(batteryDriver);
MotorDriver motorDriver;
LightingDriver lightingDriver(physics, batteryDriver);
SoundDriver soundDriver;
MqttHandler mqttHandler(mqttClient, physics, lightingDriver, soundDriver, batteryDriver);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  batteryDriver.ProcessStep();
  physics.ProcessStep();
  motorDriver.SetMotorSpeed(physics.GetSpeed());
  mqttHandler.ProcessStep();
  lightingDriver.ProcessStep();
  soundDriver.ProcessStep();
}


void setup()
{
  Serial.begin(9600);

  pinMode(D0, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);  
  
  mqttHandler.Setup();
  lightingDriver.Setup();
  soundDriver.Setup();
  batteryDriver.Setup();

  delay(10);
  
  timer.setInterval(PHYSICS_DELTAT, processStep);   

  analogWriteFreq(16384);
}


void loop()
{
  mqttHandler.Loop();

  timer.run();
}
