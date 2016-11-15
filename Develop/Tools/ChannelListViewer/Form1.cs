using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ChannelListViewer
{
    public partial class Form1 : Form
    {
        private readonly string TITLE_FIND_FIELDLIST_XML_WND = "서버의 fieldlist.xml 파일을 선택해 주세요";
        private readonly string FILTER_FIND_FIELDLIST_XML = "fieldlist.xml|fieldlist*.xml";

        private readonly string TITLE_FIND_SERVERLIST_XML_WND = "마스터서버의 serverlist.xml 파일을 선택해 주세요";
        private readonly string FILTER_FIND_SERVERLIST_XML = "serverlist.xml|serverlist.xml";

        private FieldChannelMgr m_FieldChannelMgr = null;

        public Form1()
        {
            InitializeComponent();

            m_FieldChannelMgr = new FieldChannelMgr();
        }

        private void btnFieldListXML_Click(object sender, EventArgs e)
        {
            openFileDialog_fieldlistXML.Title = TITLE_FIND_FIELDLIST_XML_WND;
            openFileDialog_fieldlistXML.Filter = FILTER_FIND_FIELDLIST_XML;
            openFileDialog_fieldlistXML.InitialDirectory = System.Windows.Forms.Application.StartupPath;

            if (openFileDialog_fieldlistXML.ShowDialog() == DialogResult.OK)
            {
                System.Environment.CurrentDirectory = Application.StartupPath;

                System.Uri uriAbsolutePath = new Uri(openFileDialog_fieldlistXML.FileName);
                System.Uri uriCurrentPath = new Uri(Application.StartupPath + "\\");

                System.Uri uriRelativePath = uriCurrentPath.MakeRelativeUri(uriAbsolutePath);

                textboxFieldListXML.Text = uriRelativePath.ToString();
                textboxServerListXML.Text = "";
                
                m_FieldChannelMgr.LoadFieldList(textboxFieldListXML.Text);

                BuildFieldList();
            }
        }

        private void btnServerListXML_Click(object sender, EventArgs e)
        {
            if (!m_FieldChannelMgr.IsLoadedFieldList)
            {
                MessageBox.Show("fieldlist.xml 을 먼저 선택하십시오!!");
                return;
            }

            openFileDialog_serverlistXML.Title = TITLE_FIND_SERVERLIST_XML_WND;
            openFileDialog_serverlistXML.Filter = FILTER_FIND_SERVERLIST_XML;
            openFileDialog_serverlistXML.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            
            if (openFileDialog_serverlistXML.ShowDialog()  == DialogResult.OK)
            {
                System.Environment.CurrentDirectory = Application.StartupPath;

                System.Uri uriAbsolutePath = new Uri(openFileDialog_serverlistXML.FileName);
                System.Uri uriCurrentPath = new Uri(Application.StartupPath + "\\");

                System.Uri uriRelativePath = uriCurrentPath.MakeRelativeUri(uriAbsolutePath);

                textboxServerListXML.Text = uriRelativePath.ToString();

                
                m_FieldChannelMgr.LoadServerList(textboxServerListXML.Text);

                BuildFieldList();
            }
        }

        


        private void BuildFieldList()
        {
            listviewFieldList.Items.Clear();
            listviewChannel.Items.Clear();

            LinkedListNode<FieldInfo> node = m_FieldChannelMgr.m_listFieldInfo.First;
            for (; node != null; node = node.Next)
            {
                FieldInfo refFieldInfo = node.Value;

                ListViewItem lviewItem = new ListViewItem();
                for (int i = 0; i < 3; ++i)
                    lviewItem.SubItems.Add("");

                int index = 0;
                lviewItem.SubItems[index++].Text = refFieldInfo.nFieldID.ToString();
                lviewItem.SubItems[index++].Text = refFieldInfo.strName;
                lviewItem.SubItems[index++].Text = refFieldInfo.nDefaultChannel.ToString() +"/"+ refFieldInfo.GetStaticChannelCount().ToString() +"/"+ refFieldInfo.GetReservedChannelCount().ToString();

                listviewFieldList.Items.Add(lviewItem);
            }
        }
        

        private void listviewFieldList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_FieldChannelMgr.IsLoadedServerList)
            {
                MessageBox.Show("serverlist.xml 을 먼저 선택하십시오!!");
                return;
            }

            RefreshChannelList();
        }

        void RefreshChannelList()
        {
            if (checkboxSortByServer.Checked)
            {
                BuildChannelList();
                listviewChannel.Sorting = System.Windows.Forms.SortOrder.Ascending;
                listviewChannel.Sorting = System.Windows.Forms.SortOrder.None;
                listviewChannel.Sorting = System.Windows.Forms.SortOrder.Ascending;
            }
            else
            {
                listviewChannel.Sorting = System.Windows.Forms.SortOrder.None;
                BuildChannelList();
            }
        }
        void BuildChannelList()
        {
            if (listviewFieldList.SelectedItems.Count == 0)
                return;

            int nFieldID = int.Parse(listviewFieldList.SelectedItems[listviewFieldList.SelectedItems.Count - 1].SubItems[0].Text);

            listviewChannel.Items.Clear();

            LinkedListNode<FieldInfo> node = m_FieldChannelMgr.m_listFieldInfo.First;
            for (; node != null; node = node.Next)
            {
                FieldInfo refFieldInfo = node.Value;
                if (refFieldInfo.nFieldID != nFieldID)
                    continue;


                for (int i = 0; i < refFieldInfo.listChannelInfo.Count; ++i)
                {
                    ChannelInfo refChannelInfo = refFieldInfo.listChannelInfo.Values[i];

                    ListViewItem lviewItem = new ListViewItem();
                    for (int j = 0; j < 3; ++j)
                        lviewItem.SubItems.Add("");

                    int index = 0;
                    lviewItem.SubItems[index++].Text = refChannelInfo.nGameServerID.ToString();
                    lviewItem.SubItems[index++].Text = refChannelInfo.nChannelID.ToString();
                    lviewItem.SubItems[index++].Text = refChannelInfo.bReserved ? "reserved" : "STATIC";

                    listviewChannel.Items.Add(lviewItem);
                }
            }
        }

        private void checkboxSortByServer_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_FieldChannelMgr.IsLoadedFieldList)
            {
                MessageBox.Show("fieldlist.xml 을 먼저 선택하십시오!!");
                return;
            }

            if (!m_FieldChannelMgr.IsLoadedServerList)
            {
                MessageBox.Show("serverlist.xml 을 먼저 선택하십시오!!");
                return;
            }

            RefreshChannelList();
        }

        private void btnRefreshFieldList_Click(object sender, EventArgs e)
        {
            if (textboxFieldListXML.Text != "")
                m_FieldChannelMgr.LoadFieldList(textboxFieldListXML.Text);

            if (textboxServerListXML.Text != "")
                m_FieldChannelMgr.LoadServerList(textboxServerListXML.Text);

            BuildFieldList();
        }

        private void btnRefreshServerList_Click(object sender, EventArgs e)
        {
            if (textboxServerListXML.Text != "")
                m_FieldChannelMgr.LoadServerList(textboxServerListXML.Text);

            BuildFieldList();
        }

    }
}
