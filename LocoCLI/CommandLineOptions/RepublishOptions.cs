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
    [Verb("republish", isDefault: false, HelpText = "Make a locomotive republish its commands and attributes.")]
    public class RepublishOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: UP1292", Required = true)]
        public string RoadNumber { get; set; }
    }
}