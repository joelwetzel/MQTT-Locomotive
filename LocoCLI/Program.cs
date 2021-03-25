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
            CancellationTokenSource tokenSource = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.Log += (sender, args) =>
            {
                Console.WriteLine(args.Log);
            };

            locoClient.ScanResultFound += (sender, args) =>
            {
                Console.WriteLine($"Locomotive found: {args.RoadNumber}");
            };

            locoClient.ConnectAndScanAsync(tokenSource.Token);

            // Wait until a key is pressed and then exit.
            await Task.Factory.StartNew(() =>
            {
                Console.ReadKey();
                tokenSource.Cancel();
            });

            await Task.Delay(100);

            return 0;
        }


        static async Task<int> RunResetAndReturnExitCode(ResetOptions opts)
        {
            CancellationTokenSource tokenSource = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.Log += (sender, args) =>
            {
                Console.WriteLine(args.Log);
            };

            await locoClient.ConnectAndSendResetAsync(opts.RoadNumber, tokenSource.Token);

            return 0;
        }


        static async Task<int> RunListenAndReturnExitCode(ListenOptions opts)
        {
            CancellationTokenSource tokenSource = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.Log += (sender, args) =>
            {
                Console.WriteLine(args.Log);
            };

            locoClient.ConnectAndListenAsync(opts.RoadNumber, opts.Attribute, tokenSource.Token);

            // Wait until a key is pressed and then exit.
            await Task.Factory.StartNew(() =>
            { 
                Console.ReadKey();
                tokenSource.Cancel();
            });

            await Task.Delay(100);

            return 0;
        }


        static async Task<int> RunSendAndReturnExitCode(SendOptions opts)
        {
            CancellationTokenSource tokenSource = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.Log += (sender, args) =>
            {
                Console.WriteLine(args.Log);
            };

            await locoClient.ConnectAndSendCommandAsync(opts.RoadNumber, opts.Command, opts.Value, tokenSource.Token);

            return 0;
        }
    }
}
