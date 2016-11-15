using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ColtViewer
{
    public partial class frmMain
    {
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
