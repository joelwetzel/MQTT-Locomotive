#include "mqttHandler.h"

// test 2

MqttHandler::MqttHandler(PubSubClient &mqttClient, IControlModel* ptrControlModel, LightingDriver &lightingDriver, SoundController &soundController, BatteryDriver &batteryDriver, SmokeDriver &smokeDriver, TachDriver &tachDriver, PidController &pidController)
    : _mqttClient(mqttClient), _ptrControlModel(ptrControlModel), _lightingDriver(lightingDriver), _soundController(soundController), _batteryDriver(batteryDriver), _smokeDriver(smokeDriver), _tachDriver(tachDriver), _pidController(pidController)
{
    boot = true;
    
    _lastControlModelId = -1;

    _lastEngineOn = false;
    _lastEngineRpms = -1;
    _lastReverser = 0;
    _lastHeadlights = 0;
    _lastCablights = false;
    _lastSmokePercent = -1;
    _lastWheelRpms = 0.0;
    _lastSpeedMph = 0.0;
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

    _masterMasterSwitchTopic = String();
    _masterEngineOnTopic = String();
    _masterEngineRpmsTopic = String();
    _masterEnginePercentTopic = String();
    _masterWheelRpmTopic = String();
    _masterReverserTopic = String();
}


void MqttHandler::Setup()
{
    WiFi.mode(WIFI_STA);
    setup_wifi();

    _mqttClient.setServer(mqtt_server, mqtt_port);

    _mqttClient.setCallback([this](char* cstrTopic, byte* payload, unsigned int length) {
        String strTopic = cstrTopic;
        payload[length] = '\0';
        String strPayload = String((char *)payload);

        float floatPayload = strPayload.toFloat();
        int intPayload = strPayload.toInt();

        if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle")
        {
            _ptrControlModel->SetThrottle(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/throttle", floatPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake")
        {
            _ptrControlModel->SetBrake(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/brake", floatPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser" ||
                 strTopic == _masterReverserTopic)
        {
            _ptrControlModel->SetReverser(intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/cablights")
        {
            _lightingDriver.SetCabLights(intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/headlights")
        {
            _lightingDriver.SetHeadlights(intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/bell")
        {
            _soundController.SetBell(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell", intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/horn")
        {
            _soundController.SetHorn(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn", _soundController.GetHorn());
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/masterswitch" ||
                 strTopic == _masterMasterSwitchTopic)
        {
            _batteryDriver.SetMasterSwitch(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/masterswitch", intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/engineon" ||
                 strTopic == _masterEngineOnTopic)
        {
            _ptrControlModel->SetEngineOn(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", _ptrControlModel->GetEngineOn());
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke")
        {
            _smokeDriver.SetSmokeDisabled(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/disablesmoke", intPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/setcontrolmodel")
        {
            if (intPayload == 1 || intPayload == 2)
            {
                _desiredControlModelId = intPayload;
            }
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/lashup")
        {
            // Switch to slave control mode
            _desiredControlModelId = 3;

            // Subscribe to engine RPM, wheel RPM, and reverser from the master loco.
            _masterMasterSwitchTopic = String("locomotives/") + strPayload + String("/attributes/masterswitch");
            _masterEngineOnTopic = String("locomotives/") + strPayload + String("/attributes/engineon");
            _masterEngineRpmsTopic = String("locomotives/") + strPayload + String("/attributes/enginerpms");
            _masterEnginePercentTopic = String("locomotives/") + strPayload + String("/attributes/enginepercent");
            _masterWheelRpmTopic = String("locomotives/") + strPayload + String("/attributes/commandedwheelrpms");
            _masterReverserTopic = String("locomotives/") + strPayload + String("/attributes/reverser");

            _mqttClient.subscribe(_masterMasterSwitchTopic.c_str());
            _mqttClient.subscribe(_masterEngineOnTopic.c_str());
            _mqttClient.subscribe(_masterEngineRpmsTopic.c_str());
            _mqttClient.subscribe(_masterEnginePercentTopic.c_str());
            _mqttClient.subscribe(_masterWheelRpmTopic.c_str());
            _mqttClient.subscribe(_masterReverserTopic.c_str());
        }
        else if (strTopic == _masterEngineRpmsTopic)
        {
            _ptrControlModel->OverrideEngineRpms(floatPayload);
        }
        else if (strTopic == _masterEnginePercentTopic)
        {
            _ptrControlModel->OverrideEnginePercent(floatPayload);
        }
        else if (strTopic == _masterWheelRpmTopic)
        {
            _ptrControlModel->OverrideWheelRpms(floatPayload);
        }
        else if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reset")
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
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/lashup");
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
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesounds", 0);
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/setcontrolmodel", _ptrControlModel->GetControlModelId());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/lashup", "-");
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
    //   - Or every 30-60 seconds

#ifdef PUBLISH_CONTROL_MODEL
    if (boot || _publishCounter % 156 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/masterswitch", _batteryDriver.GetMasterSwitch());
    }

    if (boot || _publishCounter % 183 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/throttle", _ptrControlModel->GetThrottle());
    }

    bool engineOn = _ptrControlModel->GetEngineOn();
    if ((engineOn != _lastEngineOn || _publishCounter % 211 == 0) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", engineOn);
        _lastEngineOn = engineOn;
    }

    int reverser = _ptrControlModel->GetReverser();
    if ((reverser != _lastReverser) || boot || _publishCounter % 550 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/reverser", reverser);
        _lastReverser = reverser;
    }

    int headlights = _lightingDriver.GetHeadlights();
    if ((headlights != _lastHeadlights) || boot || _publishCounter % 417 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/headlights", headlights);
        _lastHeadlights = headlights;
    }

    bool cablights = _lightingDriver.GetCabLights();
    if ((cablights != _lastCablights || _publishCounter % 218 == 0) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/cablights", cablights);
        _lastCablights = cablights;
    }

    float engineRpms = _ptrControlModel->GetEngineRpms();
    if ((fabs(engineRpms - _lastEngineRpms) > 0.05 && _publishCounter % 47 == 0) || _publishCounter % 143 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginerpms", engineRpms);
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginepercent", _ptrControlModel->GetEnginePercent());
        _lastEngineRpms = engineRpms;
    }

    float speedMph = _ptrControlModel->GetSpeedMph();
    if ((fabs(speedMph - _lastSpeedMph) > 0.01 && _publishCounter % 52 == 0) || _publishCounter % 504 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speedpercent", _ptrControlModel->GetSpeedPercent());
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speedmph", speedMph);
        _lastSpeedMph = speedMph;
    }

    float smokePercent = _ptrControlModel->GetSmokePercent();
    if ((fabs(smokePercent - _lastSmokePercent) > 0.05 && _publishCounter % 50 == 0) || _publishCounter % 502 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/smokepercent", smokePercent);
        _lastSmokePercent = smokePercent;
    }
#endif

    float wheelRpms = _tachDriver.GetWheelRpm();
    if (_publishCounter%12 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/commandedwheelrpms", _ptrControlModel->GetEstimatedWheelRpms());
#ifdef PUBLISH_TACH
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/measuredwheelrpms", wheelRpms);
#endif
        _lastWheelRpms = wheelRpms;
    }

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


void MqttHandler::publish(const char *topic, const char *value)
{
    _mqttClient.publish(topic, value);
}


void MqttHandler::ChangeControlModel(IControlModel* newControlModel)
{
    _ptrControlModel = newControlModel;
}


int MqttHandler::GetDesiredControlModelId()
{
    return _desiredControlModelId;
}