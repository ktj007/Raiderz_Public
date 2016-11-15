using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace UnitTestTool
{
    class KeyInputHandler
    {
        private readonly int
            CTRL_1 = 1,
            CTRL_2 = 2,
            CTRL_3 = 3,
            CTRL_4 = 4,
            CTRL_INSERT = 5,
            CTRL_DELETE = 6,

            ALT_1 = 11,
            ALT_2 = 12,
            ALT_3 = 13,
            ALT_4 = 14,
            ALT_INSERT = 15,
            ALT_DELETE = 16;


        public void RegisterHotKey(IntPtr Handle)
        {
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_1, 2, (int)Keys.D1);
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_2, 2, (int)Keys.D2);
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_3, 2, (int)Keys.D3);
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_4, 2, (int)Keys.D4);
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_INSERT, 2, (int)Keys.Insert);
            Win32Until.HotKey.RegisterHotKey(Handle, CTRL_DELETE, 2, (int)Keys.Delete);


            Win32Until.HotKey.RegisterHotKey(Handle, ALT_1, 1, (int)Keys.D1);
            Win32Until.HotKey.RegisterHotKey(Handle, ALT_2, 1, (int)Keys.D2);
            Win32Until.HotKey.RegisterHotKey(Handle, ALT_3, 1, (int)Keys.D3);
            Win32Until.HotKey.RegisterHotKey(Handle, ALT_4, 1, (int)Keys.D4);
            Win32Until.HotKey.RegisterHotKey(Handle, ALT_INSERT, 1, (int)Keys.Insert);
            Win32Until.HotKey.RegisterHotKey(Handle, ALT_DELETE, 1, (int)Keys.Delete);
        }

        public bool Handle_Global(Message m, ConfigData configData)
        {
            IDataObject iData = new DataObject();
            iData = Clipboard.GetDataObject();
            string strClipBoadrd = (string)iData.GetData(DataFormats.Text);

            return true;
        }

        public bool Handle_Local(object sender, KeyEventArgs e, Form1 form)
        {
            IDataObject iData = new DataObject();
            iData = Clipboard.GetDataObject();
            string strClipBoadrd = (string)iData.GetData(DataFormats.Text);

            if (Keys.Control == e.Modifiers)
            {
                switch (e.KeyCode)
                {
                    case Keys.Z:
                        {
                            form.Reset();
                        }
                        break;
                    default:
                        {
                            return false;
                        }
                }
            }
            else
            {
                switch (e.KeyCode)
                {
                    case Keys.Escape:
                        {
                            form.Close();
                        }
                        break;
                    case Keys.F1:
                        {
                            form.FocusToEnableTab();
                        }
                        break;
                    case Keys.F2:
                        {
                            form.FocusToDisableTab();
                        }
                        break;
                    case Keys.F3:
                        {
                            form.FocusToOptionTab();
                        }
                        break;
                    default:
                        {
                            return false;
                        }
                }
            }

            return true;
        }
    }    
}
