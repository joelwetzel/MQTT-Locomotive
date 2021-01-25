#include "mqttHandler.h"

MqttHandler::MqttHandler(PubSubClient &mqttClient, Physics &physics)
    : _mqttClient(mqttClient), _physics(physics)
{
    _lastEngine = -1;
    _lastSpeed = -1;

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
            _physics.SetReverser(floatPayload);
            publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/reverser", floatPayload);
        }
    });

    ArduinoOTA.setHostname(USER_DEVICE_NETWORK_ID);
    ArduinoOTA.begin();
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

        if (boot)
        {
            republishCommands();
            _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/mqttStatus", "Rebooted");
            boot = false;
        }
        else
        {
          _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/mqttStatus", "Reconnected"); 
        }

        // ... and resubscribe
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/throttle");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/brake");
        _mqttClient.subscribe("locomotives/"USER_DEVICE_NETWORK_ID"/commands/reverser");
      } 
      else 
      {
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
}


void MqttHandler::Loop()
{
    if (!_mqttClient.connected()) 
    {
        reconnect();
    }

    _mqttClient.loop();
    ArduinoOTA.handle();
}


void MqttHandler::ProcessStep()
{
    // Publish attributes to MQTT, if:
    //   - They have changed
    //   - Or every 15 seconds

    float engine = _physics.GetEngine();
    float speed = _physics.GetSpeed();

    if (fabs(engine - _lastEngine) > 0.05 || _publishCounter % 1500 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/engine", engine);
        _lastEngine = engine;
    }

    if ((fabs(speed - _lastSpeed) > 0.01 && _publishCounter % 50 == 0) || _publishCounter % 1500 == 0)
    {
        publish("locomotives/"USER_DEVICE_NETWORK_ID"/attributes/speed", speed);
        _lastSpeed = speed;
    }

    if (_publishCounter % 1500 == 0)
    {
        republishCommands();
        _mqttClient.publish("locomotives/"USER_DEVICE_NETWORK_ID"/mqttStatus", "OK"); 
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