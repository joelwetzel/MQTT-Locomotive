#include "mqttHandler.h"


MqttHandler::MqttHandler(PubSubClient &mqttClient, Physics &physics, LightingDriver &lightingDriver, SoundController &soundController, BatteryDriver &batteryDriver, SmokeDriver &smokeDriver)
    : _mqttClient(mqttClient), _physics(physics), _lightingDriver(lightingDriver), _soundController(soundController), _batteryDriver(batteryDriver), _smokeDriver(smokeDriver)
{
    _lastEngineOn = false;
    _lastEngineRpms = -1;
    _lastReverser = 0;
    _lastSmokePercent = -1;
    _lastSpeed = -1;
    _lastBell = false;
    _lastHorn = false;

    _publishCounter = 0;
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
            _physics.SetThrottle(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/throttle", floatPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake")
        {
            _physics.SetBrake(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/brake", floatPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser")
        {
            _physics.SetReverser(intPayload);
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
            _physics.SetEngineOn(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", _physics.GetEngineOn());
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke")
        {
            _smokeDriver.SetSmokeDisabled(intPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/disablesmoke", intPayload);
        }
    });

//    ArduinoOTA.setHostname(USER_DEVICE_NETWORK_ID);
//    ArduinoOTA.begin();
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
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle", _physics.GetThrottle());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake", _physics.GetBrake());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser", _physics.GetReverser());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/cablights", _lightingDriver.GetCabLights());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/headlights", _lightingDriver.GetHeadlights());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/bell", _soundController.GetBell());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/horn", _soundController.GetHorn());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/masterswitch", _batteryDriver.GetMasterSwitch());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/engineon", _physics.GetEngineOn());
    publish("locomotives/"USER_DEVICE_NETWORK_ID"/commands/disablesmoke", _smokeDriver.GetSmokeDisabled());
}


void MqttHandler::Loop()
{
    if (!_mqttClient.connected()) 
    {
        digitalWrite(MQTT_CONNECTED_PIN, 1);
        reconnect();
    }

    _mqttClient.loop();
//    ArduinoOTA.handle();
}


void MqttHandler::ProcessStep()
{
    // Publish attributes to MQTT, if:
    //   - They have changed
    //   - Or every 60 seconds

    bool masterSwitch = _batteryDriver.GetMasterSwitch();
    bool engineOn = _physics.GetEngineOn();
    float engineRpms = _physics.GetEngineRpms();
    int reverser = _physics.GetReverser();
    float smokePercent = _physics.GetSmokePercent();
    float speed = _physics.GetSpeed();
    bool bell = _soundController.GetBell();
    bool horn = _soundController.GetHorn();

    if ((fabs(engineRpms - _lastEngineRpms) > 0.05 && _publishCounter % 47 == 0) || _publishCounter % 500 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginerpms", engineRpms);
        _lastEngineRpms = engineRpms;
    }

    if ((fabs(smokePercent - _lastSmokePercent) > 0.05 && _publishCounter % 50 == 0) || _publishCounter % 502 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/smokepercent", smokePercent);
        _lastSmokePercent = smokePercent;
    }

    if ((fabs(speed - _lastSpeed) > 0.01 && _publishCounter % 52 == 0) || _publishCounter % 504 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speed", speed);
        _lastSpeed = speed;
    }

    if ((bell != _lastBell) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell", bell);
        _lastBell = bell;
    }

    if ((horn != _lastHorn) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn", horn);
        _lastHorn = horn;
    }

    if ((reverser != _lastReverser) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/reverser", reverser);
        _lastReverser = reverser;
    }

    if (boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/masterswitch", _batteryDriver.GetMasterSwitch());
    }

    if ((engineOn != _lastEngineOn || _publishCounter % 301 == 0) || boot)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engineon", engineOn);
        _lastEngineOn = engineOn;
    }

    if (_publishCounter % 396 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/battery", _batteryDriver.GetVoltage());
    }

    if (_publishCounter == 1500)
    {
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/engineStatus", "OK"); 
        _publishCounter = 0;
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