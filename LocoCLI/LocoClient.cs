using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;

namespace LocoCLI
{
    public class LocoClient
    {
        public async Task ConnectAndScanAsync(CancellationToken token)
        {
            bool disconnected = false;

            var mqttFactory = new MqttFactory();
            var mqttClient = mqttFactory.CreateMqttClient();

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            mqttClient.UseConnectedHandler(async e =>
            {
                Console.WriteLine($"Connected to MQTT.  Scanning...");

                await mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic("locomotives/discovery").Build());
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                disconnected = true;
            });

            mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                Console.WriteLine("Found locomotive: " + Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
            });

            await mqttClient.ConnectAsync(mqttOptions, CancellationToken.None);

            while (!token.IsCancellationRequested && !disconnected)
            {
                await Task.Delay(10);
            }

            Console.WriteLine("Disconnecting...");

            await mqttClient.DisconnectAsync();
        }
    }
}