using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using CombatMonitor.Types;

namespace CombatMonitor
{
    class Global
    {
        static public string ServerPath = "";
        static public bool IsRunning = false;
        static public int ElapsedTime = 0;
        static public Stats.NPC Storage_NPC = new Stats.NPC();
        static public Stats.Player Storage_Player = new Stats.Player();
        static public ArrayList listReportsAnlaysis = new ArrayList();
        static public ArrayList listReportsBasic = new ArrayList();
        static public ArrayList listReportsAnlaysisColumnName = new ArrayList();
        static public bool IsPrepareMode = false;
        static public XML.XMLTalent Storage_TalentInfo = new XML.XMLTalent();
        static public XML.XMLTalentHit Storage_TalentHitInfo = new XML.XMLTalentHit();
        static public XML.XMLItem Storage_ItemInfo = new XML.XMLItem();
        

        public struct ViewInfo_t
        {
            public int ActorID;
            public bool IsAttack;
            public bool IsLeaf;
            public bool IsNPC;
            public bool IsList;
        };
        static public ViewInfo_t ViewInfo = new ViewInfo_t();

        static public void ResetContents()
        {
            Storage_NPC.Clear();
            Storage_Player.Clear();
        }

        static public ArrayList ExtractToReport<STRUCT_TYPE>(STRUCT_TYPE s, ref ArrayList arr)
        {
            Type t = s.GetType();
            System.Reflection.FieldInfo[] fields = t.GetFields();
            foreach (System.Reflection.FieldInfo field in fields)
            {
                object value = field.GetValue(s);
                string strText = value != null ? value.ToString() : "";
                arr.Add(new Report(field.Name, strText, ""));
            }
            return arr;
        }

        static public Stats.ActorElement FindActorElement(int uiid)
        {
            if (Storage_NPC.Has(uiid))
            {
                return Storage_NPC[uiid];
            }
            else if (Storage_Player.Has(uiid))
            {
                return Storage_Player[uiid];
            }

            return null;
        }

        static public float ToPercent(float a)
        {
            a *= 100;
            return (float)Math.Round((double)a);
        }

        static public void Init()
        {
            Storage_TalentInfo = Util.XML.Deserialize<XML.XMLTalent>("../GameServer/System/talent.xml");
            Storage_TalentInfo.Cooking();

            Storage_TalentHitInfo = Util.XML.Deserialize<XML.XMLTalentHit>("../GameServer/System/talent_hit_info.xml");
            Storage_TalentHitInfo.Cooking();

            Storage_ItemInfo = Util.XML.Deserialize<XML.XMLItem>("../GameServer/System/xitem.xml");
            Storage_ItemInfo.Cooking();
        }
    }
}
