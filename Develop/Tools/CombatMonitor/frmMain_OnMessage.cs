using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CombatMonitor.Util;
using System.Runtime.InteropServices;

namespace CombatMonitor
{
    public partial class frmMain
    {
#region IPC_Implements
        [DllImport("User32.dll", CharSet = CharSet.Auto, EntryPoint = "SendMessage")]
        public static extern int SendMessage(IntPtr hWnd, Int32 Msg, IntPtr wParam, ref Win32API.COPYDATASTRUCT lParam);

        [DllImport("User32.dll", CharSet = CharSet.Auto, EntryPoint = "SendMessage")]
        public static extern int SendMessage(IntPtr hWnd, Int32 Msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        public class Win32API
        {
            public const Int32 WM_COPYDATA = 0x004A;
            public struct COPYDATASTRUCT
            {
                public IntPtr dwData;
                public int cbData;
                [MarshalAs(UnmanagedType.LPStr)]
                public String lpData;
            }
        }

        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll")]
        public static extern int GetWindowText(IntPtr hWnd, StringBuilder text, int count);

        [DllImport("user32")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool EnumChildWindows(IntPtr window, EnumWindowProc callback, IntPtr i);

        /// <summary>
        /// Returns a list of child windows
        /// </summary>
        /// <param name="parent">Parent of the windows to return</param>
        /// <returns>List of child windows</returns>
        public static List<IntPtr> GetChildWindows(IntPtr parent)
        {
            List<IntPtr> result = new List<IntPtr>();
            GCHandle listHandle = GCHandle.Alloc(result);
            try
            {
                EnumWindowProc childProc = new EnumWindowProc(EnumWindow);
                EnumChildWindows(parent, childProc, GCHandle.ToIntPtr(listHandle));
            }
            finally
            {
                if (listHandle.IsAllocated)
                    listHandle.Free();
            }
            return result;
        }

        /// <summary>
        /// Callback method to be used when enumerating windows.
        /// </summary>
        /// <param name="handle">Handle of the next window</param>
        /// <param name="pointer">Pointer to a GCHandle that holds a reference to the list to fill</param>
        /// <returns>True to continue the enumeration, false to bail</returns>
        private static bool EnumWindow(IntPtr handle, IntPtr pointer)
        {
            GCHandle gch = GCHandle.FromIntPtr(pointer);
            List<IntPtr> list = gch.Target as List<IntPtr>;
            if (list == null)
            {
                throw new InvalidCastException("GCHandle Target could not be cast as List<IntPtr>");
            }
            list.Add(handle);
            //  You can modify this to check to see if you want to cancel the operation, then return a null here
            return true;
        }

        /// <summary>
        /// Delegate for the EnumChildWindows method
        /// </summary>
        /// <param name="hWnd">Window handle</param>
        /// <param name="parameter">Caller-defined variable; we use it for a pointer to our list</param>
        /// <returns>True to continue enumerating, false to bail.</returns>
        public delegate bool EnumWindowProc(IntPtr hWnd, IntPtr parameter);
#endregion

        public void SendToServer(string strText)
        {
            IntPtr hwnd = IntPtr.Zero;
            List<IntPtr> listHandles = GetChildWindows(IntPtr.Zero);
            foreach (IntPtr each in listHandles)
            {
                const int nChars = 256;
                StringBuilder Buff = new StringBuilder(nChars);

                if (GetWindowText(each, Buff, nChars) > 0)
                {
                    const string strServerName = "GameServer [";
                    int nRes = String.Compare(Buff.ToString(), 0, strServerName, 0, strServerName.Length);
                    if (nRes == 0)
                    {
                        hwnd = each;
                        break;
                    }
                }
            }

            if (hwnd == IntPtr.Zero)
                return;

            Win32API.COPYDATASTRUCT cds = new Win32API.COPYDATASTRUCT();
            cds.dwData = (IntPtr)1001;
            cds.cbData = Encoding.Default.GetBytes(strText).Length + 1;
            cds.lpData = strText;
            SendMessage(hwnd, Win32API.WM_COPYDATA, IntPtr.Zero, ref cds);
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case Win32API.WM_COPYDATA:
                    Win32API.COPYDATASTRUCT cDataStruct = new Win32API.COPYDATASTRUCT();
                    cDataStruct = (Win32API.COPYDATASTRUCT)Marshal.PtrToStructure(m.LParam, typeof(Win32API.COPYDATASTRUCT));

                    ParseXML(cDataStruct.lpData);

                    break;
                default:
                    break;
            }
            base.WndProc(ref m);
        }
    };
}
