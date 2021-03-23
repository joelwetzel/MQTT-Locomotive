using System.Threading;
using System;
using System.Threading.Tasks;
using CommandLine;
using MQTTnet;
using MQTTnet.Client.Options;
using MQTTnet.Client;
using System.Text;

namespace LocoCLI
{
    [Verb("scan", isDefault: false, HelpText = "Scan for active locomotives.")]
    public class ScanOptions
    {
    }


    [Verb("reset", isDefault: false, HelpText = "Reset a locomotive.")]
    public class ResetOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: 1292", Required = true)]
        public string RoadNumber { get; set; }
    }


    [Verb("listen", isDefault: false, HelpText="Listen for updates from a locomotive.")]
    public class ListenOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: 1292", Required = true)]
        public string RoadNumber { get; set; }

        [Option('a', "attribute", HelpText = "Attribute to listen for.", Required = true)]
        public string Attribute { get; set; }
    }


    [Verb("send", isDefault: false, HelpText = "Send a command to a locomotive.")]
    public class SendOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: 1292", Required = true)]
        public string RoadNumber { get; set; }

        [Option('c', "command", HelpText = "Command to send.", Required = true)]
        public string Command { get; set; }

        [Option('v', "value", HelpText = "Command value to send.", Required = true)]
        public string Value { get; set; }
    }



    public class Program
    {
        static async Task<int> Main(string[] args)
        {
            return await CommandLine.Parser.Default.ParseArguments<ScanOptions, ResetOptions, ListenOptions, SendOptions>(args)
                    .MapResult<ScanOptions, ResetOptions, ListenOptions, SendOptions, Task<int>>(
                        (ScanOptions opts) => RunScanAndReturnExitCode(opts),
                        (ResetOptions opts) => RunResetAndReturnExitCode(opts),
                        (ListenOptions opts) => RunListenAndReturnExitCode(opts),
                        (SendOptions opts) => RunSendAndReturnExitCode(opts),
                        erros => Task.FromResult(1)
                    );
        }


        static async Task<int> RunScanAndReturnExitCode(ScanOptions opts)
        {
            Console.WriteLine("Connecting...");

            CancellationTokenSource cts = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.ConnectAndScanAsync(cts.Token);

            // Wait until a key is pressed and then exit.
            await Task.Factory.StartNew(() =>
            {
                Console.ReadKey();
                cts.Cancel();
            });

            await Task.Delay(100);

            return 0;
        }


        static async Task<int> RunResetAndReturnExitCode(ResetOptions opts)
        {
            Console.WriteLine("Connecting...");

            var mqttFactory = new MqttFactory();
            var mqttClient = mqttFactory.CreateMqttClient();

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            bool finished = false;

            mqttClient.UseConnectedHandler(async e =>
            {
                Console.WriteLine($"Connected to MQTT.");
                Console.WriteLine("Sending reset...");

                var message = new MqttApplicationMessageBuilder()
                                    .WithTopic($"locomotives/{opts.RoadNumber}/commands/reset")
                                    .WithPayload("1")
                                    .Build();

                await mqttClient.PublishAsync(message, CancellationToken.None);
                finished = true;
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                Console.WriteLine($"Disconnected from MQTT: {e.Reason}");
            });

            await mqttClient.ConnectAsync(mqttOptions, CancellationToken.None);

            // Exit after the command is sent.
            while (!finished)
            {
                await Task.Delay(10);
            }

            return 0;
        }


        static async Task<int> RunListenAndReturnExitCode(ListenOptions opts)
        {
            Console.WriteLine("Connecting...");

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
                Console.WriteLine($"Listening for: {opts.RoadNumber} : {opts.Attribute}");

                await mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic($"locomotives/{opts.RoadNumber}/attributes/{opts.Attribute}").Build());
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                Console.WriteLine($"Disconnected from MQTT: {e.Reason}");
            });

            mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                Console.WriteLine(Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
            });

            await mqttClient.ConnectAsync(mqttOptions, CancellationToken.None);

            // Wait until a key is pressed and then exit.
            await Task.Factory.StartNew(() =>
            {
                Console.ReadKey();
            });

            return 0;
        }


        static async Task<int> RunSendAndReturnExitCode(SendOptions opts)
        {
            Console.WriteLine("Connecting...");

            var mqttFactory = new MqttFactory();
            var mqttClient = mqttFactory.CreateMqttClient();

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            bool finished = false;

            mqttClient.UseConnectedHandler(async e =>
            {
                Console.WriteLine($"Connected to MQTT.");
                Console.WriteLine($"Sending: {opts.RoadNumber} : {opts.Command} : {opts.Value}");

                var message = new MqttApplicationMessageBuilder()
                                    .WithTopic($"locomotives/{opts.RoadNumber}/commands/{opts.Command}")
                                    .WithPayload(opts.Value)
                                    .Build();

                await mqttClient.PublishAsync(message, CancellationToken.None);
                finished = true;
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                Console.WriteLine($"Disconnected from MQTT: {e.Reason}");
            });

            await mqttClient.ConnectAsync(mqttOptions, CancellationToken.None);

            // Exit after the command is sent.
            while (!finished)
            {
                await Task.Delay(10);
            }

            return 0;
        }
    }
}
