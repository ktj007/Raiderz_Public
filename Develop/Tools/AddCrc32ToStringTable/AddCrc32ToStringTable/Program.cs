using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace AddCrc32ToStringTable
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static int Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);


            Form1 form = new Form1();

            if (0 < args.Length)
            {
                if (form.AddCrc32(args[0]))
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }

            Application.Run(form);

            return 0;
        }
    }
}
