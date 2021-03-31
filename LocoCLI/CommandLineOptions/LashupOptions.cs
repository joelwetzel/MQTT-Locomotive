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
    [Verb("lashup", isDefault: false, HelpText = "Tell a locomotive to be an MU slave to another locomotive..")]
    public class LashupOptions
    {
        [Option('n', "number", HelpText = "Road number.  Ex: UP1292", Required = true)]
        public string RoadNumber { get; set; }

        [Option('m', "master", HelpText = "Road number of the master locomotive.", Required = true)]
        public string MasterRoadNumber { get; set; }
    }
}