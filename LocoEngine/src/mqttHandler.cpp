#include "mqttHandler.h"


MqttHandler::MqttHandler(PubSubClient &mqttClient, IControlModel* ptrControlModel, LightingDriver &lightingDriver, SoundController &soundController, BatteryDriver &batteryDriver, SmokeDriver &smokeDriver, TachDriver &tachDriver, PidController &pidController)
    : _mqttClient(mqttClient), _ptrControlModel(ptrControlModel), _lightingDriver(lightingDriver), _soundController(soundController), _batteryDriver(batteryDriver), _smokeDriver(smokeDriver), _tachDriver(tachDriver), _pidController(pidController)
{
    _lastControlModelId = -1;

    _lastEngineOn = false;
    _lastEngineRpms = -1;
    _lastReverser = 0;
    _lastSmokePercent = -1;
    _lastWheelRpms = 0.0;
    _lastSpeedPercent = -1;
    _lastBell = false;
    _lastHorn = false;
    _lastBattery = 0.0;

    _lastPidControlValue = 0.0;
    _lastPidBTerm = 0.0;
    _lastPidPTerm = 0.0;
    _lastPidITerm = 0.0;
    _lastPidDTerm = 0.0;

    _publishCounter = 0;

    _desiredControlModelId = ptrControlModel->GetControlModelId();
}


void MqttHandler::Setup()
{
    WiFi.mode(WIFI_STA);
    setup_wifi();

    _mqttClient.setServer(mqtt_server, mqtt_port);

    _mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");

        char charPayload[50];
        String newTopic = topic;
        Serial.print(topic);
        Serial.print("] ");
        payload[length] = '\0';
        String newPayload = String((char *)payload);

        float floatPayload = newPayload.toFloat();
        int intPayload = newPayload.toInt();

        Serial.println(newPayload);
        Serial.println();
        newPayload.toCharArray(charPayload, newPayload.length() + 1);

        if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle")
        {
            _ptrControlModel->SetThrottle(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/throttle", floatPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake")
        {
            _ptrControlModel->SetBrake(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/brake", floatPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser")
        {
            _ptrControlModel->SetReverser(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/reverser", intPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/cablights")
        {
            _lightingDriver.SetCabLights(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/cablights", _lightingDriver.GetCabLights());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/headlights")
        {
            _lightingDriver.SetHeadlights(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/headlights", _lightingDriver.GetHeadlights());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/bell")
        {
            _soundController.SetBell(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell", _soundController.GetBell());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/horn")
        {
            _soundController.SetHorn(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn", _soundController.GetHorn());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/masterswitch")
        {
            _batteryDriver.SetMasterSwitch(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/masterswitch", intPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/engineon")
        {
            _ptrControlModel->SetEngineOn(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", _ptrControlModel->GetEngineOn());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke")
        {
            _smokeDriver.SetSmokeDisabled(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/disablesmoke", intPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/setcontrolmodel")
        {
            if (intPayload == 1 || intPayload == 2)
            {
                _desiredControlModelId = intPayload;
            }
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reset")
        {
            // This is a little hacky, that the MqttHandler knows too much about how the motor handler works, but I want to make 100% sure that we stop
            // the motors before we restart, so that the train doesn't run away.
            digitalWrite(D1, LOW);
            digitalWrite(D2, LOW);
            delay(1000);

            ESP.restart();
        }
    });
}


void MqttHandler::setup_wifi()
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("esp8266-"USER_DEVICE_NETWORK_ID"Engine-OTA");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
    Serial.println("Rebooting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}


void MqttHandler::reconnect()
{
  int retries = 0;
  int maxRetries = 150;

  while (!_mqttClient.connected())
  {
    if(retries < maxRetries)
    {
      Serial.print("Attempting MQTT connection...");

      if (_mqttClient.connect(mqtt_device_network_id))   // Password option:  client.connect(mqtt_device_network_id, mqtt_user, mqtt_pass)
      {
        Serial.println("connected");

        digitalWrite(MQTT_CONNECTED_PIN, 0);

        if (boot)
        {
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/engineStatus", "Rebooted");
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/engineIp", WiFi.localIP().toString().c_str());
            republishCommands();
            ProcessStep();
            boot = false;
        }
        else
        {
          _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/engineStatus", "Reconnected"); 
        }

        // ... and resubscribe
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/cablights");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/headlights");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/bell");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/horn");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/masterswitch");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/engineon");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/setcontrolmodel");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reset");
      } 
      else 
      {
        digitalWrite(MQTT_CONNECTED_PIN, 1);
        Serial.print("failed, rc=");
        Serial.print(_mqttClient.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }

    if(retries >= maxRetries)
    {
      ESP.restart();
    }
  }
}


void MqttHandler::republishCommands()
{
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/masterswitch", _batteryDriver.GetMasterSwitch());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/engineon", _ptrControlModel->GetEngineOn());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser", _ptrControlModel->GetReverser());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle", _ptrControlModel->GetThrottle());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake", _ptrControlModel->GetBrake());

    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/cablights", _lightingDriver.GetCabLights());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/headlights", _lightingDriver.GetHeadlights());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/bell", _soundController.GetBell());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/horn", _soundController.GetHorn());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke", _smokeDriver.GetSmokeDisabled());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/setcontrolmodel", _ptrControlModel->GetControlModelId());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reset", 0);
}


void MqttHandler::Loop()
{
    if (!_mqttClient.connected()) 
    {
        digitalWrite(MQTT_CONNECTED_PIN, 1);
        reconnect();
    }

    _mqttClient.loop();
    ArduinoOTA.handle();
}


void MqttHandler::ProcessStep()
{
    // Publish attributes to MQTT, if:
    //   - They have changed
    //   - Or every 60 seconds

#ifdef PUBLISH_CONTROL_MODEL
    if (boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/masterswitch", _batteryDriver.GetMasterSwitch());
    }

    bool engineOn = _ptrControlModel->GetEngineOn();
    if ((engineOn != _lastEngineOn || _publishCounter % 301 == 0) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", engineOn);
        _lastEngineOn = engineOn;
    }

    int reverser = _ptrControlModel->GetReverser();
    if ((reverser != _lastReverser) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/reverser", reverser);
        _lastReverser = reverser;
    }

    float engineRpms = _ptrControlModel->GetEngineRpms();
    if ((fabs(engineRpms - _lastEngineRpms) > 0.05 && _publishCounter % 47 == 0) || _publishCounter % 500 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginerpms", engineRpms);
        _lastEngineRpms = engineRpms;
    }

    float speedPercent = _ptrControlModel->GetSpeedPercent();
    if ((fabs(speedPercent - _lastSpeedPercent) > 0.01 && _publishCounter % 52 == 0) || _publishCounter % 504 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speedpercent", speedPercent);
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speedmph", _ptrControlModel->GetSpeedMph());
        _lastSpeedPercent = speedPercent;
    }

    float smokePercent = _ptrControlModel->GetSmokePercent();
    if ((fabs(smokePercent - _lastSmokePercent) > 0.05 && _publishCounter % 50 == 0) || _publishCounter % 502 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/smokepercent", smokePercent);
        _lastSmokePercent = smokePercent;
    }
#endif

#ifdef PUBLISH_TACH
    float wheelRpms = _tachDriver.GetWheelRpm();
    if (_publishCounter%12 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/commandedwheelrpms", _ptrControlModel->GetEstimatedWheelRpms());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/measuredwheelrpms", wheelRpms);
        _lastWheelRpms = wheelRpms;
    }
#endif

#ifdef PUBLISH_PID_CONTROLLER
    if (_publishCounter%23 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/pidControlValue", _pidController.GetControlValue());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/pidBTerm", _pidController.GetBTerm());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/pidPTerm", _pidController.GetPTerm());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/pidITerm", _pidController.GetITerm());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/pidDTerm", _pidController.GetDTerm());
    }
#endif

#ifdef PUBLISH_SOUNDS
    bool bell = _soundController.GetBell();
    if ((bell != _lastBell) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell", bell);
        _lastBell = bell;
    }

    bool horn = _soundController.GetHorn();
    if ((horn != _lastHorn) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn", horn);
        _lastHorn = horn;
    }
#endif

    float battery = _batteryDriver.GetVoltage();
    if ((fabs(battery - _lastBattery) > 0.05 && _publishCounter % 50 == 0) || _publishCounter % 396 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/battery", _batteryDriver.GetVoltage());
        _lastBattery = battery;
    }

    // Discovery protocol
    if (_publishCounter%500 == 0)
    {
        _mqttClient.publish("locomotives/discovery", USER_DEVICE_NETWORK_ID);
    }

    if (_publishCounter == 1500)
    {
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/engineStatus", "OK"); 
        _publishCounter = 0;
    }

    int controlModelId = _ptrControlModel->GetControlModelId();
    if (controlModelId != _lastControlModelId)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/controlmodel", controlModelId);
        _lastControlModelId = controlModelId;
    }

    _publishCounter++;
}


void MqttHandler::publish(const char *topic, float value)
{
    char charArray[50];

    String tempStr = String(value);
    tempStr.toCharArray(charArray, tempStr.length() + 1);
    _mqttClient.publish(topic, charArray);
}


void MqttHandler::publish(const char *topic, int value)
{
    char charArray[50];

    String tempStr = String(value);
    tempStr.toCharArray(charArray, tempStr.length() + 1);
    _mqttClient.publish(topic, charArray);
}


void MqttHandler::ChangeControlModel(IControlModel* newControlModel)
{
    _ptrControlModel = newControlModel;
}


int MqttHandler::GetDesiredControlModelId()
{
    return _desiredControlModelId;
}