using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;
using System.Reflection;


namespace Velixian
{
    public partial class EditForm_NPC : EditForm
    {
        internal SHXmlCore xmlCore;
        protected NPCListViewController m_ListViewController;

        //////////////////////////////////////////////////////////////////////////

        public class NPCListViewController : BaseListViewController
        {
            public NPCListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHNPC npc = (SHNPC)(selDataObject);

                string szLoot = "None";

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                npc.Compile();
                npc.Build(m_XmlCore);

                if (npc.Item_LootSpecified)
                {
                    szLoot = npc.Item_Loot.ToString();
                    if (m_XmlCore.Lootings.IsValid(npc.Item_Loot)) szLoot = m_XmlCore.Lootings[npc.Item_Loot].GetString(m_XmlCore);
                }

                lvi.Text = npc.id.ToString();
                lvi.SubItems.AddRange(new String[] { 
                (npc.Name != null) ? m_XmlCore.GetSafeString(npc.Name) : "", 
                (npc.Clan != null) ? m_XmlCore.GetSafeString(npc.Clan) : "", 
                npc.InteractCount.ToString(),
                szLoot
            });

                if (npc.Type != null)
                {
                    if (npc.Type == "monster") lvi.ImageIndex = 3; else if (npc.Type == "npc") lvi.ImageIndex = 5; else if (npc.Type == "object") lvi.ImageIndex = 8;
                }

                lvi.Tag = npc;
                lvi.ForeColor = (npc.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (npc.Passed) ? Color.White : Color.Yellow;
                if (npc.InteractCount == 0) lvi.ForeColor = Color.Gray;

            }

            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHNPC)) return 0;

                SHNPC selNPC = (SHNPC)listViewItemTag;

                if (selNPC != null)
                {
                    return selNPC.id;
                }
                return 0;
            }

            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHNPC npc = (SHNPC)(dataObject);

                if (strHead.ToLower() == "id") return ExistFilterString(strFilter, npc.id.ToString());
                if (strHead.ToLower() == "name" || strHead == "이름") return ExistFilterString(strFilter, npc.Name);
                if (strHead.ToLower() == "clan" || strHead == "소속") return ExistFilterString(strFilter, npc.Clan);

                if (ExistFilterString(strFilter, npc.id.ToString())) return true;
                if (ExistFilterString(strFilter, npc.Name)) return true;
                if (ExistFilterString(strFilter, npc.Clan)) return true;

                return false;
            }
        }

        public EditForm_NPC()
        {
            InitializeComponent();
            xmlCore = null;
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.NPC, lvNPCs, "NPC", xmlCore.NPCs.LoadingFailed, null, null, xmlCore.SaveNPCString);
            }

            if (wbNPCDetail.ObjectForScripting == null) wbNPCDetail.ObjectForScripting = mainForm;

            m_ListViewController = new NPCListViewController(xmlCore, xmlCore.NPCs, xmlCore.NPCs.dataList, lvNPCs, cbNPCFilter);

            lvNPCs.SuspendLayout();

            lvNPCs.Items.Clear();
            foreach (SHNPC npc in xmlCore.NPCs.dataList)
            {
                ListViewItem lvi = new ListViewItem();

                if ((!mainForm.mnuViewMonster.Checked && npc.Type == "monster") ||
                    (!mainForm.mnuViewNPC.Checked && npc.Type == "npc") ||
                    (!mainForm.mnuViewObject.Checked && npc.Type == "object")) continue;

                if ((mainForm.mnuViewHideNoninteract.Checked && npc.InteractCount < 1) ||
                    (mainForm.mnuViewHideNoLoot.Checked && !npc.Item_LootSpecified)) continue;

                m_ListViewController.SetListText(lvi, npc);
                lvNPCs.Items.Add(lvi);
            }
            lvNPCs.ResumeLayout();

            Assembly assem = Assembly.GetExecutingAssembly();
            AssemblyName assemName = assem.GetName();
            String version = Application.ProductVersion + " (rev. " + assemName.Version.Revision.ToString() + ")";

            wbNPCDetail.DocumentText = SoulHunt.Properties.Resources.xhtml_template.Replace("[[CONTENT_BODY]]", SoulHunt.Properties.Resources.WhatsNew.Replace("[[PRODUCT_VERSION]]", version));

        }

        private void lvNPCs_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvNPCs.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvNPCs.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHNPC))
                {
                    SHNPC npc = (SHNPC)lvi.Tag;
                    if (npc != null)
                    {
                        wbNPCDetail.DocumentText = npc.GetHTML(xmlCore);
                        Global._mainForm.SetStatusLabelText(npc.CompiledMessage);
                    }
                }
            }
            m_ListViewController.OnSelectedIndexChanged();
        }

        private void EditForm_NPC_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbNPCFilter.Focus();
            }

        }

        private void EditForm_NPC_Load(object sender, EventArgs e)
        {
            this.lvNPCs.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        private void lvNPCs_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "NPC");
        }

        private void cbNPCFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }

    }
}
