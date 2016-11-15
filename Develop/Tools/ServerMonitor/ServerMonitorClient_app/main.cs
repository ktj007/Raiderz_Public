using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using SMCommon;

namespace ServerMonitorClient
{
    static class main
    {
        private static readonly string MUTEX_NAME = "ServerMonitorClient";
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
        static int Main(string[] args)
        {
            System.Environment.CurrentDirectory = Application.StartupPath;

            InitLog();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Form form = null;
            if (IsAutoMode(args))
            {
                form = CreateAutoModeClient(args);
            }
            else
            {
                if (CheckOverlappedExecute() == true)
                {
                    ActivateAlreadyOpenedServerMonitorClient();
                    return 0;
                }

                form = CreateServerMonitorClient();
            }

            if (form == null)
                return -1;
            
            Application.Run(form);

            return 0;
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
        private static void ActivateAlreadyOpenedServerMonitorClient()
        {
            string strProcessName = Process.GetCurrentProcess().ProcessName;
            int nProcessID = Process.GetCurrentProcess().Id;

            Process[] processesServerMonitorClient;
            processesServerMonitorClient = Process.GetProcessesByName(strProcessName);

            foreach (Process processServerMonitorClient in processesServerMonitorClient)
            {
                if (processServerMonitorClient.Id != nProcessID)
                {
                    ShowWindow(processServerMonitorClient.MainWindowHandle, 1);
                    BringWindowToTop(processServerMonitorClient.MainWindowHandle);
                    SetForegroundWindow(processServerMonitorClient.MainWindowHandle);
                    break;
                }
            }
        }
        private static void InitLog()
        {
            string strLogFileName = ".\\ServerMonitorClient_Log.txt";
            FileLog.Instance.Init(strLogFileName);
        }
        private static bool IsAutoMode(string[] args)
        {
            return (args.Length > 0);
        }
        private static FormAutoModeClient CreateAutoModeClient(string[] args)
        {
            AutoModeClient autoModeClient = new AutoModeClient();
            FormAutoModeClient formAutoModeClient = new FormAutoModeClient(autoModeClient);

            if (autoModeClient.Init(formAutoModeClient, args) == false)
                return null;

            if (autoModeClient.StartAutoMode() == false)
                return null;

            return formAutoModeClient;
        }
        private static FormServerMonitorClient CreateServerMonitorClient()
        {
            ServerMonitorClient serverMonitorClient = new ServerMonitorClient();
            FormServerMonitorClient formServerMonitorClient = new FormServerMonitorClient(serverMonitorClient);

            if (serverMonitorClient.Init(formServerMonitorClient) == false)
                return null;

            return formServerMonitorClient;
        }

    }
}
