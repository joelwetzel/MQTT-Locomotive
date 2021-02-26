#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"
#include "physics.h"
#include "motorDriver.h"
#include "mqttHandler.h"
#include "lightingDriver.h"
#include "soundController.h"
#include "batteryDriver.h"
#include "smokeDriver.h"


/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

BatteryDriver batteryDriver;
Physics physics(batteryDriver);
MotorDriver motorDriver;
SmokeDriver smokeDriver;
LightingDriver lightingDriver(physics, batteryDriver);
SoundController soundController;
MqttHandler mqttHandler(mqttClient, physics, lightingDriver, soundController, batteryDriver, smokeDriver);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  batteryDriver.ProcessStep();
  physics.ProcessStep();
  motorDriver.SetMotorSpeed(physics.GetSpeed());
  smokeDriver.SetSmokePercent(physics.GetSmokePercent());
  lightingDriver.ProcessStep();
  soundController.ProcessStep();
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

  motorDriver.Setup();
  smokeDriver.Setup();
  mqttHandler.Setup();
  lightingDriver.Setup();
  soundController.Setup();
  batteryDriver.Setup();

  delay(10);
  
  timer.setInterval(PHYSICS_DELTAT, processStep);   

  // Set the PWM (which goes out to the motor controller) to a high frequency.
  // If not done, you'll get a 1KHz tone from the motor.
  analogWriteFreq(16384);
}


void loop()
{
  mqttHandler.Loop();
  timer.run();
}
