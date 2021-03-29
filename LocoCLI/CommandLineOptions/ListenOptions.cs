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
    [Verb("listen", isDefault: false, HelpText = "Listen for updates from a locomotive.")]
    public class ListenOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: 1292", Required = true)]
        public string RoadNumber { get; set; }

        [Option('a', "attribute", HelpText = "Attribute to listen for.", Required = true)]
        public string Attribute { get; set; }
    }
}