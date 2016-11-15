using System;
using System.Diagnostics;
using System.ComponentModel;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Collections;
using System.Collections.Generic;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    [ComVisible(true)]
    public partial class MainForm : Form
    {
        public SHXmlCore xmlCore;

        #region 생성자
        public MainForm()
        {
            InitializeComponent();

            Global._mainForm = this;
            m_deserializeDockContent = new DeserializeDockContent(GetContentFromPersistString);
        }
        #endregion

        #region 초기화와 종료(Load/Closing), 스플래시 스크린

        private void frmMain_Load(object sender, EventArgs e)
        {
            // 말하자면 My InitializeComponent

            LoadEnvironment();

            // 스플래시 스크린을 띄운다.
            ShowSplashScreen();
        }

        public void RefreshData()
        {
            if (xmlCore != null)
            {
                xmlCore.Refresh();
            }
            else
            {
                xmlCore = new SHXmlCore();
            }


        }

        public void OnLoadAfter(bool bFirst)
        {
            RefreshData();
            if (bFirst)
            {
                InitForms();
            }
        }

        public void ShowSplashScreen()
        {
            SplashForm splash = new SplashForm();
            try
            {
                splash.SetLoadAfter(OnLoadAfter);
                splash.ShowDialog();
            }
            catch (ArgumentNullException ex)
            {
                MessageBox.Show(ex.Message + " - " + ex.ParamName);
                Environment.Exit(-1);
            }
            catch (NullReferenceException ex)
            {
                MessageBox.Show(ex.Message + " - " + ex.Source);
                Environment.Exit(-1);
            }
            catch (Exception ex)
            {
                if (ex.InnerException != null)
                {
                    MessageBox.Show(ex.Message + "\n\n" + ex.InnerException.Message + "\n\n" + ex.InnerException.StackTrace, "로딩 에러");
                }
                else
                {
                    MessageBox.Show(ex.Message + "\n\n" + ex.StackTrace, "로딩 에러");
                }
                Environment.Exit(-1);
            }

            staPath.Text = SHGlobal.GetFolder();
            fsWatcher.Path = SHGlobal.GetFolder();
        }


        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            for (int i = 0; i < (int)ValixianFormType.PAGE_MAX; i++)
            {
                if (_Forms._TabPages[i].Modified && _Forms._TabPages[i].SaveHandler != null)
                {
                    DialogResult result = MessageBox.Show(_Forms._TabPages[i].Text + " 파일이 변경되었습니다. 저장하시겠습니까?", Application.ProductName, MessageBoxButtons.YesNoCancel);
                    
                    if (result == DialogResult.Yes)
                    {
                        _Forms._TabPages[i].TrySave();
                    }
                    else if (result == DialogResult.Cancel)
                    {
                        e.Cancel = true;
                        return;
                    }
                }
            }

            SaveEnvironment();

            SaveDockConfig();

        }

        private void SaveDockConfig()
        {
            string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "DockPanel.config");
            dockPanel.SaveAsXml(configFile);
        }


        #endregion

        #region 윈도우 이벤트

        #region 메뉴 이벤트

        private void mnuFileExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void mnuSetPath_Click(object sender, EventArgs e)
        {
            if (SHGlobal.SetupFolder() == true) RefreshData();
        }

        private void mnuHelpAbout_Click(object sender, EventArgs e)
        {
            AboutBox dlg = new AboutBox();
            dlg.ShowDialog();
        }

        private void mnuFileSaveAll_Click(object sender, EventArgs e)
        {
            SaveAll();
        }

        public void SaveAll()
        {
            fsWatcher.EnableRaisingEvents = false;

            for (int i = 0; i < (int)ValixianFormType.PAGE_MAX; i++) _Forms._TabPages[i].TrySave();

            fsWatcher.EnableRaisingEvents = true;
        }

        private void mnuItemDroperSave_Click(object sender, EventArgs e)
        {
            xmlCore.SaveItemDropers();
        }

        private void mnuItemQuestRewarder_Click(object sender, EventArgs e)
        {
            xmlCore.SaveItemQuestRewarders();
        }

        private void spellCheck_Click(object sender, EventArgs e)
        {
            SpellCheckForm spelldlg = new SpellCheckForm();
            spelldlg.Show(this);
        }

        #endregion

        #region 파일 시스템 감시자 관련

        private void MonitoredFileChanged(object sender, System.IO.FileSystemEventArgs e)
        {
            if (!Option.AutoMonitorFile) return;

            bool bMonitoredFile = false;

            foreach (SHSmartCheckFile scf in SHGlobal.GetChecklist())
            {
                if (scf.CheckType == SHSCF_Type.Both || scf.CheckType == SHSCF_Type.Server)
                {
                    if (e.FullPath.ToUpper() == (SHGlobal.GetServerSystemFolder() + scf.FilePath).ToUpper())
                    {
                        bMonitoredFile = true;
                        break;
                    }
                }

                if (scf.CheckType == SHSCF_Type.Both || scf.CheckType == SHSCF_Type.Client)
                {
                    if (e.FullPath.ToUpper() == (SHGlobal.GetClientSystemFolder() + scf.FilePath).ToUpper())
                    {
                        bMonitoredFile = true;
                        break;
                    }
                }
            }

            if (bMonitoredFile == true)
            {
                // 모든 모니터 중지
                fsWatcher.EnableRaisingEvents = false;

                ShowSplashScreen();
                staMain.Text = e.Name + " 파일이 변경되었습니다. 자동갱신했습니다.";

                fsWatcher.EnableRaisingEvents = true;
            }
        }

        #endregion

        #region NPC 편집 관련


        private void mnuViewMonster_Click(object sender, EventArgs e)
        {
            mnuViewMonster.CheckState = (mnuViewMonster.CheckState == CheckState.Checked) ? CheckState.Unchecked : CheckState.Checked;
            InitEditForm(m_editFormNPC);
        }

        private void mnuViewNPC_Click(object sender, EventArgs e)
        {
            mnuViewNPC.CheckState = (mnuViewNPC.CheckState == CheckState.Checked) ? CheckState.Unchecked : CheckState.Checked;
            InitEditForm(m_editFormNPC);
        }

        private void mnuViewObject_Click(object sender, EventArgs e)
        {
            mnuViewObject.CheckState = (mnuViewObject.CheckState == CheckState.Checked) ? CheckState.Unchecked : CheckState.Checked;
            InitEditForm(m_editFormNPC);
        }

        private void mnuViewHideNoninteract_Click(object sender, EventArgs e)
        {
            mnuViewHideNoninteract.CheckState = (mnuViewHideNoninteract.CheckState == CheckState.Checked) ? CheckState.Unchecked : CheckState.Checked;
            InitEditForm(m_editFormNPC);
        }

        private void mnuViewHideNoLoot_Click(object sender, EventArgs e)
        {
            mnuViewHideNoLoot.CheckState = (mnuViewHideNoLoot.CheckState == CheckState.Checked) ? CheckState.Unchecked : CheckState.Checked;
            InitEditForm(m_editFormNPC);
        }

        #endregion

        #endregion

        #region 검색관련

        public void FindItem()
        {
            ListViewItem lvi = null;
            VelixianFindType ft = (VelixianFindType)cbFindType.Items[cbFindType.SelectedIndex];
            ListView searchView = ft.Tag.MainList;
            if (searchView == null) return;

            int nSearchIndex = (searchView.SelectedIndices.Count > 0 && searchView.SelectedIndices[0] != searchView.Items.Count) ? searchView.SelectedIndices[0] + 1 : 0;

            if (cbFindIDOnly.SelectedIndex == 2)
            {
                lvi = SHListViewUtil.FullTextSearch(searchView, cbFindString.Text, nSearchIndex);
                if (lvi == null) lvi = SHListViewUtil.FullTextSearch(searchView, cbFindString.Text, 0);
            }
            else
            {
                lvi = searchView.FindItemWithText(cbFindString.Text, (cbFindIDOnly.SelectedIndex != 0), nSearchIndex, (cbStrict.SelectedIndex == 0));
                if (lvi == null) lvi = searchView.FindItemWithText(cbFindString.Text, (cbFindIDOnly.SelectedIndex != 0), 0, (cbStrict.SelectedIndex == 0));
            }

            if (lvi != null)
            {
                Global._VelixianForms.ShowForm(cbFindType.SelectedIndex);

                SHListViewUtil.SelectIndex(searchView, lvi.Index);
            }
            else
            {
                staMain.Text = "일치하는 값이 없습니다.";
                MessageBox.Show(staMain.Text, Application.ProductName);
            }
        }

        public void FindStringFocus()
        {
            cbFindString.Focus();
        }

        private void mnuEditFind_Click(object sender, EventArgs e)
        {
            FindStringFocus();
        }

        private void cbFindString_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                cbFindString.AutoCompleteCustomSource.Add(cbFindString.Text);
                FindItem();
            }
        }

        private void cbFindIDOnly_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbStrict.Enabled = (cbFindIDOnly.SelectedIndex != 2) ? true : false;
        }

        #endregion



        #region 도구 관련

        private void mnuRunServer_Click(object sender, EventArgs e)
        {
            LaunchExternalProgram(SHGlobal.GetFolder() + "\\Dev\\GameServer\\GameServer.exe", null, false);
        }

        private void mnuRunClient_Click(object sender, EventArgs e)
        {
            LaunchExternalProgram(SHGlobal.GetFolder() + "\\Raiderz.exe", null, false);
        }


        private void mnuSVNUpdate_Click(object sender, EventArgs e)
        {
            String param = "update \"" + SHGlobal.GetFolder() + "\"";
            MessageBox.Show(param);
            LaunchExternalProgram("svn.exe", param, true);
        }

        private void LaunchExternalProgram(String path, String param, Boolean bWait)
        {
            try
            {
                ProcessStartInfo psi = new ProcessStartInfo(path);
                
                if (param != null) psi.Arguments = param;

                Process.Start(psi);
            }
            catch (Exception ex)
            {
                if (ex.InnerException != null)
                {
                    MessageBox.Show(ex.Message + "\n\n" + ex.InnerException.Message, Application.ProductName);
                }
                else
                {
                    MessageBox.Show(ex.Message, Application.ProductName);
                }
            }
        }

        #endregion        

        private void mnuExportLootReport_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.SaveFileDialog DialogSave = new System.Windows.Forms.SaveFileDialog();

            DialogSave.DefaultExt = "csv";

            DialogSave.Filter = "CSV file (*.csv)|*.csv|All files (*.*)|*.*";

            DialogSave.AddExtension = true;

            DialogSave.RestoreDirectory = true;

            DialogSave.Title = "어디에 저장하시겠습니까?";

            //            DialogSave.InitialDirectory = @"C:/";

            if (DialogSave.ShowDialog() == DialogResult.OK)
            {
                SoulHunt.LootReportExporter lootReportExporter = new SoulHunt.LootReportExporter();
                lootReportExporter.Export(DialogSave.FileName, xmlCore);
                lootReportExporter = null;
            }

            DialogSave.Dispose();
            DialogSave = null;
        }

        private void menuNPCSpawnInfo_Click(object sender, EventArgs e)
        {
            xmlCore.SaveNPCSpawnInfo();
        }

        private void menuRecipeByNPC_Click(object sender, EventArgs e)
        {
            xmlCore.SaveRecipeByNPC();
        }

        private void mnuExportNPCShopReport_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.SaveFileDialog DialogSave = new System.Windows.Forms.SaveFileDialog();

            DialogSave.DefaultExt = "csv";

            DialogSave.Filter = "CSV file (*.csv)|*.csv|All files (*.*)|*.*";

            DialogSave.AddExtension = true;

            DialogSave.RestoreDirectory = true;

            DialogSave.Title = "어디에 저장하시겠습니까?";

            if (DialogSave.ShowDialog() == DialogResult.OK)
            {
                SoulHunt.NPCShopReportExporter npcShopReportExporter = new SoulHunt.NPCShopReportExporter();
                npcShopReportExporter.Export(DialogSave.FileName, xmlCore);
                npcShopReportExporter = null;
            }

            DialogSave.Dispose();
            DialogSave = null;
        }

        private void tvConditionDetail_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void tvCondDetail_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void subLootAddSilver_Click(object sender, EventArgs e)
        {

        }

        private void mnuViewEditForm_NPC_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("NPC").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_Quest_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("QUEST").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_Condition_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("CONDITION").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_ItemCondition_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("ITEMCONDITION").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_Item_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("ITEM").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_Looting_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("LOOTING").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_Dialog_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("DIALOG").form;
            thisEditForm.Show(dockPanel);
        }

        private void mnuViewEditForm_String_Click(object sender, EventArgs e)
        {
            EditForm thisEditForm = Global._VelixianForms.FindForm("STRING").form;
            thisEditForm.Show(dockPanel);
        }


        private void InitEditFormDock(EditForm newEditForm)
        {
            if (dockPanel.DocumentStyle == DocumentStyle.SystemMdi)
            {
                newEditForm.MdiParent = this;
                newEditForm.Show();
            }
            else
            {
                newEditForm.Show(dockPanel, DockState.Document);
            }
        }

        private void mnuClearDockConfig_Click(object sender, EventArgs e)
        {
            dockPanel.SuspendLayout(true);

            if (dockPanel.DocumentStyle == DocumentStyle.SystemMdi)
            {
                foreach (Form form in MdiChildren)
                    form.Close();
            }

            m_editFormNPC.Show(dockPanel, DockState.Document);
            m_editFormQuest.Show(dockPanel, DockState.Document);
            m_editFormCondition.Show(dockPanel, DockState.Document);
            m_editFormItemCondition.Show(dockPanel, DockState.Document);
            m_editFormItem.Show(dockPanel, DockState.Document);
            m_editFormLooting.Show(dockPanel, DockState.Document);
            m_editFormDIalog.Show(dockPanel, DockState.Document);
            m_editFormString.Show(dockPanel, DockState.Document);
            m_OutputForm.Show(dockPanel, DockState.Document);

            dockPanel.ResumeLayout(true, true);
        }

        private void mnuLockDock_Click(object sender, EventArgs e)
        {
            dockPanel.AllowEndUserDocking = !dockPanel.AllowEndUserDocking;
            mnuLockDock.Checked = !dockPanel.AllowEndUserDocking;
        }

        private void tsbtnSaveAll_Click(object sender, EventArgs e)
        {
            SaveAll();
        }

        private void mnuMain_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void mnuko_KR_Click(object sender, EventArgs e)
        {
            if (mnuko_KR.Checked) return;

            mnuko_KR.Checked = true;
            mnuen_US.Checked = false;
            mnude_DE.Checked = false;

            mnuko_KR.CheckState = (mnuko_KR.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnuen_US.CheckState = (mnuen_US.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnude_DE.CheckState = (mnude_DE.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;

            SHGlobal.Locale.Type = SHLocaleType.KO_KR;

            RefreshData();
            InitEditForms();

            Global.Output("언어 변경함 : 한국(KO_KR)\n");
        }

        private void mnuen_US_Click(object sender, EventArgs e)
        {
            if (mnuen_US.Checked) return;

            mnuko_KR.Checked = false;
            mnuen_US.Checked = true;
            mnude_DE.Checked = false;

            mnuko_KR.CheckState = (mnuko_KR.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnuen_US.CheckState = (mnuen_US.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnude_DE.CheckState = (mnude_DE.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;

            SHGlobal.Locale.Type = SHLocaleType.EN_US;

            RefreshData();
            InitEditForms();

            Global.Output("언어 변경함 : 미국(EN_US)\n");
        }

        private void mnude_DE_Click(object sender, EventArgs e)
        {
            return;

            mnuko_KR.Checked = false;
            mnuen_US.Checked = false;
            mnude_DE.Checked = true;

            mnuko_KR.CheckState = (mnuko_KR.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnuen_US.CheckState = (mnuen_US.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;
            mnude_DE.CheckState = (mnude_DE.CheckState == CheckState.Checked) ? CheckState.Checked : CheckState.Unchecked;

            SHGlobal.Locale.Type = SHLocaleType.DE_DE;

            RefreshData();
            InitEditForms();
        }

        private void mnuHelp_Click(object sender, EventArgs e)
        {
        }

        private void mnuOption_Click(object sender, EventArgs e)
        {
            OptionForm dlg = new OptionForm();
            dlg.ShowDialog();
        }

        private void mnuReloadXml_Click(object sender, EventArgs e)
        {
            ReloadAllXmlFile();
        }

        public void ReloadAllXmlFile()
        {
            if (_Forms.IsExistModifiedFiles())
            {
                if (MessageBox.Show("저장하지 않은 항목이 있습니다. 정말 모든 XML을 다시 읽겠습니까?", Application.ProductName, MessageBoxButtons.YesNo) != DialogResult.Yes)
                {
                    return;
                }
            }

            RefreshData();

            try
            {
                SHGlobal.CheckFiles(null);
            }
            catch (Exception ex)
            {
                Global.OutputWarning(ex.Message);
            }

            InitEditForms();

            Global.Output("모든 XML 다시 읽기 완료\n");
        }

        private void mnuOpenBackupFolder_Click(object sender, EventArgs e)
        {
            Process.Start(@"Backup\");
        }

        private void mnuOpenServerSystemFolder_Click(object sender, EventArgs e)
        {
            Process.Start(SHGlobal.GetServerSystemFolder());
        }

        private void mnuOpenClientSystemFolder_Click(object sender, EventArgs e)
        {
            Process.Start(SHGlobal.GetClientSystemFolder());
        }
    }
}
