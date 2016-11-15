using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.IO;
using ColtViewer.Model.Types;
using System.Windows.Forms;
using AIMonitor.Model.Types;
using System.Drawing;


namespace ColtViewer
{
    public partial class frmMain
    {
        private Byte[] StringToUTF8ByteArray(String pXmlString)
        {
            UTF8Encoding encoding = new UTF8Encoding();
            Byte[] byteArray = encoding.GetBytes(pXmlString);
            return byteArray;
        }


        private T GetDeserialize<T>(string strText)
        {
            XmlSerializer xs = new XmlSerializer(typeof(T));
            MemoryStream memoryStream = new MemoryStream(StringToUTF8ByteArray(strText));
            XmlTextWriter xmlTextWriter = new XmlTextWriter(memoryStream, Encoding.UTF8);
            return (T)xs.Deserialize(memoryStream);
        }

        void ParseXML(string strText)
        {
            XmlParserContext context = new XmlParserContext(null, null, null, XmlSpace.None);
            XmlTextReader reader = new XmlTextReader(strText, XmlNodeType.Element, context);
            if (!reader.Read())
                return;
            
            if (reader.LocalName == "NPCAttr")
            {
                if (checkPauseNPCUpdate.Checked)
                    return;

                lblUpdateTime.Text = "last updated at " + DateTime.Now.ToString("hh:mm:ss");

                NPCAttr old_attr = npc_attr;
                npc_attr = GetDeserialize<NPCAttr>(strText);

                if (old_attr == null ||
                    old_attr.UIID != npc_attr.UIID)
                {
                    // NPC 변경됨
                    ClearListLog();
                }

                // 잡모드 창 활성화
//                listBJobQueue.Enabled = npc_attr.IsNowCombat == 1;
//                listMJobQueue.Enabled = npc_attr.IsNowCombat != 1;
                bool bCombat = npc_attr.IsNowCombat == 1;
                groupBoxB.Text = "BehaviorJob's Queue";
                groupBoxM.Text = "MissionJob's Queue";
                if (bCombat)
                {
                    groupBoxB.Text += " (활성화 됨)";
                    groupBoxM.Font = new Font(groupBoxM.Font, FontStyle.Regular);
                    groupBoxB.Font = new Font(groupBoxB.Font, FontStyle.Bold);
                }
                else
                {
                    groupBoxM.Text += " (활성화 됨)";
                    groupBoxB.Font = new Font(groupBoxB.Font, FontStyle.Regular);
                    groupBoxM.Font = new Font(groupBoxM.Font, FontStyle.Bold);
                }

                propertyGrid.SelectedObject = npc_attr;
            }
            else if (reader.LocalName == "BJobQueue")
            {
                if (checkPauseNPCUpdate.Checked)
                    return;

                listBJobQueue.Items.Clear();

                BJobQueue dataJobQueue = GetDeserialize<BJobQueue>(strText);
                if (dataJobQueue.arrayItem == null)
                    return;

                foreach (BJobQueue.Item each in dataJobQueue.arrayItem)
                {
                    ListViewItem item = new ListViewItem(each.JobName);
                    item.SubItems.Add(each.TaskName);
                    item.SubItems.Add(each.Param);
                    listBJobQueue.Items.Add(item);
                }
            }
            else if (reader.LocalName == "MJobQueue")
            {
                if (checkPauseNPCUpdate.Checked)
                    return;

                listMJobQueue.Items.Clear();

                MJobQueue dataJobQueue = GetDeserialize<MJobQueue>(strText);
                if (dataJobQueue.arrayItem == null)
                    return;

                foreach (MJobQueue.Item each in dataJobQueue.arrayItem)
                {
                    ListViewItem item = new ListViewItem(each.JobName);
                    item.SubItems.Add(each.Param);
                    listMJobQueue.Items.Add(item);
                }
            }
            else if (reader.LocalName == "SJobQueue")
            {
                if (checkPauseNPCUpdate.Checked)
                    return;

                listSJobQueue.Items.Clear();

                SJobQueue dataJobQueue = GetDeserialize<SJobQueue>(strText);
                if (dataJobQueue.arrayItem == null)
                    return;

                foreach (SJobQueue.Item each in dataJobQueue.arrayItem)
                {
                    ListViewItem item = new ListViewItem(each.JobName);
                    item.SubItems.Add(each.Param);
                    listSJobQueue.Items.Add(item);
                }
            }
            else if (reader.LocalName == "Log")
            {
                if (checkPauseNPCUpdate.Checked)
                    return;

                if (checkPauseLogUpdate.Checked)
                    return;
                                
                Log.Item dataItem = GetDeserialize<Log.Item>(strText);

                dataItem.Date = DateTime.Now.ToString("hh:mm:ss");

                ListViewItem item = new ListViewItem(dataItem.Date);
                item.SubItems.Add(dataItem.Type);
                item.SubItems.Add(dataItem.Text);

                switch (dataItem.Type)
                {
                    case "JobEnq":
                        if (!CheckShowJobLog.Checked)
                            return;
                        item.BackColor = Color.Gold;
                        break;
                    case "JobFin":
                        if (!CheckShowJobLog.Checked)
                            return;
                        item.BackColor = Color.Gold;
                        break;
                    case "JobEvent":
                        if (!CheckShowJobLog.Checked)
                            return;
                        item.BackColor = Color.Gold;
                        break;
                    case "Callback":
                        if (!checkCallback.Checked)
                            return;
                        item.BackColor = Color.YellowGreen;
                        break;
                    case "GLogD":
                        if (!checkShowGLog.Checked)
                            return;
                        item.BackColor = Color.GhostWhite;
                        item.ForeColor = Color.Gray;
                        break;
                    case "GLogI":
                        if (!checkShowGLog.Checked)
                            return;
                        item.BackColor = Color.GhostWhite;
                        item.ForeColor = Color.Black;
                        break;
                    case "GLogW":
                        if (!checkShowGLog.Checked)
                            return;
                        item.BackColor = Color.GhostWhite;
                        item.ForeColor = Color.Orange;
                        break;
                    case "GLogE":
                        if (!checkShowGLog.Checked)
                            return;
                        item.BackColor = Color.GhostWhite;
                        item.ForeColor = Color.Red;
                        break;
                    case "Event":
                        if (!checkEvent.Checked)
                           return;
                        item.BackColor = Color.SandyBrown;
                        break;
                }
                                
                listLog.Items.Add(item);

                listLog.EnsureVisible(listLog.Items.Count - 1);
            }

            reader.Close();

        }
    }
}
