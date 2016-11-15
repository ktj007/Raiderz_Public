/*
 * SoulHunt.cs
 * SoulHunt와 관련된 각종 자료형들을 구현
 */
#region Using문

using System;
using System.ComponentModel;
using System.Linq;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace SoulHunt
{
    #region SHConditionsXml
    
    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHConditionsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHConditions), ElementName="CONDITIONS")]
        public ArrayList dataList;
        
        private Dictionary<int, SHConditions> Index;

        public override bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
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

        public SHConditions this[int nIndex]
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

        public SHConditions this[string szIndex]
        {
            get
            {
                try
                {
                    int nIndex = Convert.ToInt32(szIndex);
                    return Index[nIndex];
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }

        public SHConditionsXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHConditions>();
        }

        [XmlInclude(typeof(SHConditions))]
        public void Add(SHConditions i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHConditions conds in dataList)
            {
                Index[conds.id] = conds;
                if (!conds.Compile()) _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;
            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            foreach (SHConditions conds in dataList) if (!conds.Build(core)) _bPassed = false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        class ConditionsComparer : IComparer
        {
            public ConditionsComparer()
            {
            }

            public int Compare(object x, object y)
            {
                SHConditions qFirst = (SHConditions)x;
                SHConditions qSecond = (SHConditions)y;

                return qFirst.id.CompareTo(qSecond.id);
            }
        }

        public override void Sort()
        {
            dataList.Sort(new ConditionsComparer());
        }

    }

    [Serializable]
    public sealed class SHConditions : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("Make sure each condition's ID is Unique.(Velixian does not check this.)"), DisplayName("(ID)")]
        public int id
        {
            get { return _id; }
            set { idSpecified = (value != 0) ? true : false; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        private string _comment;
        [XmlAttribute, Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        [Category("Basic Info"), DescriptionAttribute("Developer Comments for this Condition."), DisplayName("(Comment)")]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        [XmlElement("AND")]
        public SHAnd and;

        [XmlElement("OR")]
        public SHOr or;

        [XmlElement(Type = typeof(SHCondition), ElementName = "CONDITION")]
        public ArrayList dataList;

        public SHConditions():base()
        {
            id = 0;
            comment = "";
            dataList = new ArrayList();
        }

        public SHConditions(int nID, string szComment, int nLineNumber):base()
        {
            id = nID;
            comment = szComment;
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHCondition))]
        public void Add(SHCondition newCond)
        {
            dataList.Add(newCond);
        }

        public void Remove(SHCondition cond)
        {
            dataList.Remove(cond);
        }

        public SHCondition this[int nIndex]
        {
            get
            {
                return (SHCondition)this.dataList[nIndex];
            }
        }

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            if (dataList.Count <= 0 && null == or && null == and)
            {
                return Error("Does not include any CONDITION, AND, OR");
            }

            if (null != and)
            {
                if (!and.Compile()) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Compile()) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Compile()) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            if (!_bPassed) return _bPassed;

            if (null != and)
            {
                if (!and.Build(core)) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Build(core)) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Build(core)) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public string GetString(SHXmlCore core)
        {
            string ret = id.ToString();            

            return ret;
        }

        public void Assign(SHConditions srcConds)
        {
            _comment = srcConds._comment;

            if (srcConds.and != null)
                this.and = (SHAnd)srcConds.and.Clone();
            else this.and = null;

            if (srcConds.or != null)
                this.or = (SHOr)srcConds.or.Clone();
            else this.or = null;


            dataList.Clear();

            foreach (SHCondition obj in srcConds.dataList)
            {
                dataList.Add((SHCondition)obj.Clone());
            }
        }
    }
    #endregion

    public sealed class SHAnd : SHDataObject
    {
        [XmlElement(Type = typeof(SHCondition), ElementName = "CONDITION")]
        public ArrayList dataList;

        [XmlElement("AND")]
        public SHAnd and;

        [XmlElement("OR")]
        public SHOr or;

        public SHAnd()
        {
            dataList = new ArrayList();
        }

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            if (dataList.Count <= 0 && null == or && null == and)
            {
                return Error("Does not include any CONDITION, AND, OR");
            }

            if (null != and)
            {
                if (!and.Compile()) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Compile()) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Compile()) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            if (!_bPassed) return _bPassed;

            if (null != and)
            {
                if (!and.Build(core)) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Build(core)) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Build(core)) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public string GetString(SHXmlCore core)
        {
            string ret = "AND";

            return ret;
        }

        public object Clone()
        {
            SHAnd clone = new SHAnd();


            if (this.and != null)
                clone.and = (SHAnd)this.and.Clone();
            else clone.and = null;

            if (this.or != null)
                clone.or = (SHOr)this.or.Clone();
            else clone.or = null;


            foreach (SHCondition obj in dataList)
            {
                clone.dataList.Add((SHCondition)obj.Clone());
            }

            return clone;
        }

    }

    public sealed class SHOr : SHDataObject
    {
        [XmlElement(Type = typeof(SHCondition), ElementName = "CONDITION")]
        public ArrayList dataList;

        [XmlElement("AND")]
        public SHAnd and;

        [XmlElement("OR")]
        public SHOr or;

        public SHOr()
        {
            dataList = new ArrayList();
        }

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            if (dataList.Count <= 0 && null == or && null == and)
            {
                return Error("Does not include any CONDITION, AND, OR.");
            }

            if (null != and)
            {
                if (!and.Compile()) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Compile()) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Compile()) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            if (!_bPassed) return _bPassed;

            if (null != and)
            {
                if (!and.Build(core)) return Error(and.CompiledMessage);
            }

            if (null != or)
            {
                if (!or.Build(core)) return Error(or.CompiledMessage);
            }

            foreach (SHCondition cond in dataList) if (!cond.Build(core)) return Error(cond.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public string GetString(SHXmlCore core)
        {
            string ret = "OR";

            return ret;
        }

        public object Clone()
        {
            SHOr clone = new SHOr();

            if (this.and != null)
                clone.and = (SHAnd)this.and.Clone();
            else clone.and = null;

            if (this.or != null)
                clone.or = (SHOr)this.or.Clone();
            else clone.or = null;

            foreach (SHCondition obj in dataList)
            {
                clone.dataList.Add((SHCondition)obj.Clone());
            }

            return clone;
        }
    }  


    [Serializable]
    public sealed class SHCondition : SHDataObject
    {
        [Serializable]
        //[TypeConverter(typeof(SHCT_CondType))]
        public enum ConditionType
        {
            level,
            quest,
            quest_var,
            quest_objective_complete,
            quest_objective_incomplete,
            item,
            noitem,
            manyitem,
            fewitem,
            equip,
            race,
            sex,
            faction,
            title,
            date,
            talent,
            talent_style_tp,
            inn,
            kill,
            qpvp_team,
        }

        public sealed class SHCTString
        {
            [XmlIgnore]
            public SHCondition.ConditionType Type;
            [XmlIgnore]
            public String Text;

            public SHCTString(SHCondition.ConditionType type, String text)
            {
                Type = type;
                Text = text;
            }
        }

        [XmlIgnore]
        public static SHCTString[] TypeNaming = new SHCTString[] {
            new SHCTString(SHCondition.ConditionType.level, "Level"),
            new SHCTString(SHCondition.ConditionType.quest, "Quest ID"),
            new SHCTString(SHCondition.ConditionType.quest_var, "Quest Var"),
            new SHCTString(SHCondition.ConditionType.quest_objective_complete, "Quest Obj. Complete"),
            new SHCTString(SHCondition.ConditionType.quest_objective_incomplete, "Quest Obj. Fail"),
            new SHCTString(SHCondition.ConditionType.item, "Item"),
            new SHCTString(SHCondition.ConditionType.noitem, "No Item"),
            new SHCTString(SHCondition.ConditionType.manyitem, "Many Items"),
            new SHCTString(SHCondition.ConditionType.fewitem, "Few Items"),
            new SHCTString(SHCondition.ConditionType.equip, "Equip"),
            new SHCTString(SHCondition.ConditionType.race, "Race"),
            new SHCTString(SHCondition.ConditionType.sex, "Sex"),
            new SHCTString(SHCondition.ConditionType.faction, "Faction"),
            new SHCTString(SHCondition.ConditionType.title, "Title"),
            new SHCTString(SHCondition.ConditionType.date, "Limited Time(Event)"),
            new SHCTString(SHCondition.ConditionType.talent, "Talent"),
            new SHCTString(SHCondition.ConditionType.talent_style_tp, "Talent TP"),
            new SHCTString(SHCondition.ConditionType.inn, "Inn"),
            new SHCTString(SHCondition.ConditionType.kill, "Kill"),
            new SHCTString(SHCondition.ConditionType.qpvp_team, "QPVP Team"),
        };

        private SHCondition.ConditionType _type;
        [XmlAttribute]
        [Category("Basic"), DescriptionAttribute("Define the kinds of conditions. Param1 and param2, depending on the type of conditions will be interpreted differently. For more information, please refer to the MDN."), DisplayName("Type")]
        //[TypeConverter(typeof(SHCT_CondType))]
        public SHCondition.ConditionType type
        {
            get { return _type; }
            set { typeSpecified = true; _type = value; }
        }
        [XmlIgnore]
        public bool typeSpecified;

        private string _param1
        {
            get { return _param1; }
            set { _param2 = (value != String.Empty) ? value : null; }
        }
        [XmlAttribute]
        [Category("Basic"), DescriptionAttribute("Conditional statement to set the parameters for the type. This value depends on the type of conditions. For more information, please refer to the MDN."), DisplayName("Param 1")]
        public string param1 { get; set; }

        private string _param2;
        [XmlAttribute]
        [Category("Basic"), DescriptionAttribute("Conditional statement to set the parameters for the type. This value depends on the type of conditions. For more information, please refer to the MDN."), DisplayName("Param 2")]
        public string param2
        {
            get { return _param2; }
            set { _param2 = (value != String.Empty) ? value : null; }
        }

        [XmlElement("AND")]
        public SHAnd and;

        [XmlElement("OR")]
        public SHOr or;

        public object Clone()
        {
            SHCondition clone = new SHCondition();

            clone._type = this._type;
            clone.typeSpecified = this.typeSpecified;

            clone.param1 = this.param1;
            clone._param2 = this._param2;

            if (this.and != null)
                clone.and = (SHAnd)this.and.Clone();
            else clone.and = null;

            if (this.or != null)
                clone.or = (SHOr)this.or.Clone();
            else clone.or = null;

            return clone;
        }

        #region GetString
        public string GetString(SHXmlCore core)
        {
            string ret = "";

            if (!_bPassed) return _szCompiled;

            try
            {
                switch (type)
                {
                    case SHCondition.ConditionType.level:
                        if (param1 != null)
                        {
                            ret = "Level " + param1 + " More Than";
                            if (param2 != null) ret += "More Than, ";
                        }
                        if (param2 != null)
                        {
                            ret += "Level" + param2 + "Less than";
                        }
                        break;
                    case SHCondition.ConditionType.quest:
                        ret += "[" + core.GetSafeString(core.Quests[param1].title) + "] Quest";

                        if (param2 == "notexist")
                        {
                            ret += "Does not have";
                        }
                        else if (param2 == "exist")
                        {
                            ret += "Exists";
                        }
                        else if (param2 == "complete")
                        {
                            ret += "Completeable";
                        }
                        else if (param2 == "notcomplete")
                        {
                            ret += "Not Completeable ";
                        }
                        else if (param2 == "done")
                        {
                            ret += "Complete";
                        }
                        else if (param2 == "notdone")
                        {
                            ret += "Incomplete";
                        }
                        else if (param2 == "active")
                        {
                            ret += "The (Challenge Field) Activated";
                        }
                        break;
                    case SHCondition.ConditionType.quest_var:
                        ret += "[" + core.GetSafeString(core.Quests[param1].title) + "] Quest Variables " + param2;
                        break;
                    case SHCondition.ConditionType.quest_objective_complete:
                        ret += "[" + core.GetSafeString(core.Quests[param1].title) + "] Quest " + param2 + "Step is complete";
                        break;
                    case SHCondition.ConditionType.quest_objective_incomplete:
                        ret += "[" + core.GetSafeString(core.Quests[param1].title) + "] Quest " + param2 + "Step is incomplete";
                        break;
                    case SHCondition.ConditionType.item:
                    case SHCondition.ConditionType.noitem:
                    case SHCondition.ConditionType.manyitem:
                    case SHCondition.ConditionType.fewitem:
                    case SHCondition.ConditionType.equip:
                        string[] itemsID = param1.Split(new Char[] { ',' });
                        for (int i = 0; i < itemsID.Length; i++)
                        {
                            string item = itemsID[i];

                            item.Trim();
                            if (item != "")
                            {
                                ret += "[" + core.GetSafeString(core.Items[param1].name) + "]";
                                if (i != itemsID.Length - 1) ret += ", ";
                            }
                        }
                        if (type == SHCondition.ConditionType.item) ret += " Must Have Item(s)";
                        else if (type == SHCondition.ConditionType.noitem) ret += " Has No Items";
                        else if (type == SHCondition.ConditionType.manyitem) ret += param2 + " Must Have Lots of Items";
                        else if (type == SHCondition.ConditionType.fewitem) ret += param2 + "Must Have A Few Items";
                        else if (type == SHCondition.ConditionType.equip) ret += " Requires Equip";

                        break;
                    case SHCondition.ConditionType.race:
                        if (param1 == "HUMAN") ret += "Must be Human"; else if (param1 == "VOLO") ret += "Must be Volvo"; else if (param1 == "NEBIT") ret += "Must be Nebit";
                        break;
                    case SHCondition.ConditionType.sex:
                        if (param1 == "MALE") ret += "Must be Male"; else if (param1 == "FEMALE") ret += "Must be Female";
                        break;
                    case SHCondition.ConditionType.faction:
                        ret += param1 + "One Faction " + param2 + " More than";
                        break;
                    case SHCondition.ConditionType.title:
                        string[] titlesID = param1.Split(new Char[] { ',' });
                        for (int i = 0; i < titlesID.Length; i++)
                        {
                            string titleID = titlesID[i];
                            titleID = titleID.Trim();
                            if (titleID != "")
                            {
                                ret += "[" + titleID + "]";
                                if (i != titlesID.Length - 1) ret += ", ";
                            }
                        }
                        ret += " Use the title(?)";
                        break;
                    case SHCondition.ConditionType.date:
                        if (param1 != null)
                        {
                            ret += param1 + "Time Elapsed";
                            if (param2 != null) ret += " And ";
                        }
                        if (param2 != null) ret += param2 + "Transfer";
                        break;
                    case SHCondition.ConditionType.talent:
                        string[] talentsID = param1.Split(new Char[] { ',' });
                        for (int i = 0; i < talentsID.Length; i++)
                        {
                            string talentID = talentsID[i];
                            talentID = talentID.Trim();
                            if (talentID != "")
                            {
                                ret += "[" + core.GetSafeString(core.Talents[talentID].name) + "]";
                                if (i != talentsID.Length - 1) ret += ", ";
                            }
                        }
                        ret += " No Talent";
                        break;
                    case SHCondition.ConditionType.talent_style_tp:
                        ret += "[" + param1 + "] Requires Talent Style " + param2;// +" Requires";
                        break;
                    case SHCondition.ConditionType.inn:
                        ret += "Inn " + param1 + "Regist Time";
                        break;
                    case SHCondition.ConditionType.kill:
                        ret += core.GetSafeString(core.NPCs[param1].Name) + "Kill NPC";
                        break;
                    case SHCondition.ConditionType.qpvp_team:
                        ret += "QPVP From" + param1 + "Time";
                        break;
                    default:
                        break;
                }
            }
            catch (KeyNotFoundException)
            {
                _bPassed = false;
                ret = _szCompiled = "[" + type.ToString().ToUpper() + "] Not found.";
            }
            catch (Exception ex)
            {
                _bPassed = false;
                ret = _szCompiled = "[" + type.ToString().ToUpper() + "] Fatal Error: " + ex.Message;
            }
            
            return ret;
        }
        #endregion

        #region Compilation

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            switch (type)
            {
                case SHCondition.ConditionType.level:
                    if (param1 == null && param2 == null) return Error("[" + type.ToString().ToUpper() + "] param1 param2 are missing.");
                    break;
                case SHCondition.ConditionType.quest:
                case SHCondition.ConditionType.quest_var:
                case SHCondition.ConditionType.faction:
                case SHCondition.ConditionType.date:
                case SHCondition.ConditionType.quest_objective_complete:
                case SHCondition.ConditionType.quest_objective_incomplete:
                case SHCondition.ConditionType.inn:
                case SHCondition.ConditionType.manyitem:
                case SHCondition.ConditionType.fewitem:
                case SHCondition.ConditionType.talent_style_tp:
                    if (param1 == null || param2 == null) return Error("[" + type.ToString().ToUpper() + "] param1 param2 are missing.");
                    if (type == SHCondition.ConditionType.quest && param2 != "notexist" && param2 != "exist" && param2 != "complete" && param2 != "notcomplete" && param2 != "active" && param2 != "done" && param2 != "notdone") return Error("[" + type.ToString().ToUpper() + "] Using wrong param2.");
                    if (type == SHCondition.ConditionType.date && ((param1.Length != 6 && param1.Length != 4) || (param2.Length != 6 && param2.Length != 4))) return Error("[" + type.ToString().ToUpper() + "] Date format is wrong.");
                    if (type == SHCondition.ConditionType.inn && param2 != "regist" && param2 != "notregist") return Error("[" + type.ToString().ToUpper() + "] Using the wrong param2.");
                    break;
                case SHCondition.ConditionType.item:
                case SHCondition.ConditionType.noitem:
                case SHCondition.ConditionType.equip:
                case SHCondition.ConditionType.race:
                case SHCondition.ConditionType.sex:
                case SHCondition.ConditionType.title:
                case SHCondition.ConditionType.talent:                
                case SHCondition.ConditionType.kill:
                case SHCondition.ConditionType.qpvp_team:
                    if (param1 == null || param2 != null) return Error("[" + type.ToString().ToUpper() + "] param1 param2 are missing.");
                    if ((type == SHCondition.ConditionType.race && (param1 != "HUMAN" && param1 != "NEBIT" && param1 == "VOLO")) ||
                        (type == SHCondition.ConditionType.sex && (param1 != "MALE" && param1 != "FEMALE"))) return Error("[" + type.ToString().ToUpper() + "] Value is outside max of param1");
                    break;
                default:
                    return Error("Bad type info.");
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            switch (type)
            {
                case SHCondition.ConditionType.quest:
                case SHCondition.ConditionType.quest_var:
                    if (!e.Quests.IsValid(param1)) _bPassed = false;
                    break;
                case SHCondition.ConditionType.quest_objective_complete:
                case SHCondition.ConditionType.quest_objective_incomplete:
                    if (!e.Quests.IsValid(param1)) _bPassed = false;
                    break;
                case SHCondition.ConditionType.item:
                case SHCondition.ConditionType.noitem:
                case SHCondition.ConditionType.equip:
                case SHCondition.ConditionType.manyitem:
                case SHCondition.ConditionType.fewitem:
                    string[] itemsID = param1.Split(new Char[] { ',' });
                    for (int i = 0; i < itemsID.Length; i++)
                    {
                        string szItem = itemsID[i].Trim();
                        if (szItem != String.Empty && !e.Items.IsValid(param1))
                        {
                            _bPassed = false;
                            break;
                        }
                    }
                    break;
                case SHCondition.ConditionType.talent:
                    string[] talentID = param1.Split(new Char[] { ',' });
                    for (int i = 0; i < talentID.Length; i++)
                    {
                        string szTalent = talentID[i].Trim();
                        if(szTalent != String.Empty && !e.Talents.IsValid(szTalent))
                        {
                            _bPassed = false;
                            break;
                        }
                    }
                    break;
            }

            if (!_bPassed) return Error("[" + type.ToString().ToUpper() + "] Error'd Fatally.");

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
