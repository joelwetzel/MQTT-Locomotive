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
}