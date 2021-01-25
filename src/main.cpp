#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"
#include "physics.h"
#include "motorDriver.h"
#include "mqttHandler.h"


/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

Physics physics(PHYSICS_DELTAT);
MotorDriver motorDriver;
MqttHandler mqttHandler(mqttClient, physics);

/*****************  END GLOBALS SECTION ***********************************/


void processStep()
{
  physics.ProcessStep();
  motorDriver.SetMotorSpeed(physics.GetSpeed());
  mqttHandler.ProcessStep();
}


void setup()
{
  Serial.begin(9600);

  pinMode(D0, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  mqttHandler.Setup();

  delay(10);
  
  timer.setInterval(PHYSICS_DELTAT, processStep);   

  analogWriteFreq(16384);
}


void loop()
{
  mqttHandler.Loop();

  timer.run();
}
