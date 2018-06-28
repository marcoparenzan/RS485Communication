using RJCP.IO.Ports;
using System;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using static System.Console;

namespace RS485Communication
{
    class Program
    {
        static async Task Main(string[] args)
        {
            WriteLine("RS485Master");
            WriteLine($"Opening on {args[0]} port");

            var port = new SerialPortStream(args[0], 57600, 8, Parity.None, StopBits.One);
            port.DataReceived += (s, e) => {
                var buffer = new byte[port.BytesToRead];
                var read = port.Read(buffer, 0, buffer.Length);
                var id = buffer[0];
                var cmd = buffer[1];
                var cmdlen = buffer[2];
                WriteLine($"{read} bytes from {id}: cmd={cmd} len={cmdlen}");

                switch (cmd)
                {
                    case 0x80 + 0x02:
                        var readTemperatureValue = System.BitConverter.ToSingle(buffer, 3);
                        WriteLine($"READ TEMPERATURE={readTemperatureValue}");
                        break;
                }
            };
            port.Open();

            var requestT0 = new byte[] { 0, 2, 2 };
            var requestT1 = new byte[] { 1, 2, 2 };

            try
            {
                while (true)
                {
                    WriteLine("Sending T0");
                    port.Write(requestT0, 0, requestT0.Length);
                    await Task.Delay(2000);

                    WriteLine("Sending T1");
                    port.Write(requestT1, 0, requestT1.Length);
                    await Task.Delay(2000);
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                port.Close();
            }
        }
    }
}
