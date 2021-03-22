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

    [Verb("listen", isDefault: false, HelpText="Listen for updates from a locomotive.")]
    public class ListenOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: 1292", Required = true)]
        public string RoadNumber { get; set; }
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

            var mqttOptions = new MqttClientOptionsBuilder()
                                .WithClientId($"LocoCLI{Guid.NewGuid()}")
                                .WithTcpServer("mqtt.local")
                                .WithCleanSession()
                                .Build();

            return await CommandLine.Parser.Default.ParseArguments<ScanOptions, ListenOptions, SendOptions>(args)
                    .MapResult<ScanOptions, ListenOptions, SendOptions, Task<int>>(
                        (ScanOptions opts) => RunScanAndReturnExitCode(opts, mqttOptions),
                        (ListenOptions opts) => RunListenAndReturnExitCode(opts),
                        (SendOptions opts) => RunSendAndReturnExitCode(opts),
                        erros => Task.FromResult(1)
                    );
        }

        static async Task<int> RunScanAndReturnExitCode(ScanOptions opts, IMqttClientOptions mqttOptions)
        {
            Console.WriteLine("Scanning...");

            var mqttFactory = new MqttFactory();
            var mqttClient = mqttFactory.CreateMqttClient();

            mqttClient.UseConnectedHandler(async e =>
            {
                Console.WriteLine($"Connected to MQTT.");

                await mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic("locomotives/discovery").Build());
            });

            mqttClient.UseDisconnectedHandler(e =>
            {
                Console.WriteLine($"Disconnected from MQTT: {e.Reason}");
            });

            mqttClient.UseApplicationMessageReceivedHandler(e =>
            {
                Console.WriteLine("Found locomotive: " + Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
            });

            await mqttClient.ConnectAsync(mqttOptions, CancellationToken.None);

            while (true)
            {
                await Task.Delay(10);
            }

            return 0;
        }

        static async Task<int> RunListenAndReturnExitCode(ListenOptions opts)
        {
            Console.WriteLine($"Listening for {opts.RoadNumber}...");

            await Task.Delay(2000);

            Console.WriteLine("Done.");

            return 0;
        }

        static async Task<int> RunSendAndReturnExitCode(SendOptions opts)
        {
            Console.WriteLine($"Sending: {opts.RoadNumber} : {opts.Command} : {opts.Value}");

            await Task.Delay(2000);

            Console.WriteLine("Done.");

            return 0;
        }
    }
}
