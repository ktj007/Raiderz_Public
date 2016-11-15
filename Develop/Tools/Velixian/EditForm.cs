using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    public class EditForm : DockContent
    {
        public virtual void InitEditForm(MainForm mainForm)
        {

        }

        protected virtual bool OnEditFormKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.F)
            {
                Global._mainForm.FindStringFocus();
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.S)
            {
                Global._mainForm.SaveAll();
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D1)
            {
                Global._VelixianForms.ShowForm("NPC");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D2)
            {
                Global._VelixianForms.ShowForm("QUEST");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D3)
            {
                Global._VelixianForms.ShowForm("CONDITION");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D4)
            {
                Global._VelixianForms.ShowForm("ITEMCONDITION");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D5)
            {
                Global._VelixianForms.ShowForm("ITEM");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D6)
            {
                Global._VelixianForms.ShowForm("LOOTING");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D7)
            {
                Global._VelixianForms.ShowForm("DIALOG");
                return true;
            }
            else if (e.Control && e.KeyCode == Keys.D8)
            {
                Global._VelixianForms.ShowForm("STRING");
                return true;
            }
            else if (e.KeyCode == Keys.F5)
            {
                Global._mainForm.ReloadAllXmlFile();
                return true;
            }

            return false;
        }

    }
}