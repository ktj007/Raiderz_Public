using System;
using System.Text;
using System.Runtime.InteropServices;

namespace UnitTestTool
{
    namespace Win32Until
    {
        class INI
        {
            [DllImport("kernel32")]
            public static extern bool WritePrivateProfileString(string lpAppName, string lpKeyName, string lpString, string lpFileName);

            [DllImport("kernel32")]
            public static extern int GetPrivateProfileInt(string lpAppName, string lpKeyName, int nDefault, string lpFileName);

            [DllImport("kernel32")]
            public static extern int GetPrivateProfileString(string lpAppName, string lpKeyName, string lpDefault, StringBuilder lpReturnedString, int nSize, string lpFileName);
        }

        class Mutex
        {
            [DllImport("user32.dll")]
            public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
            [DllImport("user32.dll")]
            public static extern void BringWindowToTop(IntPtr hWnd);
            [DllImport("user32.dll")]
            public static extern void SetForegroundWindow(IntPtr hWnd);
        }        

        class ClipBoard
        {
            [DllImport("User32.dll")]
            public static extern int SetClipboardViewer(int hWndNewViewer);

            [DllImport("User32.dll", CharSet=CharSet.Auto)]
            public static extern bool ChangeClipboardChain(IntPtr hWndRemove, IntPtr hWndNewNext);

            [DllImport("user32.dll", CharSet=CharSet.Auto)]
            public static extern int SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, IntPtr lParam);
        }

        class HotKey
        {
            [DllImport("user32.dll")]
            public static extern bool RegisterHotKey(IntPtr hWnd, int id, int fsModifiers, int vlc);
        }
    }
}
