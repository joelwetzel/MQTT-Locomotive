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
}