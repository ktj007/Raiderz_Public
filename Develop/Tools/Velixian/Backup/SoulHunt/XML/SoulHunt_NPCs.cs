using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

namespace SoulHunt
{
    #region SHNPCIType
    [Serializable]
    public enum SHNPCIType
    {
        [XmlEnum(Name = "NONE")]
        NONE,
        [XmlEnum(Name = "dialog")]
        dialog,
        [XmlEnum(Name = "trade")]
        trade,
        [XmlEnum(Name = "quest_begin")]
        quest_begin,
        [XmlEnum(Name = "quest_end")]
        quest_end,
        [XmlEnum(Name = "quest_count")]
        quest_count,
        [XmlEnum(Name = "quest_count_immo")]
        quest_count_immo,
        [XmlEnum(Name = "auction")]
        auction,
        [XmlEnum(Name = "mailbox")]
        mailbox,
        [XmlEnum(Name = "storage")]
        storage,
        [XmlEnum(Name = "loot")]
        loot,
        [XmlEnum(Name = "loot_immo")]
        loot_immo,

        [XmlEnum(Name = "loot_immortal")]
        loot_immortal,

        [XmlEnum(Name = "soldtrade")]
        soldtrade,
        [XmlEnum(Name = "craft")]
        craft,
        [XmlEnum(Name = "inn_regist")]
        inn_regist,
        [XmlEnum(Name = "inn_sleep")]
        inn_sleep,
        [XmlEnum(Name = "soulbinding")]
        soulbinding,
        [XmlEnum(Name = "gather_1")]
        gather_1,
        [XmlEnum(Name = "gather_2")]
        gather_2,
        [XmlEnum(Name = "gather_3")]
        gather_3,
        [XmlEnum(Name = "gather_4")]
        gather_4,
        [XmlEnum(Name = "gather_5")]
        gather_5,
        [XmlEnum(Name = "gather_6")]
        gather_6,
        [XmlEnum(Name = "gather_7")]
        gather_7,
        [XmlEnum(Name = "gather_8")]
        gather_8,
        [XmlEnum(Name = "gather_9")]
        gather_9,
        [XmlEnum(Name = "gather_10")]
        gather_10,
        [XmlEnum(Name = "challenger_quest")]
        challenger_quest,
        [XmlEnum(Name = "trigger")]
        trigger,
        [XmlEnum(Name = "switch")]
        trigger2,
        [XmlEnum(Name = "guild_create")]
        guild_create,
        [XmlEnum(Name = "guild_destroy")]
        guild_destroy,
        [XmlEnum(Name = "guild_storage")]
        guild_storage,
        
    }
    #endregion

    [Serializable]
    public sealed class SHNPC : SHDataObject
    {
        private SHNPCIType[] _iType;
        private int[] _iCond;
        private string[] _iAct;

        [XmlAttribute]
        public int id;
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        public string Name;

        [XmlAttribute]
        public string Type;

        [XmlAttribute]
        public int Grade;
        [XmlIgnore]
        public bool GradeSpecified;

        [XmlAttribute]
        public string Clan;

        [XmlAttribute]
        public string Interact;

        [XmlAttribute]
        public int MinLevel;
        [XmlIgnore]
        public bool MinLevelSpecified;

        [XmlAttribute]
        public int MaxLevel;
        [XmlIgnore]
        public bool MaxLevelSpecified;

        [XmlAttribute]
        public int Item_Loot;
        [XmlIgnore]
        public bool Item_LootSpecified;

        [XmlAttribute]
        public string validation;

        #region 상호작용1
        [XmlAttribute]
        public SHNPCIType IType1
        {
            get { return _iType[0]; }
            set { _iType[0] = value; }
        }
        [XmlAttribute]
        public int ICond1
        {
            get { return _iCond[0]; }
            set { _iCond[0] = value; }
        }
        [XmlAttribute]
        public string IAct1
        {
            get { return _iAct[0]; }
            set { _iAct[0] = value; }
        }
        #endregion

        #region 상호작용2
        [XmlAttribute]
        public SHNPCIType IType2
        {
            get { return _iType[1]; }
            set { _iType[1] = value; }
        }
        [XmlAttribute]
        public int ICond2
        {
            get { return _iCond[1]; }
            set { _iCond[1] = value; }
        }
        [XmlAttribute]
        public string IAct2
        {
            get { return _iAct[1]; }
            set { _iAct[1] = value; }
        }
        #endregion

        #region 상호작용3
        [XmlAttribute]
        public SHNPCIType IType3
        {
            get { return _iType[2]; }
            set { _iType[2] = value; }
        }
        [XmlAttribute]
        public int ICond3
        {
            get { return _iCond[2]; }
            set { _iCond[2] = value; }
        }
        [XmlAttribute]
        public string IAct3
        {
            get { return _iAct[2]; }
            set { _iAct[2] = value; }
        }
        #endregion

        #region 상호작용4
        [XmlAttribute]
        public SHNPCIType IType4
        {
            get { return _iType[3]; }
            set { _iType[3] = value; }
        }
        [XmlAttribute]
        public int ICond4
        {
            get { return _iCond[3]; }
            set { _iCond[3] = value; }
        }
        [XmlAttribute]
        public string IAct4
        {
            get { return _iAct[3]; }
            set { _iAct[3] = value; }
        }
        #endregion

        #region 상호작용5
        [XmlAttribute]
        public SHNPCIType IType5
        {
            get { return _iType[4]; }
            set { _iType[4] = value; }
        }
        [XmlAttribute]
        public int ICond5
        {
            get { return _iCond[4]; }
            set { _iCond[4] = value; }
        }
        [XmlAttribute]
        public string IAct5
        {
            get { return _iAct[4]; }
            set { _iAct[4] = value; }
        }
        #endregion

        #region 상호작용6
        [XmlAttribute]
        public SHNPCIType IType6
        {
            get { return _iType[5]; }
            set { _iType[5] = value; }
        }
        [XmlAttribute]
        public int ICond6
        {
            get { return _iCond[5]; }
            set { _iCond[5] = value; }
        }
        [XmlAttribute]
        public string IAct6
        {
            get { return _iAct[5]; }
            set { _iAct[5] = value; }
        }
        #endregion

        #region 상호작용7
        [XmlAttribute]
        public SHNPCIType IType7
        {
            get { return _iType[6]; }
            set { _iType[6] = value; }
        }
        [XmlAttribute]
        public int ICond7
        {
            get { return _iCond[6]; }
            set { _iCond[6] = value; }
        }
        [XmlAttribute]
        public string IAct7
        {
            get { return _iAct[6]; }
            set { _iAct[6] = value; }
        }
        #endregion

        #region 상호작용8
        [XmlAttribute]
        public SHNPCIType IType8
        {
            get { return _iType[7]; }
            set { _iType[7] = value; }
        }
        [XmlAttribute]
        public int ICond8
        {
            get { return _iCond[7]; }
            set { _iCond[7] = value; }
        }
        [XmlAttribute]
        public string IAct8
        {
            get { return _iAct[7]; }
            set { _iAct[7] = value; }
        }
        #endregion

        #region 상호작용9
        [XmlAttribute]
        public SHNPCIType IType9
        {
            get { return _iType[8]; }
            set { _iType[8] = value; }
        }
        [XmlAttribute]
        public int ICond9
        {
            get { return _iCond[8]; }
            set { _iCond[8] = value; }
        }
        [XmlAttribute]
        public string IAct9
        {
            get { return _iAct[8]; }
            set { _iAct[8] = value; }
        }
        #endregion

        #region 상호작용10
        [XmlAttribute]
        public SHNPCIType IType10
        {
            get { return _iType[9]; }
            set { _iType[9] = value; }
        }
        [XmlAttribute]
        public int ICond10
        {
            get { return _iCond[9]; }
            set { _iCond[9] = value; }
        }
        [XmlAttribute]
        public string IAct10
        {
            get { return _iAct[9]; }
            set { _iAct[9] = value; }
        }
        #endregion

        #region ICond 시리즈를 위한 XmlIgnore
        [XmlIgnoreAttribute]
        public bool IType1Specified;
        [XmlIgnoreAttribute]
        public bool IType2Specified;
        [XmlIgnoreAttribute]
        public bool IType3Specified;
        [XmlIgnoreAttribute]
        public bool IType4Specified;
        [XmlIgnoreAttribute]
        public bool IType5Specified;
        [XmlIgnoreAttribute]
        public bool IType6Specified;
        [XmlIgnoreAttribute]
        public bool IType7Specified;
        [XmlIgnoreAttribute]
        public bool IType8Specified;
        [XmlIgnoreAttribute]
        public bool IType9Specified;
        [XmlIgnoreAttribute]
        public bool IType10Specified;

        [XmlIgnoreAttribute]
        public bool ICond1Specified;
        [XmlIgnoreAttribute]
        public bool ICond2Specified;
        [XmlIgnoreAttribute]
        public bool ICond3Specified;
        [XmlIgnoreAttribute]
        public bool ICond4Specified;
        [XmlIgnoreAttribute]
        public bool ICond5Specified;
        [XmlIgnoreAttribute]
        public bool ICond6Specified;
        [XmlIgnoreAttribute]
        public bool ICond7Specified;
        [XmlIgnoreAttribute]
        public bool ICond8Specified;
        [XmlIgnoreAttribute]
        public bool ICond9Specified;
        [XmlIgnoreAttribute]
        public bool ICond10Specified;
        #endregion

        [XmlIgnoreAttribute]
        public int InteractCount;

        [XmlAnyAttribute]
        public XmlAttribute[] XAttributes;

        public SHNPC()
        {
            Interact = "False";
            validation = "True";

            _iType = new SHNPCIType[10];
            _iCond = new int[10];
            _iAct = new string[10];

            InteractCount = 0;
        }

        public bool Interactable()
        {
            return (Interact.ToUpper() == "FALSE") ? false : true;
        }

        public bool IsValidatable()
        {
            return (validation.ToUpper() == "FALSE") ? false : true;
        }

        public override bool Compile()
        {
            if (idSpecified && Name != null)
            {
                _bPassed = true;
                _szCompiled = Name;
            }
            else
            {
                return Error("이름이 없거나 아이디 미지정된 NPC");
            }
            
            InteractCount = 0;
            for (int i = 0; i < 10; i++)
            {
                if (_iType[i] != SHNPCIType.NONE && _iType[i] != SHNPCIType.mailbox)
                {
                    InteractCount++;
                    if (_iAct[i] == null) return Error(i.ToString() + "번째 상호작용 iAct가 기술되지 않았음.");
                }
            }
            
            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            for (int i = 0; i < 10; i++)
            {
                if (_iType[i] != SHNPCIType.NONE)
                {
                    // 컨디션 Validate
                    if (_iCond[i] != 0 && !e.Conditions.IsValid(_iCond[i])) return Error("생성되지 않거나 오류가 있는 조건을 참조중 (" + _iCond[i].ToString() + ")");

                    // 각 타입별로 인자가 제대로 들어가있는지 확인
                    if (_iAct[i] != null)
                    {
                        switch (_iType[i])
                        {
                            case SHNPCIType.quest_begin:
                            case SHNPCIType.quest_end:
                                if (!e.Quests.IsValid(_iAct[i])) return Error("생성되지 않거나 오류가 있는 퀘스트를 참조중 (" + _iAct[i] + ")");
                                break;
                            case SHNPCIType.dialog:
                                if (!e.Dialogs.IsValid(_iAct[i])) return Error("생성되지 않거나 오류가 있는 대화를 참조중 (" + _iAct[i] + ")");
                                break;
                            //아직 체크하지 않는 타입들
                            case SHNPCIType.quest_count:
                            case SHNPCIType.auction:
                            case SHNPCIType.craft:
                            case SHNPCIType.inn_sleep:
                            case SHNPCIType.inn_regist:
                            case SHNPCIType.loot:
                            case SHNPCIType.loot_immo:
                            case SHNPCIType.mailbox:
                            case SHNPCIType.soldtrade:
                            case SHNPCIType.storage:
                            case SHNPCIType.trade:
                            case SHNPCIType.trigger:
                                break;
                        }
                    }
                }
            }
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;
            String szClan;
            String szParsed;
            String szType = String.Empty;
            String szLevel = String.Empty;
            String szGrade = String.Empty;

            szClan = (Clan != null) ? "&lt;" + Clan + "&gt;" : String.Empty;
            szGrade = (GradeSpecified) ? Grade.ToString() : "명시되지 않음";

            if (Type != null)
            {
                if (Type == "monster") szType = "몬스터"; else if (Type == "object") szType = "오브젝트"; else if (Type == "npc") szType = "NPC"; else if (Type == "gather") szType = "채집";  else szType = "알수없는 타입";
            }

            if (MinLevelSpecified)
            {
                if (MaxLevelSpecified) szLevel = (MinLevel == MaxLevel) ? MinLevel.ToString() : MinLevel.ToString() + " ~ " + MaxLevel.ToString(); else szLevel = MaxLevel.ToString();
            }
            else
            {
                if (MaxLevelSpecified) szLevel = MaxLevel.ToString(); else szLevel = "없음";
            }

            if (Passed)
            {
                ret = Properties.Resources.npc_template.Replace("[[Name]]", Name);
                ret = ret.Replace("[[ID]]", id.ToString());
                ret = ret.Replace("[[Clan]]", szClan);
                ret = ret.Replace("[[Type]]", szType);
                ret = ret.Replace("[[Level]]", szLevel);
                ret = ret.Replace("[[Grade]]", szGrade);

                for (int i = 0; i < _iType.Length; i++)
                {
                    if (_iType[i] != SHNPCIType.NONE)
                    {
                        szParsed = Properties.Resources.npc_interaction_template.Replace("[[Interaction]]", GetInteractStringViaHtml(core, _iType[i], _iAct[i]));
                        if (_iCond[i] != 0) szParsed = szParsed.Replace("[[Condition]]", core.GetAnchorString(core.Conditions[_iCond[i]].GetString(core), "CONDITION", _iCond[i].ToString(), false)); else szParsed = szParsed.Replace("[[Condition]]", "없음");
                        szParsed = szParsed.Replace("[[Details]]", GetInteractDetailsStringViaHtml(core, _iType[i], _iAct[i]));

                        ret += szParsed;
                    }
                }

                if (Item_LootSpecified) ret += core.Lootings[Item_Loot].ToBody(core);
            }
            else
            {
                if (Name != null && idSpecified)
                {
                    // 에러메시지 출력
                    ret = Properties.Resources.error_template.Replace("[[Message]]", _szCompiled);

                    ret += Properties.Resources.npc_template.Replace("[[Name]]", Name);
                    ret = ret.Replace("[[ID]]", id.ToString());
                    ret = ret.Replace("[[Clan]]", szClan);
                    ret = ret.Replace("[[Type]]", szType);
                    ret = ret.Replace("[[Level]]", szLevel);
                    ret = ret.Replace("[[Grade]]", szGrade);
                }
                else
                {
                    ret = Properties.Resources.error_template.Replace("[[Message]]", _szCompiled);
                }
            }

            return ret;
        }

        // 퀘스트 타입을 HTML 문자열로 변환시킨다.
        private static string GetInteractStringViaHtml(SHXmlCore core, SHNPCIType iType, string szAct)
        {
            string ret = "없음";

            switch (iType)
            {
                case SHNPCIType.quest_begin:
                    ret = "[" + core.GetAnchorString(core.Quests[szAct].title, "quest", szAct, true) + "</a>] 퀘스트 시작.";
                    break;
                case SHNPCIType.quest_end: ret = "퀘스트 완료.";
                    ret = "[" + core.GetAnchorString(core.Quests[szAct].title, "quest", szAct, true) + "</a>] 퀘스트 완료.";
                    break;
                case SHNPCIType.dialog: ret = "[" + core.GetAnchorString(core.Dialogs[szAct].text, "dialog", szAct, true) + "(...)]" + " 대화를 시작함."; break;
                case SHNPCIType.auction: ret = "옥션창을 열어줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.craft: ret = "생산스킬 창을 열어줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.inn_sleep: ret = "휴식을 취하게 함. (여관) "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.inn_regist: ret = "여관을 등록시킴. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.loot: ret = "루팅창을 열어줌."; ret += "(" + szAct + ")"; break;
                case SHNPCIType.loot_immo: ret = "루팅창을 열어줌. 단, 루팅을 전부 다 해도 객체는 사라지지 않음. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.mailbox: ret = "우편함을 열어줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.soldtrade: ret = "플레이어가 판매했던 아이템 목록을 보여줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.storage: ret = "창고를 보여줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.trade: ret = "판매창을 열어줌. "; ret += "(" + szAct + ")"; break;
                case SHNPCIType.gather_1: ret = "티어1 채집하게 함."; break;
                case SHNPCIType.gather_2: ret = "티어2 채집하게 함."; break;
                case SHNPCIType.gather_3: ret = "티어3 채집하게 함."; break;
                case SHNPCIType.gather_4: ret = "티어4 채집하게 함."; break;
                case SHNPCIType.gather_5: ret = "티어5 채집하게 함."; break;
                case SHNPCIType.gather_6: ret = "티어6 채집하게 함."; break;
                case SHNPCIType.gather_7: ret = "티어7 채집하게 함."; break;
                case SHNPCIType.gather_8: ret = "티어8 채집하게 함."; break;
                case SHNPCIType.gather_9: ret = "티어9 채집하게 함."; break;
                case SHNPCIType.gather_10: ret = "티어10 채집하게 함."; break;
                case SHNPCIType.challenger_quest: ret = "도전자 퀘스트를 받게 함."; break;
                case SHNPCIType.trigger: ret = "트리거"; break;
            }

            return ret;
        }

        private static string GetInteractDetailsStringViaHtml(SHXmlCore core, SHNPCIType iType, string szAct)
        {
            string ret = "";

            switch (iType)
            {
                case SHNPCIType.quest_begin:
                    if (core.Quests[szAct].dialogSpecified) ret = "대화: " + core.GetAnchorString(core.Dialogs[core.Quests[szAct].dialog].text, "dialog", core.Quests[szAct].dialog.ToString(), true); else ret = "시작 다이얼로그가 기술되지 않았습니다.";
                    break;
                case SHNPCIType.quest_end:
                    if (core.Quests[szAct].Rewards != null && core.Quests[szAct].Rewards.dialogSpecified)
                    {
                        if (core.Dialogs.IsValid(core.Quests[szAct].Rewards.dialog) && core.Dialogs[core.Quests[szAct].Rewards.dialog].Passed)
                        {
                            ret = "대화: " + core.GetAnchorString(core.Dialogs[core.Quests[szAct].Rewards.dialog].text, "dialog",
                                core.Quests[szAct].Rewards.dialog.ToString(), true);
                        }
                        else
                        {
                            ret = "오류가 있는 대화를 참조중입니다. (" + core.Quests[szAct].Rewards.dialog + ")";
                        }
                    }
                    else
                    {
                        ret = "종료 다이얼로그가 기술되지 않았습니다.";
                    }
                    break;
                //case SHNPCIType.dialog: ret = "[" + core.GetAnchorString(core.Dialogs[szAct].text, "dialog", szAct, true) + "(...)]" + " 대화를 시작함"; break;
            }

            return ret;
        }

        public SHNPCIType[] GetIType()
        {
            return _iType;
        }

        public string[] GetIAct()
        {
            return _iAct;
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHNPCsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHNPC), ElementName="NPC")]
        public ArrayList dataList;

        private Dictionary<int, SHNPC> Index; 

        public SHNPC this[int nIndex]
        {
            get
            {
                try
                {
                    return Index[nIndex];
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }

        public SHNPC this[string szIndex]
        {
            get
            {
                try
                {
                    return Index[Convert.ToInt32(szIndex)];
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }

        private Dictionary<int, ArrayList> IndexByLootID;
        public ArrayList GetNPCIDListByLootID(int nLootID)
        {
            try
            {
                return IndexByLootID[nLootID];
            }
            catch
            {
                return null;
            }
        }

        private Dictionary<int, ArrayList> IndexByQuestID;
        public ArrayList GetNPCIDListByQuestID(int nQuestID)
        {
            try
            {
                return IndexByQuestID[nQuestID];
            }
            catch
            {
                return null;
            }
        }

        public override bool IsValid(String szIndex)
        {
            if (szIndex == null) return false;
            try
            {
                return Index.ContainsKey(Int32.Parse(szIndex));
            }
            catch (FormatException)
            {
                return false;
            }
        }

        public override bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
        }

        public SHNPCsXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHNPC>();
            IndexByLootID = new Dictionary<int, ArrayList>();
            IndexByQuestID = new Dictionary<int, ArrayList>();
        }

        [XmlInclude(typeof(SHNPC))]
        public void Add(SHNPC i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;
            _szCompiled = "완료";

            foreach (SHNPC npc in dataList)
            {
                if (!npc.Compile()) _bPassed = false;
                Index[npc.id] = npc;

                if (npc.IsValidatable() == true)
                    Complile_NPCIDListByLootID(npc.id, npc.Item_Loot);

                if (SHNPCIType.quest_end == npc.IType1)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct1));
                }
                if (SHNPCIType.quest_end == npc.IType2)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct2));
                }
                if (SHNPCIType.quest_end == npc.IType3)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct3));
                }
                if (SHNPCIType.quest_end == npc.IType4)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct4));
                }
                if (SHNPCIType.quest_end == npc.IType5)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct5));
                }
                if (SHNPCIType.quest_end == npc.IType6)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct6));
                }
                if (SHNPCIType.quest_end == npc.IType7)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct7));
                }
                if (SHNPCIType.quest_end == npc.IType8)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct8));
                }
                if (SHNPCIType.quest_end == npc.IType9)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct9));
                }
                if (SHNPCIType.quest_end == npc.IType10)
                {
                    Complile_NPCIDListByQuestID(npc.id, int.Parse(npc.IAct10));
                }
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        private void Complile_NPCIDListByLootID(int nNPCID, int nLootID)
        {
            ArrayList npcIDList = GetNPCIDListByLootID(nLootID);
            if (null == npcIDList)
            {
                ArrayList newNPCIDList = new ArrayList();
                newNPCIDList.Add(nNPCID);
                IndexByLootID[nLootID] = newNPCIDList;
            }
            else
            {
                npcIDList.Add(nNPCID);
                IndexByLootID[nLootID] = npcIDList;
            }
        }

        private void Complile_NPCIDListByQuestID(int nNPCID, int nQuestID)
        {
            ArrayList npcIDList = GetNPCIDListByQuestID(nQuestID);
            if (null == npcIDList)
            {
                ArrayList newNPCIDList = new ArrayList();
                newNPCIDList.Add(nNPCID);
                IndexByQuestID[nQuestID] = newNPCIDList;
            }
            else
            {
                npcIDList.Add(nNPCID);
                IndexByQuestID[nQuestID] = npcIDList;
            }
        }

        public override bool Build(SHXmlCore e)
        {
            foreach (SHNPC npc in dataList)
            {
                if (npc.Passed)
                {
                    if (!npc.Build(e))
                    {
                        _bPassed = false;
                    }
                }
            }
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return true;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}
