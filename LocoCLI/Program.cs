using System;
using CommandLine;

namespace LocoCLI
{
    [Verb("scan", isDefault: true, HelpText = "Scan for active locomotives.")]
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

    class Program
    {
        static int Main(string[] args)
        {
            return CommandLine.Parser.Default.ParseArguments<ScanOptions, ListenOptions, SendOptions>(args)
                    .MapResult(
                        (ScanOptions opts) => RunScanAndReturnExitCode(opts),
                        (ListenOptions opts) => RunListenAndReturnExitCode(opts),
                        (SendOptions opts) => RunSendAndReturnExitCode(opts),
                        erros => 1
                    );
        }

        static int RunScanAndReturnExitCode(ScanOptions opts)
        {
            Console.WriteLine("Scanning...");

            return 0;
        }

        static int RunListenAndReturnExitCode(ListenOptions opts)
        {
            Console.WriteLine($"Listening for {opts.RoadNumber}...");

            return 0;
        }

        static int RunSendAndReturnExitCode(SendOptions opts)
        {
            Console.WriteLine($"Sending: {opts.RoadNumber} : {opts.Command} : {opts.Value}");

            return 0;
        }
    }
}
