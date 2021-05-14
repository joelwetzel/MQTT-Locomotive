#include "mqttHandler.h"


MqttHandler::MqttHandler(PubSubClient &mqttClient, TachDriver &tachDriver)
    : _mqttClient(mqttClient), _tachDriver(tachDriver)
{
    _lastWheelRpms = 0.0;

    _publishCounter = 0;
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

        if (strTopic == "locomotives/"USER_DEVICE_NETWORK_ID"/commands/reset")
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
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/tachCalibratorStatus", "Rebooted");
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/tachCalibratorIp", WiFi.localIP().toString().c_str());
            republishCommands();
            ProcessStep();
            boot = false;
        }
        else
        {
          _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/tachCalibratorStatus", "Reconnected"); 
        }

        // ... and resubscribe
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

    float wheelRpms = _tachDriver.GetWheelRpm();
    if (_publishCounter%12 == 0)
    {
#ifdef PUBLISH_TACH
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/measuredwheelrpms", wheelRpms);
#endif
        _lastWheelRpms = wheelRpms;
    }

    // Discovery protocol
    if (_publishCounter%500 == 0)
    {
        _mqttClient.publish("locomotives/discovery", USER_DEVICE_NETWORK_ID);
    }

    if (_publishCounter == 1500)
    {
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/tachCalibratorStatus", "OK"); 
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