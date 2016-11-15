using System;
using System.Windows.Forms;
using System.Collections.Generic;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;
using System.IO;

namespace Velixian
{
    public partial class MainForm : Form
    {
        public VelixianForms _Forms;

        private DeserializeDockContent m_deserializeDockContent;

        private EditForm_NPC m_editFormNPC = new EditForm_NPC();
        private EditForm_Quest m_editFormQuest = new EditForm_Quest();
        private EditForm_Condition m_editFormCondition = new EditForm_Condition();
        private EditForm_ItemCondition m_editFormItemCondition = new EditForm_ItemCondition();
        private EditForm_Item m_editFormItem = new EditForm_Item();
        private EditForm_Looting m_editFormLooting = new EditForm_Looting();
        private EditForm_Dialog m_editFormDIalog = new EditForm_Dialog();
        private EditForm_String m_editFormString = new EditForm_String();
        private OutputForm m_OutputForm = new OutputForm();

        private void InitForms()
        {
            _Forms = new VelixianForms();

            Global.Init(this, _Forms, xmlCore);

            LoadDockConfig();

            InitEditForms();

            //
            cbFindType.SelectedIndex = 0;
            cbFindIDOnly.SelectedIndex = 2;
            cbStrict.SelectedIndex = 0;

            LoadTabEnvironment(0);

            m_editFormNPC.Show(dockPanel);
        }

        private void LoadDockConfig()
        {
            string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "DockPanel.config");

            if (File.Exists(configFile))
                dockPanel.LoadFromXml(configFile, m_deserializeDockContent);
        }

        private void InitEditForms()
        {
            InitEditForm(m_editFormNPC);
            InitEditForm(m_editFormQuest);
            InitEditForm(m_editFormCondition);
            InitEditForm(m_editFormItemCondition);
            InitEditForm(m_editFormItem);
            InitEditForm(m_editFormLooting);
            InitEditForm(m_editFormDIalog);
            InitEditForm(m_editFormString);
            InitEditForm(m_OutputForm);
        }

        public void AddForm(EditForm _form, ValixianFormType editFormType, ListView lvMain, String keyword, bool bLoadingFailed, ToolStripMenuItem menu, TabInitializeHandler handler, TabSaveHandler save_handler)
        {
            _Forms.Add(cbFindType, _form, editFormType, lvMain, keyword, bLoadingFailed, menu, handler, save_handler);
        }

        private void InitEditForm(EditForm newEditForm)
        {
            if (dockPanel.DocumentStyle == DocumentStyle.SystemMdi)
            {
                newEditForm.MdiParent = this;
                newEditForm.Show();
            }
            else
            {
                newEditForm.Show(dockPanel);
            }

            newEditForm.InitEditForm(this);
        }

        public void SetStatusLabelText(String Text)
        {
            staMain.Text = Text;
        }

        public void SetListViewStatusLabelText(int nIndex, int nMaxCount)
        {
            staListViewIndex.Text = String.Format("({0} / {1})", nIndex, nMaxCount);
        }

        private IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(EditForm_NPC).ToString())
                return m_editFormNPC;
            else if (persistString == typeof(EditForm_Quest).ToString())
                return m_editFormQuest;
            else if (persistString == typeof(EditForm_Condition).ToString())
                return m_editFormCondition;
            else if (persistString == typeof(EditForm_ItemCondition).ToString())
                return m_editFormItemCondition;
            else if (persistString == typeof(EditForm_Item).ToString())
                return m_editFormItem;
            else if (persistString == typeof(EditForm_Looting).ToString())
                return m_editFormLooting;
            else if (persistString == typeof(EditForm_Dialog).ToString())
                return m_editFormDIalog;
            else if (persistString == typeof(EditForm_String).ToString())
                return m_editFormString;
            else if (persistString == typeof(OutputForm).ToString())
                return m_OutputForm;

            return null;
        }

        public void ShowEditForm(VelixianForm velixianForm)
        {
            velixianForm.form.Show(dockPanel);
        }

        private void MainTabIndexChanged(object sender, EventArgs e)
        {
/*
            Boolean bUpdate = false;

            Cursor csOld = Cursor;
            Cursor = Cursors.WaitCursor;

            SaveTabEnvironment(_nLastIndex);

            // 지난 탭 전용메뉴를 끈다.
            if (_nLastIndex >= 0 && _Tabs._TabPages[_nLastIndex].Menu != null) _Tabs._TabPages[_nLastIndex].Menu.Visible = false;

            _nLastIndex = tabMain.SelectedIndex;
            cbFindType.SelectedIndex = _nLastIndex;

            // 새로운 탭 전용메뉴를 켠다.
            if (_Tabs._TabPages[_nLastIndex].Menu != null) _Tabs._TabPages[_nLastIndex].Menu.Visible = true;

            // 뭔가 변경된게 있다면 컨트롤들을 전부 업데이트한다.
            for (int i = 0; i < VelixianTab.PAGEMAX; i++) if (!_Tabs._TabPages[i].Updated) bUpdate = true;

            if (bUpdate) for (int i = 0; i < VelixianTab.PAGEMAX; i++) _Tabs._TabPages[i].Update();

            Cursor = csOld;
            LoadTabEnvironment(tabMain.SelectedIndex);
*/
        }

        public void Output(string strMessage)
        {
            m_OutputForm.Output(strMessage);
        }
    }

    public class VelixianFindType
    {
        public String Text;
        public VelixianForm Tag;

        public VelixianFindType(String text, VelixianForm tab)
        {
            Text = text;
            Tag = tab;
        }

        public override string ToString()
        {
            return Text;
        }
    }

    public delegate void TabInitializeHandler();
    public delegate void TabSaveHandler();
}
