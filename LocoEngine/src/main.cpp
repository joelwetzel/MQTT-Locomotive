#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"

#include "iControlModel.h"
#include "simulatorControlModel.h"
#include "toyControlModel.h"

#include "motorDriver.h"
#include "mqttHandler.h"
#include "lightingDriver.h"
#include "soundController.h"
#include "batteryDriver.h"
#include "smokeDriver.h"
#include "tachDriver.h"


/*****************  START GLOBALS SECTION ***********************************/

WiFiClient espClient;
PubSubClient mqttClient(espClient);
SimpleTimer timer;

BatteryDriver batteryDriver;

SimulatorControlModel refSimulatorControlModel(batteryDriver);
IControlModel* ptrSimulatorControlModel(&refSimulatorControlModel);

ToyControlModel refToyControlModel(batteryDriver);
IControlModel* ptrToyControlModel(&refToyControlModel);

#ifdef SIMULATOR_CONTROL_MODEL
IControlModel* ptrControlModel = ptrSimulatorControlModel;
#elif defined TOY_CONTROL_MODEL
IControlModel* ptrControlModel = ptrToyControlModel;
#endif

MotorDriver motorDriver;
SmokeDriver smokeDriver;
TachDriver tachDriver;
LightingDriver lightingDriver(ptrControlModel, batteryDriver);
SoundController soundController;
MqttHandler mqttHandler(mqttClient, ptrControlModel, lightingDriver, soundController, batteryDriver, smokeDriver, tachDriver);

/*****************  END GLOBALS SECTION ***********************************/

void changeControlModel(IControlModel* newPtr)
{
  ptrControlModel = newPtr;
  lightingDriver.ChangeControlModel(newPtr);
  mqttHandler.ChangeControlModel(newPtr);
}


void processStep()
{
  batteryDriver.ProcessStep();
  ptrControlModel->ProcessStep();
  motorDriver.SetMotorSpeed(ptrControlModel->GetSpeedPercent());
  smokeDriver.SetSmokePercent(ptrControlModel->GetSmokePercent());
  tachDriver.ProcessStep();
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
  tachDriver.Setup();
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

  int desiredControlModelId = mqttHandler.GetDesiredControlModelId();
  if (desiredControlModelId != ptrControlModel->GetControlModelId() && ptrControlModel->GetSpeedPercent() < 0.1)
  {
    if (desiredControlModelId == 1)
    {
      changeControlModel(ptrSimulatorControlModel);
    }
    else if (desiredControlModelId == 2)
    {
      changeControlModel(ptrToyControlModel);
    }
  }

  timer.run();
}
