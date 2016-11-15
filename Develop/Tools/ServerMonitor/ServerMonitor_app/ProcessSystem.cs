using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using SMCommon;
using System.Threading;

namespace ServerMonitor
{
    public class ProcessSystem
    {
        [DllImport("user32.dll")]
        public static extern int SendMessage(int hWnd, int wMsg, int wParam, int lParam);
        [DllImport("user32.dll")]
        public static extern int GetWindow(int hWnd, int wCmd);
        [DllImport("user32.dll")]
        public static extern int GetWindowThreadProcessId(int hWnd, out int nProcessId);

        private const int WM_DESTROY_SERVER = 0x0400 + 25;
        private const int GW_HWNDFIRST = 0;
        private const int GW_HWNDNEXT = 2;

        private const int TIME_MS_WAIT_TERMINATE_EVENT_RESPONSE = 10000;

        private LinkedList<Process> m_listProcess = new LinkedList<Process>();

        public virtual bool StartProcess(string strProcessExePath, out int nProcessID)
        {
            nProcessID = 0;
            if (strProcessExePath.Length == 0) return false;

            Process process = null;

            try
            {
                process = Process.Start(strProcessExePath);
                nProcessID = process.Id;
            }
            catch (SystemException exception)
            {
                FileLog.Instance.Write("실패! 프로세스를 시작하지 못했습니다.({0}) 예외 메시지 : {1}", strProcessExePath, exception.Message);
                nProcessID = 0;
                return false;
            }
            finally
            {
                AddProcess(process);
            }
            return true;
        }

        // 윈도우 메시지를 받을 수 있는 서버를 종료할 때 사용.
        // 콘솔 및 서비스 모드 서버에서는 사용하지 않습니다.
        public virtual bool SendDestroyMessage(int nProcessID)
        {
            if (nProcessID == 0) return false;

            Process process = FindProcess(nProcessID);
            if (null == process) return false;

            bool bSend = false;

            try
            {
                int nTargetProcessID = 0;
                int nTargetWnd = GetWindow(Process.GetCurrentProcess().MainWindowHandle.ToInt32(), GW_HWNDFIRST);
                while (nTargetWnd != 0)
                {
                    GetWindowThreadProcessId(nTargetWnd, out nTargetProcessID);

                    if (nTargetProcessID == nProcessID)
                    {
                        SendMessage(nTargetWnd, WM_DESTROY_SERVER, 0, 0);
                        bSend = true;
                    }

                    nTargetWnd = GetWindow(nTargetWnd, GW_HWNDNEXT);
                }
            }
            catch (SystemException exception)
            {
                FileLog.Instance.Write("실패! 프로세스를 정지하지 못했습니다. (Name:{0}, ID:{1}) 예외 메시지 : {2}", process.ProcessName, nProcessID, exception.Message);
                return false;
            }
            finally
            {
                process.Close();
                RemoveProcess(process);
            }
            return bSend;
        }

        public virtual bool PostSignal_Terminate(ServerInfo refServerInfo)
        {
            if (null == refServerInfo) return false;

            refServerInfo.SetEventSignal_Terminate();

            return true;
        }

        public virtual bool IsRunningProcess(string strProcessName, int nProcessID)
        {
            if (strProcessName.Length == 0 || nProcessID == 0) return false;

            Process[] serverProcesses;
            serverProcesses = Process.GetProcessesByName(strProcessName);

            foreach (Process serverProcess in serverProcesses)
            {
                if (serverProcess.Id == nProcessID)
                    return true;
            }

            return false;
        }

        private void AddProcess(Process process)
        {
            m_listProcess.AddLast(process);
        }
        private void RemoveProcess(Process process)
        {
            m_listProcess.Remove(process);
        }
        private Process FindProcess(int nProcessID)
        {
            foreach (Process serverProcess in m_listProcess)
            {
                if (serverProcess.Id == nProcessID)
                    return serverProcess;
            }
            return null;
        }

    }
}
