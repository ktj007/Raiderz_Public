using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace ExportAreaNameString
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main(String[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            
            Form1 form = new Form1();
            
            if (0 < args.Length)
            {
                form.Export(args[0]);
            }
            else
            {
                Application.Run(form);
            }
        }
    }
}
