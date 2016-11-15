using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using CombatMonitor.Types;


namespace CombatMonitor
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

        void SetTypeFieldValue(object obj, string strName, int adjust)
        {
            Type t = obj.GetType();
            //System.Reflection.FieldInfo field = t.GetField(strName);
            //int value = (int)field.GetValue(obj);
            //value += adjust;
            //field.SetValue(obj, value);

            System.Reflection.FieldInfo[] fields = t.GetFields();
            foreach (System.Reflection.FieldInfo field in fields)
            {
                object value = field.GetValue(obj);
                if (field.Name == strName)
                {
                    value = (int)value+adjust;
                }
                field.SetValue(obj, value);
            }
        }

        void ParseStats(string strName, Stats.ActorElement actorOwner, int uidAttacker, int TalentID, int adjust)
        {
            if (actorOwner != null)
            {
                Stats.TalentElement talentDefense = actorOwner.talentDefense.Get(TalentID);
                SetTypeFieldValue(talentDefense, strName, adjust);
            }
            
            Stats.ActorElement actorAttacker = Global.FindActorElement(uidAttacker);
            if (actorAttacker != null)
            {
                Stats.TalentElement talentAttacker = actorAttacker.talentOffense.Get(TalentID);
                SetTypeFieldValue(talentAttacker, strName, adjust);
            }
        }

        void ParseXML(string strText)
        {
            XmlParserContext context = new XmlParserContext(null, null, null, XmlSpace.None);
            XmlTextReader reader = new XmlTextReader(strText, XmlNodeType.Element, context);
            if (!reader.Read())
                return;

            switch (reader.LocalName)
            {
                case "Command":
                    Types.Command command;
                    command = GetDeserialize<Types.Command>(strText);
                    switch (command.Type)
                    {
                        case "start":
                            OnStart();
                            break;
                        case "stop":
                            OnStop();
                            break;
                    }
                    break;
                case "Player":
                    {
                        Types.Player xml_player = GetDeserialize<Types.Player>(strText);
                        Stats.PlayerElement e = new Stats.PlayerElement();
                        e.proto = xml_player;
                        if (Global.Storage_Player.Add(e.proto.UIID, e) &&
                            !Global.IsPrepareMode)
                        {
                            MakeTreeViewItems();
                        }
                    }
                    break;
                case "NPC":
                    {
                        NPC xml_npc = GetDeserialize<Types.NPC>(strText);
                        Stats.NPCElement e = new Stats.NPCElement();
                        e.proto = xml_npc;
                        if (Global.Storage_NPC.Add(e.proto.ID, e) &&
                            !Global.IsPrepareMode)
                        {
                            MakeTreeViewItems();
                        }
                    }
                    
                    break;
                case "Stats":
                    {
                        Types.Stats xml_stats = GetDeserialize<Types.Stats>(strText);
                        string strType = xml_stats.Type;

                        Stats.ActorElement actorOwner = Global.FindActorElement(xml_stats.Owner);
                        if (xml_stats.SourceType == "talent")
                        {
                            int TalentID = xml_stats.SourceID;
                            
                            switch (strType)
                            {
                                case "UseTalent":
                                    {
                                        Stats.TalentElement talentOwner = actorOwner.talentOffense.Get(TalentID);
                                        ++talentOwner.Use;
                                    }
                                    break;
                                case "ActTalent":
                                    {
                                        Stats.TalentElement talentOwner = actorOwner.talentOffense.Get(TalentID);
                                        ++talentOwner.Act;
                                    }
                                    break;
                                case "Damage":
                                    {
                                        Stats.ActorElement actorAttacker = Global.FindActorElement(xml_stats.Attacker);
                                        if (actorAttacker != null)
                                        {
                                            Stats.TalentElement talentAttacker = actorAttacker.talentOffense.Get(TalentID);
                                            talentAttacker.Damage += xml_stats.Damage;
                                            Stats.TalentElement talentDefense = actorOwner.talentDefense.Get(TalentID);
                                            talentDefense.Damage += xml_stats.Damage;
                                        }
                                    }
                                    break;
                                default:
                                    {
                                        ParseStats(strType, actorOwner, xml_stats.Attacker, TalentID, 1);
                                    }
                                    break;
                                    
                            }
                        }
                        else if (xml_stats.SourceType == "combat_time")
                        {
                            switch (strType)
                            {
                                case "Start":
                                    {
                                        if (actorOwner != null)
                                        {
                                            actorOwner.IsNowCombat = true;
                                        }
                                    }
                                    break;
                                case "End":
                                    {
                                        if (actorOwner != null)
                                        {
                                            actorOwner.IsNowCombat = false;
                                        }
                                    }
                                    break;
                            }
                        }

                        if (Global.ViewInfo.IsList)
                        {
                            if (Global.ViewInfo.IsNPC)
                            {
                                Global.listReportsAnlaysis = Global.Storage_NPC.ExtractReportAnalysis();
                            }
                            else
                            {
                                Global.listReportsAnlaysis = Global.Storage_Player.ExtractReportAnalysis();
                            }
                            MakeInfomationView();
                        }
                        else
                        {
                            if (!Global.ViewInfo.IsLeaf)
                            {
                                if (Global.ViewInfo.ActorID == xml_stats.Owner)
                                {
                                    if (actorOwner != null)
                                    {
                                        if (Global.ViewInfo.IsAttack)
                                        {
                                            MakeTalentView(actorOwner.talentOffense);
                                        }
                                        else
                                        {
                                            MakeTalentView(actorOwner.talentDefense);
                                        }
                                    }
                                }
                                else if (Global.ViewInfo.ActorID == xml_stats.Attacker)
                                {
                                    Stats.ActorElement actorAttacker = Global.FindActorElement(xml_stats.Attacker);
                                    if (actorAttacker != null)
                                    {
                                        if (Global.ViewInfo.IsAttack)
                                        {
                                            MakeTalentView(actorAttacker.talentOffense);
                                        }
                                        else
                                        {
                                            MakeTalentView(actorAttacker.talentDefense);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    break;
            }

            reader.Close();
        }
    }
}
