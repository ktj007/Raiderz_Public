using System;
using System.Windows.Forms;
using System.Configuration;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.ComponentModel;

namespace SoulHunt
{
    public class NPCShopReportExporter
    {
        private string ExportNPCShop(SHXmlCore xmlCore)
        {
            string str = "";

            foreach (SHNPCShop npcShop in xmlCore.NPCShop.dataList)
            {
                str += NPCSHOP_SetString(npcShop, xmlCore);
                str += ",";
                str += "\n";
            }

            return str;
        }

        private string GetItemName(SHXmlCore xmlCore, int nItemID)
        {
            string str = "";

            if (xmlCore.Items.IsValid(nItemID))
            {
                return xmlCore.GetSafeString(xmlCore.Items[nItemID].name);
            }

            return str;
        }

        private string GetNPCName(SHXmlCore xmlCore, int nNPCShopID)
        {
            string str = "";

            foreach (SHNPC npc in xmlCore.NPCs.dataList)
            {
                for (int i = 0; i < 10; i++)
                {
                    if (npc.GetIType()[i] == SHNPCIType.trade)
                    {
                        if (npc.GetIAct()[i] == nNPCShopID.ToString())
                        {
                            str += npc.Name;
                            str += "(";
                            str += npc.id.ToString();
                            str += ")";
                            str += "\n";
                        }
                    }
                }
            }

            if (str == "")
            {
                str = "<사용안함>\n";
            }

            return str;
        }

        private string ProcessItem(SHXmlCore xmlCore, SHItemString item)
        {
            string str = "";

            if (item == null) return str;

            for (int i = 0; i < item.ItemIDList.Count; i++)
            {
                int nItemID = (int)(item.ItemIDList[i]);

                str += ",";
                str += GetItemName(xmlCore, nItemID);
                str += String.Format(", {0:s}\n", (item.ItemIDList[i].ToString()));
            }

            return str;
        }

        private string NPCSHOP_Item(SHNPCShop npcShop, SHXmlCore xmlCore)
        {
            string str = "";

            for (int i = 0; i < npcShop.dataList.Count; i++)
            {
                if (npcShop.dataList[i].GetType() == typeof(SHItemString))
                {
                    str += ProcessItem(xmlCore, npcShop.dataList[i] as SHItemString);
                }
            }

            return str;
        }

        private string NPCSHOP_SetString(SHNPCShop npcShop, SHXmlCore xmlCore)
        {
            string str = "";

            npcShop.Compile();
            npcShop.Build(xmlCore);

            if (npcShop.idSpecified)
            {
                str += GetNPCName(xmlCore, npcShop.id);
            }

            //str = (npcShop.idSpecified) ? npcShop.id.ToString() : String.Empty;

            str += "\n";

            str += NPCSHOP_Item(npcShop, xmlCore);

            return str;
        }

        public void Export(string strFileName, SHXmlCore xmlCore)
        {
            StreamWriter wr = null;
            try
            {
                wr = new StreamWriter(strFileName, false, System.Text.Encoding.Default);

                string strLine = "NPC, Item, ItemID\n";
                strLine += ExportNPCShop(xmlCore);

                wr.WriteLine(strLine);
            }
            finally
            {
                if (wr != null)
                {
                    wr.Close();
                }
            }

        }
    }
}