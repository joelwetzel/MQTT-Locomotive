#include "mqttHandler.h"

#include <Regexp.h>


MqttHandler::MqttHandler(PubSubClient &mqttClient, LocoList &locoList, LocoDisplayController &locoDisplayController)
    : _mqttClient(mqttClient), _locoList(locoList), _locoDisplayController(locoDisplayController)
{
    _publishCounter = 0;
}


void MqttHandler::subscribeToLoco(String roadName)
{
  _mqttClient.subscribe(("locomotives/" + roadName + "/attributes/masterswitch").c_str());
}


void MqttHandler::Setup()
{
  _locoDisplayController.HandleDisconnected();

  WiFi.mode(WIFI_STA);
  setup_wifi();

  _mqttClient.setServer(mqtt_server, mqtt_port);

  _mqttClient.setCallback([this](char *cstrTopic, byte *payload, unsigned int length)
  {
    String strTopic = cstrTopic;
    payload[length] = '\0';
    String strPayload = String((char *)payload);

    float floatPayload = strPayload.toFloat();
    int intPayload = strPayload.toInt();

    //Serial.printf(("Received message: " + strTopic + "\n").c_str());
    //Serial.printf(("Payload: " + strPayload + "\n").c_str());

    if (strTopic == "locomotives/discovery")
    {
      //Serial.println("Discovered locomotive: " + strPayload);
      _locoList.AddLoco(strPayload);
      subscribeToLoco(strPayload);
    }
    else if (strTopic.endsWith("/masterswitch"))
    {
      MatchState ms;
      ms.Target((char*)strTopic.c_str());

      char result = ms.Match("locomotives\/([a-zA-Z0-9]+)\/attributes\/masterswitch\0", 0);
      //char result = ms.Match("attributes\/masterswitch", 0);

      if (result == REGEXP_MATCHED && ms.level == 1)    // Matched and one captured match.
      {
        char buf [100];
        char *roadName = ms.GetCapture(buf, 0);
        int masterSwitch = intPayload;

        Serial.printf("Roadname: ");
        Serial.printf(roadName);
        Serial.printf("\n");
      }
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

  ArduinoOTA.setHostname("esp8266-"USER_DEVICE_NETWORK_ID"-OTA");
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

        _locoDisplayController.HandleConnectedToMqtt();

        digitalWrite(MQTT_CONNECTED_PIN, 0);

        if (boot)
        {
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/panelStatus", "Rebooted");
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/panelIp", WiFi.localIP().toString().c_str());
            //ProcessStep();
            boot = false;
        }
        else
        {
          _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/panelStatus", "Reconnected"); 
        }

        // ... and resubscribe
         _mqttClient.subscribe("locomotives/discovery");
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


void MqttHandler::Loop()
{
    if (!_mqttClient.connected()) 
    {
      _locoDisplayController.HandleDisconnected();
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

    // float battery = _batteryDriver.GetVoltage();
    // if ((fabs(battery - _lastBattery) > 0.05 && _publishCounter % 50 == 0) || _publishCounter % 396 == 0)
    // {
    //     publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/battery", _batteryDriver.GetVoltage());
    //     _lastBattery = battery;
    // }


    if (_publishCounter == 1500)
    {
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/panelStatus", "OK"); 
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


void MqttHandler::publish(const char *topic, const char *value)
{
    _mqttClient.publish(topic, value);
}


