using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;

namespace LocoCLI
{
    public struct ScanResultArgs
    {
        public string RoadNumber { get; }

        public ScanResultArgs(string roadNumber) : this()
        {
            this.RoadNumber = roadNumber;
        }
    }


    public class LocoClient
    {
        public event EventHandler<ScanResultArgs> ScanResultFound;

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
                Console.WriteLine($"Connected to MQTT.");
                Console.WriteLine("Scanning...");

                await mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic("locomotives/discovery").Build());
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                disconnected = true;
            });

            mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                var args = new ScanResultArgs(Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
                ScanResultFound?.Invoke(this, args);
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