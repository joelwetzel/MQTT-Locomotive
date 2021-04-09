#include <Arduino.h>
#include <SimpleTimer.h>    // https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <math.h>

#include "config.h"

#include "iControlModel.h"
#include "simulatorControlModel.h"
#include "toyControlModel.h"
#include "muSlaveControlModel.h"

#include "pidController.h"
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

MuSlaveControlModel refMuControlModel(batteryDriver);
IControlModel *ptrMuControlModel(&refMuControlModel);

#ifdef SIMULATOR_CONTROL_MODEL
IControlModel* ptrControlModel = ptrSimulatorControlModel;
#elif defined TOY_CONTROL_MODEL
IControlModel* ptrControlModel = ptrToyControlModel;
#elif defined MU_SLAVE_CONTROL_MODEL
IControlModel* ptrControlModel = ptrMuControlModel;
#endif

PidController pidController;
MotorDriver motorDriver;
SmokeDriver smokeDriver;
TachDriver tachDriver;
LightingDriver lightingDriver(ptrControlModel, batteryDriver);
SoundController soundController;

MqttHandler mqttHandler(mqttClient, ptrControlModel, lightingDriver, soundController, batteryDriver, smokeDriver, tachDriver, pidController);

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

#ifdef DIRECT_SPEED_CONTROL
  motorDriver.SetMotorSpeed(ptrControlModel->GetSpeedPercent() * ptrControlModel->GetDirectionOfTravel());
#elif defined PID_SPEED_CONTROL
  float desiredWheelRpms = ptrControlModel->GetEstimatedWheelRpms();
  float measuredWheelRpms = tachDriver.GetWheelRpm();
  pidController.Update(desiredWheelRpms, measuredWheelRpms, micros());
  float controlledMotorPercent = pidController.GetControlValue();
  motorDriver.SetMotorSpeed(controlledMotorPercent * ptrControlModel->GetDirectionOfTravel());
#endif

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
}


void loop()
{
  mqttHandler.Loop();

  int desiredControlModelId = mqttHandler.GetDesiredControlModelId();
  if (desiredControlModelId != ptrControlModel->GetControlModelId() && ptrControlModel->GetSpeedMph() < 1)
  {
    if (desiredControlModelId == 1)
    {
      changeControlModel(ptrSimulatorControlModel);
    }
    else if (desiredControlModelId == 2)
    {
      changeControlModel(ptrToyControlModel);
    }
    else if (desiredControlModelId == 3)
    {
      changeControlModel(ptrMuControlModel);
    }
  }

  timer.run();
}
