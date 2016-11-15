using System;
using System.Windows.Forms;
using System.Collections.Generic;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    static class Global
    {
        static public MainForm _mainForm { get; set; }
        static public VelixianForms _VelixianForms { get; set; }
        static public SHXmlCore _xmlCore { get; set; }

        static public void Init(MainForm mainForm, VelixianForms velixianForms, SHXmlCore xmlCore)
        {
            _mainForm = mainForm;
            _VelixianForms = velixianForms;
            _xmlCore = xmlCore;
        }

        static public void Output(string strMessage)
        {
            _mainForm.Output(strMessage);
        }
        static public void OutputError(string strMessage)
        {
            _mainForm.Output("[Error] " + strMessage + "\n");
        }
        static public void OutputWarning(string strMessage)
        {
            _mainForm.Output("[Warning] " + strMessage + "\n");
        }

    }


}