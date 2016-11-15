using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CombatMonitor.Util;
using System.Collections;

namespace CombatMonitor
{
    public partial class frmMain : Form
    {
        private string strConfigXMLFile = "config.xml";
        private ListViewColumnSorter listViewBasicSorter = new ListViewColumnSorter();
        private ListViewColumnSorter listViewAnalysisSorter = new ListViewColumnSorter();

        public frmMain()
        {
            Global.Init();

            InitializeComponent();

            listViewBasic.ListViewItemSorter = listViewBasicSorter;
            listViewAnalysis.ListViewItemSorter = listViewAnalysisSorter;
        }

        private void LoadConfigXML()
        {
            if (System.IO.File.Exists(strConfigXMLFile))
            {
                Types.Config config = Util.XML.Deserialize<Types.Config>(strConfigXMLFile);
                txtStartParam.Text = config.StartParams;
            }
        }

        private void SaveConfigXML()
        {
            Types.Config config = new Types.Config();
            config.StartParams = txtStartParam.Text;
            Util.XML.Serialize<Types.Config>(strConfigXMLFile, ref config);
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            comboStartType.Items.Add("field");
            comboStartType.SelectedIndex = 0;

            LoadConfigXML();
            MakeTreeViewItems();
        }

        private void MakeTreeViewItems()
        {
            treeView1.Nodes.Clear();
            TreeNode nodeRoot = treeView1.Nodes.Add("Actors");
            TreeNode nodePlayers = nodeRoot.Nodes.Add("Players");
            TreeNode nodeNPCs = nodeRoot.Nodes.Add("NPCs");

            foreach (Stats.PlayerElement each in Global.Storage_Player.dataList)
            {
                TreeNode node = 
                    nodePlayers.Nodes.Add(each.proto.Name + " (" + each.proto.UIID + ")");
                node.ToolTipText = each.proto.UIID.ToString();
                node.Nodes.Add("공격").ToolTipText = node.ToolTipText;
                node.Nodes.Add("방어").ToolTipText = node.ToolTipText;
            }

            foreach (Stats.NPCElement each in Global.Storage_NPC.dataList)
            {
                TreeNode node = 
                    nodeNPCs.Nodes.Add(each.proto.Name + " (" + each.proto.ID + ")");
                node.ToolTipText = each.proto.ID.ToString();
                node.Nodes.Add("공격").ToolTipText = node.ToolTipText;
                node.Nodes.Add("방어").ToolTipText = node.ToolTipText;
            }

            nodeRoot.Expand();
            nodePlayers.Expand();
            nodeNPCs.Expand();
            //treeView1.ExpandAll();
        }

        private void ResetInfomationView()
        {
            listViewAnalysis.Clear();
            listViewBasic.Clear();
            Global.listReportsAnlaysis.Clear();
            Global.listReportsBasic.Clear();
        }

        private void MakeInfomationView(Stats.ActorElement actorOwner)
        {
            if (actorOwner == null)
                return;

            Global.listReportsAnlaysis = actorOwner.ExtractReportAnalysis();
            Global.listReportsBasic = actorOwner.ExtractReportBasic();
            MakeInfomationView();
        }

        private void MakeInfomationView()
        {
            listViewAnalysis.Clear();
            listViewBasic.Clear();

            listViewAnalysis.Columns.Add("Name", 150, HorizontalAlignment.Center);
            listViewAnalysis.Columns.Add("Param1", 100, HorizontalAlignment.Left);
            listViewAnalysis.Columns.Add("Param2", 100, HorizontalAlignment.Left);

            listViewBasic.Columns.Add("Name", 150, HorizontalAlignment.Center);
            listViewBasic.Columns.Add("Param1", 100, HorizontalAlignment.Left);
            listViewBasic.Columns.Add("Param2", 100, HorizontalAlignment.Left);
                            
            Util.ListView.ReflectionStructToItems<Types.Report>(ref listViewAnalysis, ref Global.listReportsAnlaysis);
            Util.ListView.ReflectionStructToItems<Types.Report>(ref listViewBasic, ref Global.listReportsBasic);

            Global.listReportsAnlaysis.Clear();
            Global.listReportsBasic.Clear();
            Global.listReportsAnlaysisColumnName.Clear();

            // Sort
            listViewBasic.Sort();
            listViewAnalysis.Sort();
        }
         

        private void MakeTalentView(Stats.Talent stats)
        {
            listViewAnalysis.Clear();
            listViewBasic.Clear();

            ArrayList arrAnalysis = new ArrayList();
            foreach (Stats.TalentElement each in stats.talents)
            {
                Stats.TalentAnalysis a = new Stats.TalentAnalysis();
                a.ID = each.ID;
                a.Name = each.Name;
                a.Success = 0;
                a.Invalidation = 0;
                a.Critical = 0;
                if (each.HitSegmentQty > 0)
                {
                    a.Success = Global.ToPercent(each.Act <= 0 ? 0 : (float)each.HitSuccess / (float)each.HitSegmentQty / (float)each.Act);
                    a.Invalidation = Global.ToPercent(each.Act <= 0 ? 0 : (float)(each.Avoid + each.Guard + each.Immuned) / ((float)each.Act * (float)each.HitSegmentQty));
                    a.Critical = Global.ToPercent(each.CriHit <= 0 ? 0 : (float)each.Hit / (float)each.HitSegmentQty / (float)each.CriHit);
                }
                a.Failed = Global.ToPercent(each.Use <= 0 ? 0 : 1.0f - (float)each.Act / (float)each.Use);
                a.GuardQuality = Global.ToPercent(each.Guard <= 0 ? 0 : (float)(each.AbsoluteGuard + (float)each.PerfectGuard) / (float)each.Guard);
                a.Damage = each.Damage;
                arrAnalysis.Add(a);
            }

            // Analysis
            Util.ListView.ReflectionStructToColumn<Stats.TalentAnalysis>(ref listViewAnalysis);
            Util.ListView.ReflectionStructToItems<Stats.TalentAnalysis>(ref listViewAnalysis, ref arrAnalysis);

            // Basic
            Util.ListView.ReflectionStructToColumn<Stats.TalentElement>(ref listViewBasic);
            Util.ListView.ReflectionStructToItems<Stats.TalentElement>(ref listViewBasic, ref stats.talents);

            // Sort
            listViewBasic.Sort();
            listViewAnalysis.Sort();
        }

        private void BtnStart_Click(object sender, EventArgs e)
        {
            if (Global.IsRunning)
            {
                string strCmd = "";
                strCmd += "stop";
                OnStop();

                SendToServer(strCmd);
            }
            else
            {
                Global.IsPrepareMode = true;
                Global.ResetContents();
                MakeTreeViewItems();


                ResetInfomationView();

                string strCmd = "";
                strCmd += "start ";
                strCmd += comboStartType.SelectedItem.ToString();
                strCmd += " ";
                strCmd += txtStartParam.Text;

                SendToServer(strCmd);
                SaveConfigXML();
            }
        }

        private void OnStart()
        {
            Global.IsPrepareMode = false;
            BtnStart.Text = "중지(&S)";
            Global.IsRunning = true;
            Global.ElapsedTime = 0;
            lblElapsed.Text = "";
            lblElapsed.Visible = true;
            timerTick.Start();
            txtStartParam.Enabled = false;
            comboStartType.Enabled = false;
            MakeTreeViewItems();
        }

        private void OnStop()
        {
            BtnStart.Text = "시작(&S)";
            Global.IsRunning = false;
            lblElapsed.Visible = false;
            timerTick.Stop();
            txtStartParam.Enabled = true;
            comboStartType.Enabled = true;
        }

        private void timerTick_Tick(object sender, EventArgs e)
        {
            ++Global.ElapsedTime;
            lblElapsed.Text = string.Format("진행된 시간: {0}초", Global.ElapsedTime);

            if (Global.ViewInfo.IsLeaf)
            {
                Stats.ActorElement actorOwner = Global.FindActorElement(Global.ViewInfo.ActorID);
                MakeInfomationView(actorOwner);
            }

            foreach (Stats.ActorElement each in Global.Storage_NPC.dataList)
            {
                if (each.IsNowCombat)
                {
                    each.CombatTime += 1;
                }
            }
            foreach (Stats.ActorElement each in Global.Storage_Player.dataList)
            {
                if (each.IsNowCombat)
                {
                    each.CombatTime += 1;
                }
            }
        }

        private void treeViewSelectImpl(Stats.ActorElement element, string strSubType)
        {
            Global.ViewInfo.IsLeaf = false;
            if (strSubType == "공격")
            {
                Global.ViewInfo.IsAttack = true;
                MakeTalentView(element.talentOffense);
            }
            else if (strSubType == "방어")
            {
                Global.ViewInfo.IsAttack = false;
                MakeTalentView(element.talentDefense);
            }
            else
            {
                Global.ViewInfo.IsLeaf = true;
                MakeInfomationView(element);
            }
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Action == TreeViewAction.ByMouse)
            {
                string[] words = e.Node.FullPath.Split("\\,()".ToCharArray());
                if (words.Length <= 1)
                    return;

                Global.ViewInfo.ActorID = 0;
                Global.ViewInfo.IsList = false;
                Global.ViewInfo.IsLeaf = false;
                
                string strRoot = words[0];
                string strType = words[1];
                switch (strType)
                {
                    case "Players":
                        {
                            if (words.Length > 2)
                            {
                                int nID = Int32.Parse(e.Node.ToolTipText);
                                Global.ViewInfo.ActorID = nID;
                                Stats.PlayerElement element = Global.Storage_Player[nID];

                                treeViewSelectImpl(element, e.Node.Text);
                            }
                            else
                            {
                                Global.ViewInfo.IsNPC = false;
                                Global.ViewInfo.IsList = true;
                                Global.listReportsAnlaysis = Global.Storage_Player.ExtractReportAnalysis();
                                Global.listReportsAnlaysisColumnName.Add("이름");
                                Global.listReportsAnlaysisColumnName.Add("DPS");
                                Global.listReportsAnlaysisColumnName.Add("누적피해 (공격측)");
                                MakeInfomationView();
                            }
                        } break;
                    case "NPCs":
                        {
                            if (words.Length > 2)
                            {
                                int nID = Int32.Parse(e.Node.ToolTipText);
                                Global.ViewInfo.ActorID = nID;
                                Stats.NPCElement element = Global.Storage_NPC[nID];
                                
                                treeViewSelectImpl(element, e.Node.Text);
                            }
                            else
                            {
                                Global.ViewInfo.IsNPC = true;
                                Global.ViewInfo.IsList = true;
                                Global.listReportsAnlaysis = Global.Storage_NPC.ExtractReportAnalysis();
                                Global.listReportsAnlaysisColumnName.Add("이름");
                                Global.listReportsAnlaysisColumnName.Add("DPS");
                                Global.listReportsAnlaysisColumnName.Add("누적피해 (공격측)");
                                MakeInfomationView();
                            }
                        } break;
                }
            }
        }

        private void listViewAnalysis_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            Util.ListView.ColumnClickToSort(ref listViewBasic, ref listViewBasicSorter, e);
        }

        private void listViewBasic_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            Util.ListView.ColumnClickToSort(ref listViewAnalysis, ref listViewAnalysisSorter, e);
        }
    }
}
