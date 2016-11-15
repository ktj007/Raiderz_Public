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
    public class LootReportExporter
    {
        private string ExportItem(SHXmlCore xmlCore)
        {
            string str = "";

            foreach (SHLoot loot in xmlCore.Lootings.dataList)
            {
                str += LOOT_SetString(loot, xmlCore);
                str += "\n";
            }

            return str;
        }

        private string ProcessLootSilver(SHLootSilver silver)
        {
            string str = "";

            if (silver == null) return str;

            str += "골드,";
            str += (silver.rate.ToString() + "%");
            str += ",";
            str += ((silver.minSpecified) ? silver.min.ToString() : "(미정의)");
            str += ",";
            str += ((silver.maxSpecified) ? silver.max.ToString() : "(미정의)");
            str += ",";

            return str;
        }

        private string ProcessLootItem(SHXmlCore xmlCore, SHLootItem item)
        {
            string str = "";

            if (item == null) return str;

            str = (xmlCore.Items.IsValid(item.id) && xmlCore.Items[item.id].Passed) ? xmlCore.GetSafeString(xmlCore.Items[item.id].name) : "미존재 아이템 (" + item.id.ToString() + ")";
            str += ",";
            str += (item.rate.ToString() + "%");
            str += ",";
            str += ((item.minSpecified) ? item.min.ToString() : "(미정의)");
            str += ",";
            str += ((item.maxSpecified) ? item.max.ToString() : "(미정의)");
            str += ",";

            return str;
        }

        private string LOOT_Item(SHLoot loot, SHXmlCore xmlCore)
        {
            string str = "";

            for (int i = 0; i < loot.dataList.Count; i++)
            {
                if (loot.dataList[i].GetType() == typeof(SHLootSilver))
                {
                    str += ProcessLootSilver(loot.dataList[i] as SHLootSilver);
                }
                else if (loot.dataList[i].GetType() == typeof(SHLootItem))
                {
                    str += ProcessLootItem(xmlCore, loot.dataList[i] as SHLootItem);
                }
            }

            return str;
        }

        private string LOOT_SetString(SHLoot loot, SHXmlCore xmlCore)
        {
            string str = "";

            loot.Compile();
            loot.Build(xmlCore);

            str = (loot.idSpecified) ? loot.id.ToString() : String.Empty;

            str += ",";

            str += ((loot.comment != null) ? loot.comment : String.Empty);

            str += ",";

            str += LOOT_Item(loot, xmlCore);

            return str;
        }

        public void Export(string strFileName, SHXmlCore xmlCore)
        {
            StreamWriter wr = null;
            try
            {
                wr = new StreamWriter(strFileName, false, System.Text.Encoding.Default);

                string strLine;

                strLine = "ID,NPC,";
                for (int i = 0; i < 11; i++)
                {
                    String str = String.Format("{0:d}-전리품,{0:D}-비율,{0:d}-최소량, {0:d}-최대량, ", i + 1, i + 1, i + 1, i + 1);
                    strLine += str;
                }

                strLine += "\n";
                strLine += ExportItem(xmlCore);

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