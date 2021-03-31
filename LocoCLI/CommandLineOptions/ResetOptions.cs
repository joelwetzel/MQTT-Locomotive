using System.Threading;
using System;
using System.Threading.Tasks;
using CommandLine;
using MQTTnet;
using MQTTnet.Client.Options;
using MQTTnet.Client;
using System.Text;

namespace LocoCLI.CommandLineOptions
{
    [Verb("reset", isDefault: false, HelpText = "Reset a locomotive.")]
    public class ResetOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: UP1292", Required = true)]
        public string RoadNumber { get; set; }
    }
}