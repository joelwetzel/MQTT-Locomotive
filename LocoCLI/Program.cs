using System.Threading;
using System;
using System.Threading.Tasks;
using CommandLine;
using MQTTnet;
using MQTTnet.Client.Options;
using MQTTnet.Client;
using System.Text;

using LocoCLI.CommandLineOptions;

namespace LocoCLI
{
    public class Program
    {
        static async Task<int> Main(string[] args)
        {
            return await CommandLine.Parser.Default.ParseArguments<ScanOptions, ResetOptions, ListenOptions, SendOptions, LashupOptions>(args)
                    .MapResult<ScanOptions, ResetOptions, ListenOptions, SendOptions, LashupOptions, Task<int>>(
                        (ScanOptions opts) => RunScanAndReturnExitCode(opts),
                        (ResetOptions opts) => RunResetAndReturnExitCode(opts),
                        (ListenOptions opts) => RunListenAndReturnExitCode(opts),
                        (SendOptions opts) => RunSendAndReturnExitCode(opts),
                        (LashupOptions opts) => RunLashupAndReturnExitCode(opts),
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

            await locoClient.ConnectAsync(tokenSource.Token);

            locoClient.ScanResultFound += (sender, args) =>
            {
                Console.WriteLine($"Locomotive found: {args.RoadNumber}");
            };

            locoClient.ScanAsync(tokenSource.Token);

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

            await locoClient.ConnectAsync(tokenSource.Token);

            await locoClient.SendResetAsync(opts.RoadNumber, tokenSource.Token);

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

            await locoClient.ConnectAsync(tokenSource.Token);

            locoClient.ListenAsync(opts.RoadNumber, opts.Attribute, tokenSource.Token);

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

            await locoClient.ConnectAsync(tokenSource.Token);

            await locoClient.SendCommandAsync(opts.RoadNumber, opts.Command, opts.Value, tokenSource.Token);

            return 0;
        }


        static async Task<int> RunLashupAndReturnExitCode(LashupOptions opts)
        {
            CancellationTokenSource tokenSource = new CancellationTokenSource();

            var locoClient = new LocoClient();

            locoClient.Log += (sender, args) =>
            {
                Console.WriteLine(args.Log);
            };

            await locoClient.ConnectAsync(tokenSource.Token);

            await locoClient.SendCommandAsync(opts.RoadNumber, "lashup", opts.MasterRoadNumber , tokenSource.Token);

            return 0;
        }
    }
}
