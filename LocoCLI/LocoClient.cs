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

    public struct LogArgs
    {
        public string Log { get; }

        public LogArgs(string log) : this()
        {
            this.Log = log;
        }
    }


    public class LocoClient
    {
        public event EventHandler<LogArgs> Log;
        public event EventHandler<ScanResultArgs> ScanResultFound;


        public async Task ConnectAndSendResetAsync(string roadNumber, CancellationToken token)
        {
            Log?.Invoke(this, new LogArgs("Connecting..."));

            var mqttFactory = new MqttFactory();
            var mqttClient = mqttFactory.CreateMqttClient();

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            mqttClient.UseConnectedHandler(async e =>
            {
                Log?.Invoke(this, new LogArgs("Connected to MQTT."));
                Log?.Invoke(this, new LogArgs("Sending reset..."));

                var message = new MqttApplicationMessageBuilder()
                                .WithTopic($"locomotives/{roadNumber}/commands/reset")
                                .WithPayload("1")
                                .Build();

                await mqttClient.PublishAsync(message, token);
                Log?.Invoke(this, new LogArgs("Reset sent."));
            });

            await mqttClient.ConnectAsync(mqttOptions, token);
        }


        public async Task ConnectAndScanAsync(CancellationToken token)
        {
            Log?.Invoke(this, new LogArgs("Connecting..."));

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
                Log?.Invoke(this, new LogArgs("Connected to MQTT."));
                Log?.Invoke(this, new LogArgs("Scanning..."));

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

            await mqttClient.ConnectAsync(mqttOptions, token);

            while (!token.IsCancellationRequested && !disconnected)
            {
                await Task.Delay(10);
            }

            Log?.Invoke(this, new LogArgs("Disconnecting..."));

            await mqttClient.DisconnectAsync();
        }
    }
}