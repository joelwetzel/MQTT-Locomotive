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

        private IMqttClient _mqttClient;

        private bool _isConnected = false;

        public async Task ConnectAsync(CancellationToken token)
        {
            Log?.Invoke(this, new LogArgs("Connecting..."));

            var mqttFactory = new MqttFactory();
            _mqttClient = mqttFactory.CreateMqttClient();

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            _mqttClient.UseConnectedHandler(async e =>
            {
                _isConnected = true;
                Log?.Invoke(this, new LogArgs("Connected to MQTT."));
            });

            _mqttClient.UseDisconnectedHandler(e =>
            {
                _isConnected = false;
                Log?.Invoke(this, new LogArgs("Disconnected from MQTT."));
            });

            await _mqttClient.ConnectAsync(mqttOptions, token);
        }


        public async Task SendCommandAsync(string roadNumber, string command, string value, CancellationToken token)
        {
            if (!_isConnected)
            {
                throw new InvalidOperationException("Not connected to MQTT.");
            }

            Log?.Invoke(this, new LogArgs($"Sending: {roadNumber}/commands/{command} : {value}"));

            var message = new MqttApplicationMessageBuilder()
                            .WithTopic($"locomotives/{roadNumber}/commands/{command}")
                            .WithPayload(value)
                            .Build();

            await _mqttClient.PublishAsync(message, token);
            Log?.Invoke(this, new LogArgs("Command sent."));
        }


        public async Task SendResetAsync(string roadNumber, CancellationToken token)
        {
            if (!_isConnected)
            {
                throw new InvalidOperationException("Not connected to MQTT.");
            }

            Log?.Invoke(this, new LogArgs("Sending reset..."));

            var message = new MqttApplicationMessageBuilder()
                            .WithTopic($"locomotives/{roadNumber}/commands/reset")
                            .WithPayload("1")
                            .Build();

            await _mqttClient.PublishAsync(message, token);
            Log?.Invoke(this, new LogArgs("Reset sent."));
        }


        public async Task ListenAsync(string roadNumber, string attribute, CancellationToken token)
        {
            if (!_isConnected)
            {
                throw new InvalidOperationException("Not connected to MQTT.");
            }

            Log?.Invoke(this, new LogArgs($"Listening for: {roadNumber}/attributes/{attribute}"));

            _mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                Log?.Invoke(this, new LogArgs(Encoding.UTF8.GetString(e.ApplicationMessage.Payload)));
            });

            await _mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic($"locomotives/{roadNumber}/attributes/{attribute}").Build());

            while (!token.IsCancellationRequested && _isConnected)
            {
                await Task.Delay(10);
            }

            Log?.Invoke(this, new LogArgs("Disconnecting..."));

            await _mqttClient.DisconnectAsync();
        }


        public async Task ScanAsync(CancellationToken token)
        {
            if (!_isConnected)
            {
                throw new InvalidOperationException("Not connected to MQTT.");
            }

            Log?.Invoke(this, new LogArgs("Scanning..."));

            _mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                var args = new ScanResultArgs(Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
                ScanResultFound?.Invoke(this, args);
            });

            await _mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic("locomotives/discovery").Build());

            while (!token.IsCancellationRequested && _isConnected)
            {
                await Task.Delay(10);
            }

            Log?.Invoke(this, new LogArgs("Disconnecting..."));

            await _mqttClient.DisconnectAsync();
        }
    }
}