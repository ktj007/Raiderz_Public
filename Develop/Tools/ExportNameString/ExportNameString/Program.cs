using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Runtime.InteropServices;


namespace ExportNameString
{
    static class Program
    {
        [DllImport("kernel32.dll")]
        static extern bool AttachConsole(int dwProcessId);
        private const int ATTACH_PARENT_PROCESS = -1;


        [STAThread]        
        static void Main(string[] args)
        {
            Argument arg = new Argument(args);
            arg.Parser();

            if (arg.HasKey("?"))
            {
                AttachConsole(ATTACH_PARENT_PROCESS);
                
                Console.WriteLine("\n");
                Console.WriteLine("ExportNameString -file <string[]> -path <string>");
                Console.WriteLine("    file: 추출할 XML 스크립트 파일");
                Console.WriteLine("    path: 목표 폴더");                
                Console.WriteLine("\n");

                return;
            }

            if (arg.HasKey("file") && arg.HasKey("path"))
            {
                foreach (string file in arg.GetValue("file"))
                {
                    Exporter.Export(file, arg.GetValue("path")[0]);
                }
                
                return;
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }        
    }
}
