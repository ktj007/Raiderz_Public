using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;
using System.Runtime.InteropServices;
using SMCommon;

namespace ServerMonitor
{
    static class main
    {
        private static readonly string MUTEX_NAME = "ServerMonitor";
        private static Mutex m_mutexServerMonitorClient = null;

        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
        [DllImport("user32.dll")]
        public static extern void BringWindowToTop(IntPtr hWnd);
        [DllImport("user32.dll")]
        public static extern void SetForegroundWindow(IntPtr hWnd);

        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static int Main()
        {
            System.Environment.CurrentDirectory = Application.StartupPath;

            InitLog();

            if (CheckOverlappedExecute() == true)
            {
                ActivateAlreadyOpenedServerMonitor();
                return 0;
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            ServerMonitor serverMonitor = new ServerMonitor();
            FormServerMonitor formServerMonitor = new FormServerMonitor(serverMonitor);

            if (serverMonitor.Init(formServerMonitor) == false)
            {
                FileLog.Instance.Write("실패! ServerMonitor 초기화에 실패했습니다!");
                serverMonitor.Destroy();
                return -1;
            }
            Application.Run(formServerMonitor);

            return 0;
        }

        private static void InitLog()
        {
            if (System.IO.Directory.Exists(".\\log") == false)
            {
                System.IO.Directory.CreateDirectory(".\\log");
            }

            string strLogFileName;
            DateTime now = DateTime.Now;
            strLogFileName = ".\\log\\ServerMonitor_" + now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
            FileLog.Instance.Init(strLogFileName);
        }

        private static bool CheckOverlappedExecute()
        {
            bool bCreatedNew;
            try
            {
                m_mutexServerMonitorClient = new Mutex(true, MUTEX_NAME, out bCreatedNew);
            }
            catch
            {
                return true;
            }

            if (bCreatedNew == false) return true;

            return false;
        }

        private static void ActivateAlreadyOpenedServerMonitor()
        {
            string strProcessName = Process.GetCurrentProcess().ProcessName;
            int nProcessID = Process.GetCurrentProcess().Id;

            FileLog.Instance.Write("중복 실행 되었습니다. ProcessName:{0}, ProcessID:{1}", strProcessName, nProcessID);

            Process[] processesServerMonitor;
            processesServerMonitor = Process.GetProcessesByName(strProcessName);

            foreach (Process processServerMonitor in processesServerMonitor)
            {
                if (processServerMonitor.Id != nProcessID)
                {
                    FileLog.Instance.Write("종료! ServerMonitor 가 이미 실행 중입니다.");
                    ShowWindow(processServerMonitor.MainWindowHandle, 1);
                    BringWindowToTop(processServerMonitor.MainWindowHandle);
                    SetForegroundWindow(processServerMonitor.MainWindowHandle);
                    break;
                }
            }
        }

    }
}