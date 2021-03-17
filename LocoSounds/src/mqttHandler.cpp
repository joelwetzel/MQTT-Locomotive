#include "mqttHandler.h"


MqttHandler::MqttHandler(PubSubClient &mqttClient, AudioDriver &audioDriver)
    : _mqttClient(mqttClient), _audioDriver(audioDriver)
{
    _boot = true;
    _publishCounter = 0;
}


void MqttHandler::Setup()
{
    WiFi.mode(WIFI_STA);
    setup_wifi();

    _mqttClient.setServer(mqtt_server, mqtt_port);

    _mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        char charPayload[50];
        String newTopic = topic;
        payload[length] = '\0';
        String newPayload = String((char *)payload);

        float floatPayload = newPayload.toFloat();
        int intPayload = newPayload.toInt();

        //Serial.println(newPayload);

        newPayload.toCharArray(charPayload, newPayload.length() + 1);

        if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell")
        {
            _audioDriver.SetBell(intPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn")
        {
            _audioDriver.SetHorn(intPayload);
        }
        else if (newTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginerpms")
        {
            _audioDriver.setEngineRpms(floatPayload);
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

  ArduinoOTA.setHostname("esp8266-"USER_DEVICE_NETWORK_ID"Audio-OTA");
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
      Serial.print("\nAttempting MQTT connection...");

      if (_mqttClient.connect(mqtt_device_network_id))   // Password option:  client.connect(mqtt_device_network_id, mqtt_user, mqtt_pass)
      {
        Serial.println("connected");

        digitalWrite(MQTT_CONNECTED_PIN, 0);
        
        if (_boot)
        {
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/audioStatus", "Rebooted");
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/audioIp", WiFi.localIP().toString().c_str());
            ProcessStep();
            _boot = false;
        }
        else
        {
          _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/audioStatus", "Reconnected"); 
        }

        // ... and resubscribe
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/bell");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/horn");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/enginerpms");
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
        digitalWrite(MQTT_CONNECTED_PIN, 1);
        reconnect();
    }

    _mqttClient.loop();
    ArduinoOTA.handle();
}


void MqttHandler::ProcessStep()
{
    if (_publishCounter == 1500)
    {
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/audioStatus", "OK"); 
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