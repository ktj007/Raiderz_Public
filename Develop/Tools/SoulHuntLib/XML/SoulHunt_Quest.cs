#region Using문

using System;
using System.ComponentModel;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace SoulHunt {
    #region SHQuestsXml
    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHQuestsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHQuest), ElementName = "QUEST")]
        public ArrayList dataList;

        private Dictionary<int, SHQuest> Index;

        public SHQuest this[int nIndex]
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

        public SHQuest this[string szIndex]
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

        public SHQuestsXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHQuest>();
        }

        [XmlInclude(typeof(SHQuest))]
        public void Add(SHQuest i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            Sort();

            foreach (SHQuest quest in dataList)
            {
                if (!quest.Compile()) _bPassed = false;
                Index[quest.id] = quest;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        class QuestComparer : IComparer
        {
            public QuestComparer()
            {
            }

            public int Compare(object x, object y)
            {
                SHQuest qFirst = (SHQuest)x;
                SHQuest qSecond = (SHQuest)y;

                return qFirst.id.CompareTo(qSecond.id);
            }
        }

        public override void Sort()
        {
            dataList.Sort(new QuestComparer());
        }

    }
    #endregion

    #region SHQuest
    [Serializable]
    public sealed class SHQuest : SHDataObject
    {
        private int _id;
        private string _title;
        private string _desc;
        private string _category;
        private bool _repeatable;
        private int _dialog;
        private int _incomplete_dialog;
        private int _level;
        private int _timelimit;
        private int _condition;
        private string _destroy_item;
        private SHQuestShareType _share;
        private int _party_field;
        private int _party_sensor;
        private int _party_member_count;
        private int _cq_condition;
        private int _share_condition;
        private int _players;
        private SHQuestTrialType _trialtype;
        private int _field_group;
        private string _comment;
        private bool _validation;


        [XmlAttribute ("id")]
        [Category("기본"), DescriptionAttribute("퀘스트의 아이디값입니다. 이 값은 반드시 유일해야 합니다."), DisplayName("(아이디)")]
        public int id
        {
            get
            {
                return _id;
            }
            set
            {
                idSpecified = true;
                _id = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool idSpecified;


        [XmlElement("ITEM")]
        public SHItemAddRemove Items;

        [XmlElement("OBJECTIVES")]
        public SHQuestObjectives Objectives;

        [XmlElement("REWARDS")]
        public SHQuestRewards Rewards;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("퀘스트 제목을 설정합니다."), DisplayName("(이름)")]
        public string title
        {
            get { return _title; }
            set { _title = (value != String.Empty) ? value : null; }
        }

        

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("퀘스트 설명을 설정합니다."), DisplayName("desc"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string desc
        {
            get { return _desc; }
            set { _desc = value; }
        }

        

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("퀘스트 카테고리를 설정합니다."), DisplayName("category")]
        public string category
        {
            get { return _category; }
            set { _category = (value != String.Empty) ? value : null; }
        }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("반복 여부를 설정합니다."), DisplayName("repeatable")]
        public bool repeatable
        {
            get
            {
                return _repeatable;
            }
            set
            {
                repeatableSpecified = true;
                _repeatable = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool repeatableSpecified;


        [XmlAttribute]
        public int players
        {
            get
            {
                return _players;
            }
            set
            {
                playersSpecified = true;
                _players = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool playersSpecified;

        
        [XmlAttribute]
        public SHQuestShareType share
        {
            get 
            {
                return _share; 
            }
            set 
            {
                shareSpecified = true;
                _share = value; 
            }
        }

        [XmlIgnoreAttribute]
        public bool shareSpecified;



        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("대화를 설정합니다."), DisplayName("dialog")]
        public int dialog
        {
            get
            {
                return _dialog;
            }
            set
            {
                dialogSpecified = true;
                _dialog = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool dialogSpecified;

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("퀘스트를 아직 완료하지 못했을 때 사용하는 대화"), DisplayName("incomplete_dialog")]
        public int incomplete_dialog
        {
            get
            {
                return _incomplete_dialog;
            }
            set
            {
                if (value > 0)
                {
                    incomplete_dialogSpecified = true;
                    _incomplete_dialog = value;
                }
                else
                {
                    incomplete_dialogSpecified = false;
                    _incomplete_dialog = 0;
                }
            }
        }

        [XmlIgnoreAttribute]
        public bool incomplete_dialogSpecified;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("레벨을 설정합니다."), DisplayName("level")]
        public int level
        {
            get
            {
                return _level;
            }
            set
            {
                levelSpecified = true;
                _level = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool levelSpecified;

        [XmlAttribute]
        public int timelimit
        {
            get
            {
                return _timelimit;
            }
            set
            {
                timelimitSpecified = true;
                _timelimit = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool timelimitSpecified;


        [XmlAttribute]
        public int condition
        {
            get
            {
                return _condition;
            }
            set
            {
                conditionSpecified = true;
                _condition = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool conditionSpecified;

        [XmlAttribute]
        public SHQuestTrialType trialtype
        {
            get
            {
                return _trialtype;
            }
            set
            {
                if (value == SHQuestTrialType.none)
                {
                    trialtypeSpecified = false;
                    _trialtype = value;
                    return;
                }

                trialtypeSpecified = true;
                _trialtype = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool trialtypeSpecified;

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("트라이얼 퀘스트를 수행하는 필드 그룹"), DisplayName("field_group")]
        public int field_group
        {
            get
            {
                return _field_group;
            }
            set
            {
                field_groupSpecified = true;
                _field_group = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool field_groupSpecified;
        
        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("퀘스트 완료와 취소시 파괴되는 아이템"), DisplayName("destroy_item")]
        public string destroy_item
        {
            get { return _destroy_item; }
            set { _destroy_item = (value != String.Empty) ? value : null; }
        }

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("퀘스트 공유가 가능한지 체크하는 조건"), DisplayName("share_condition")]
        public int share_condition
        {
            get
            {
                return _share_condition;
            }
            set
            {
                share_conditionSpecified = true;
                _share_condition = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool share_conditionSpecified;


        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("도전자 퀘스트 수락시 필요 조건"), DisplayName("cq_condition")]
        public int cq_condition
        {
            get
            {
                return _cq_condition;
            }
            set
            {
                cq_conditionSpecified = true;
                _cq_condition = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool cq_conditionSpecified;

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("자동파티매칭 퀘스트의 필드 ID"), DisplayName("party_field")]
        public int party_field
        {
            get
            {
                return _party_field;
            }
            set
            {
                party_fieldSpecified = true;
                _party_field = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool party_fieldSpecified;

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("자동파티매칭 퀘스트의 센서 ID"), DisplayName("party_sensor")]
        public int party_sensor
        {
            get
            {
                return _party_sensor;
            }
            set
            {
                party_sensorSpecified = true;
                _party_sensor = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool party_sensorSpecified;

        [XmlAttribute]
        [Category("기타"), DescriptionAttribute("자동파티 최소 인원"), DisplayName("party_member_count")]
        public int party_member_count
        {
            get
            {
                return _party_member_count;
            }
            set
            {
                party_member_countSpecified = true;
                _party_member_count = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool party_member_countSpecified;


        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("메모를 남깁니다."), DisplayName("메모")]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("검증 여부를 설정합니다."), DisplayName("validation")]
        public bool validation
        {
            get
            {
                return _validation;
            }
            set
            {
                validationSpecified = true;
                _validation = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool validationSpecified;

        public override bool Compile()
        {
            _bPassed = (idSpecified && (title != null)) ? true : false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            if (Objectives != null && Objectives.Compile() == false)
            {
                _szCompiled = Objectives.CompiledMessage;
                _bPassed = false;
            }

            if (Rewards != null && Rewards.Compile() == false)
            {
                _szCompiled = Rewards.CompiledMessage;
                _bPassed = false;
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (_bPassed == true)
            {
                if (dialogSpecified && (!e.Dialogs.IsValid(dialog) || !e.Dialogs[dialog].Passed))
                    return Error("[기타] 항목의 dialog 값이 생성되지 않았거나 오류가 있는 대화를 참조중(" + dialog + ")");

                if (incomplete_dialogSpecified && (!e.Dialogs.IsValid(incomplete_dialog) || !e.Dialogs[incomplete_dialog].Passed))
                    return Error("[기타] 항목의 incomplete_dialog 값이 생성되지 않았거나 오류가 있는 대화를 참조중(" + incomplete_dialog + ")");

                if (conditionSpecified && (!e.Conditions.IsValid(condition) || !e.Conditions[condition].Passed)) 
                    return Error("[기타] 항목의 condition 값이 생성되지 않거나 오류가 있는 조건을 참조중 (" + condition + ")");

                if (Objectives != null && Objectives.Build(e) == false) return Error(Objectives.CompiledMessage);

                if (Rewards != null && Rewards.Build(e) == false) return Error(Rewards.CompiledMessage);
            }

            return _bPassed;
        }

        private string ToBody_NPC(SHXmlCore core)
        {
            String ret = String.Empty;
            String szNPC;

            // NPC
            foreach (SHNPC npc in core.NPCs.dataList)
            {
                if (npc.Type.ToString().ToLower() == "monster") continue;

                SHNPCIType[] _iType = npc.GetIType();
                string[] _iAct = npc.GetIAct();
                for (int i = 0; i < 10; i++)
                {
                    if (_iType[i] != SHNPCIType.quest_begin &&
                        _iType[i] != SHNPCIType.quest_end &&
                        _iType[i] != SHNPCIType.quest_count &&
                        _iType[i] != SHNPCIType.quest_count_immo &&
                        _iType[i] != SHNPCIType.challenger_quest)
                    {
                        continue;
                    }

                    if (core.Quests.IsValid(_iAct[i]) && this.id == Int32.Parse(_iAct[i]))
                    {
                        szNPC = Properties.Resources.quest_npc_template.Replace("[[NPC_NAME]]", npc.Name);

                        szNPC = szNPC.Replace("[[NPC_ID]]", npc.id.ToString());

                        string szQuestType = string.Empty;
                        if (_iType[i] == SHNPCIType.quest_begin) szQuestType = "퀘스트 시작";
                        else if (_iType[i] == SHNPCIType.quest_end) szQuestType = "퀘스트 완료";
                        else if (_iType[i] == SHNPCIType.quest_count) szQuestType = "quest count";
                        else if (_iType[i] == SHNPCIType.quest_count_immo) szQuestType = "quest count immo";
                        else if (_iType[i] == SHNPCIType.challenger_quest) szQuestType = "도전자 퀘스트";

                        szNPC = szNPC.Replace("[[QUEST_TYPE]]", szQuestType);

                        ret += szNPC;

                        break;
                    }
                }
            }
            return ret;
        }

        private string ToBody_Preview(SHXmlCore core)
        {
            String ret = String.Empty;
            ret = Properties.Resources.quest_preview_template.Replace("[[QUEST_TITLE]]", this.title);
            ret = ret.Replace("[[QUEST_DESC]]", SHWebUtil.ReplaceDialogTextForWeb(core.GetSafeString(this.desc)));

            return ret;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;

            ret = Properties.Resources.quest_template.Replace("[[ID]]", this.id.ToString());

            ret = ret.Replace("[[QUEST_TEXT]]", core.GetSafeString(this.title));

            ret = ret.Replace("[[QUEST_NPCS]]", ToBody_NPC(core));

            if (Objectives != null)
                ret = ret.Replace("[[QUEST_OBJECTIVES]]", Objectives.ToBody(core));
            else
                ret = ret.Replace("[[QUEST_OBJECTIVES]]", "");

            if (Rewards != null)
                ret = ret.Replace("[[QUEST_REWARDS]]", Rewards.ToBody(core));
            else
                ret = ret.Replace("[[QUEST_REWARDS]]", "");

            ret = ret.Replace("[[QUEST_PREVIEW]]", ToBody_Preview(core));

            //if (this.Say != null)
            //{
            //    ret += this.Say.ToBody(core);
            //}

            return ret;

        }

        public void Assign(SHQuest srcQuest)
        {
            _title = srcQuest._title;
            _desc = srcQuest._desc;
            _category = srcQuest._category;

            _repeatable = srcQuest._repeatable;
            repeatableSpecified = srcQuest.repeatableSpecified;

            _dialog = srcQuest._dialog;
            dialogSpecified = srcQuest.dialogSpecified;

            _incomplete_dialog = srcQuest._incomplete_dialog;
            incomplete_dialogSpecified = srcQuest.incomplete_dialogSpecified;

            _level = srcQuest._level;
            levelSpecified = srcQuest.levelSpecified;

            _timelimit = srcQuest._timelimit;
            timelimitSpecified = srcQuest.timelimitSpecified;

            _condition = srcQuest._condition;
            conditionSpecified = srcQuest.conditionSpecified;

            _destroy_item = srcQuest._destroy_item;

            _share = srcQuest._share;
            shareSpecified = srcQuest.shareSpecified;

            _party_field = srcQuest._party_field;
            party_fieldSpecified = srcQuest.party_fieldSpecified;

            _party_sensor = srcQuest._party_sensor;
            party_sensorSpecified = srcQuest.party_sensorSpecified;

            _party_member_count = srcQuest._party_member_count;
            party_member_countSpecified = srcQuest.party_member_countSpecified;

            _cq_condition = srcQuest._cq_condition;
            cq_conditionSpecified = srcQuest.cq_conditionSpecified;


            _share_condition = srcQuest._share_condition;
            share_conditionSpecified = srcQuest.share_conditionSpecified;

            _players = srcQuest._players;
            playersSpecified = srcQuest.playersSpecified;

            _trialtype = srcQuest._trialtype;
            trialtypeSpecified = srcQuest.trialtypeSpecified;

            _field_group = srcQuest._field_group;
            field_groupSpecified = srcQuest.field_groupSpecified;

            _comment = srcQuest._comment;

            _validation = srcQuest._validation;
            validationSpecified = srcQuest.validationSpecified;

            if (srcQuest.Items == null) this.Items = null;
            else
            {
                this.Items = (SHItemAddRemove) srcQuest.Items.Clone();
            }

            if (srcQuest.Objectives == null) this.Objectives = null;
            else
            {
                this.Objectives = (SHQuestObjectives)srcQuest.Objectives.Clone();
            }

            if (srcQuest.Rewards == null) this.Rewards = null;
            else
            {
                this.Rewards = (SHQuestRewards)srcQuest.Rewards.Clone();
            }

        }
    }
    #endregion

    #region SHQuestShareType
    [Serializable]
    public enum SHQuestShareType
    {
        none,
        manual,
        auto,
    }
    #endregion

    #region SHQuestTrialType
    [Serializable]
    public enum SHQuestTrialType
    {
        none,
        party,
        single
    }
    #endregion

    #region SHQuestObjectives
    [Serializable]
    public sealed class SHQuestObjectives : SHDataObject, ICloneable
    {
        [XmlElement(Type = typeof(SHQuestObjective), ElementName = "OBJECTIVE")]
        public ArrayList dataList;

        public SHQuestObjectives()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHQuestObjective))]
        public void Add(SHQuestObjective i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (dataList.Count <= 0)
                return Error("Objectives의 갯수가 0입니다.");

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;

            foreach (SHQuestObjective obj in dataList)
            {
                String szObj = String.Empty;
                szObj = Properties.Resources.quest_objective_template.Replace("[[QUEST_OBJ_ID]]", obj.id.ToString());

                szObj = szObj.Replace("[[QUEST_OBJ_DESC]]", core.GetSafeString(obj.desc));

                szObj = szObj.Replace("[[QUEST_OBJ_PARAM1]]", obj.param1);
                szObj = szObj.Replace("[[QUEST_OBJ_PARAM2]]", obj.param2);
                szObj = szObj.Replace("[[QUEST_OBJ_FIELD]]", obj.field.ToString());
                szObj = szObj.Replace("[[QUEST_OBJ_MARKER]]", obj.marker.ToString());

                ret += szObj;
            }

            return ret;
        }

        public object Clone()
        {
            SHQuestObjectives clone = new SHQuestObjectives();

            foreach (SHQuestObjective obj in dataList)
            {
                clone.dataList.Add((SHQuestObjective)obj.Clone());
            }
            return clone;
        }

    }
    #endregion

    #region SHQuestObjective
    [Serializable]
    public sealed class SHQuestObjective : ICloneable
    {
        private int _id;
        private string _desc;
        private SHQuestType _type;
        private string _param1;
        private string _param2;
        private int _field;
        private int _marker;
        private int _field2;
        private int _marker2;
        private string _public_progress;

        /// 
        [XmlAttribute]
        public int id
        {
            get
            {
                return _id;
            }
            set
            {
                idSpecified = true;
                _id = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool idSpecified;
        
        [XmlAttribute]
        public string desc
        {
            get
            {
                return _desc;
            }
            set
            {
                descSpecified = true;
                _desc = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool descSpecified;

        [XmlAttribute]
        public SHQuestType type
        {
            get
            {
                return _type;
            }
            set
            {
                typeSpecified = true;
                _type = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool typeSpecified;

        [XmlAttribute]
        public string param1
        {
            get
            {
                return _param1;
            }
            set
            {
                param1Specified = true;
                _param1 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool param1Specified;

        [XmlAttribute]
        public string param2
        {
            get
            {
                return _param2;
            }
            set
            {
                param2Specified = true;
                _param2 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool param2Specified;


        [XmlAttribute]
        public int field
        {
            get
            {
                return _field;
            }
            set
            {
                fieldSpecified = true;
                _field = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool fieldSpecified;


        [XmlAttribute]
        public int marker
        {
            get
            {
                return _marker;
            }
            set
            {
                markerSpecified = true;
                _marker = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool markerSpecified;

        [XmlAttribute]
        public int field2
        {
            get
            {
                return _field2;
            }
            set
            {
                field2Specified = true;
                _field2 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool field2Specified;


        [XmlAttribute]
        public int marker2
        {
            get
            {
                return _marker2;
            }
            set
            {
                marker2Specified = true;
                _marker2 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool marker2Specified;

        [XmlAttribute]
        public string public_progress
        {
            get
            {
                return _public_progress;
            }
            set
            {
                public_progressSpecified = true;
                _public_progress = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool public_progressSpecified;

        public object Clone()
        {
            SHQuestObjective clone = new SHQuestObjective();

            clone._id = this._id;
            clone.idSpecified = this.idSpecified;

            clone._desc = this._desc;
            clone.descSpecified = this.descSpecified;

            clone._type = this._type;
            clone.typeSpecified = this.typeSpecified;

            clone._param1 = this._param1;
            clone.param1Specified = this.param1Specified;

            clone._param2 = this._param2;
            clone.param2Specified = this.param2Specified;

            clone._field = this._field;
            clone.fieldSpecified = this.fieldSpecified;

            clone._marker = this._marker;
            clone.markerSpecified = this.markerSpecified;

            clone._field2 = this._field2;
            clone.field2Specified = this.field2Specified;

            clone._marker2 = this._marker2;
            clone.marker2Specified = this.marker2Specified;

            clone._public_progress = this._public_progress;
            clone.public_progressSpecified = this.public_progressSpecified;

            
            return clone;
        }

    }
    #endregion

    #region SHQuestType
    [Serializable]
    public enum SHQuestType
    {
        DESTROY,
        COLLECT,
        ACT,
        SCOUT,
        INTERACT,
        ESCORT
    }
    #endregion

    #region SHQuestReward
    [Serializable]
    public sealed class SHQuestReward : SHDataObject, ICloneable
    {
        private SHQuestRewardType _type;
        private string _param1;
        private string _param2;

        [XmlAttribute]
        public SHQuestRewardType type
        {
            get
            {
                return _type;
            }
            set
            {
                typeSpecified = true;
                _type = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool typeSpecified;
        
        [XmlAttribute]
        public string param1
        {
            get
            {
                return _param1;
            }
            set
            {
                param1Specified = true;
                _param1 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool param1Specified;

        [XmlAttribute]
        public string param2
        {
            get
            {
                return _param2;
            }
            set
            {
                param2Specified = true;
                _param2 = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool param2Specified;


        public override bool Compile()
        {
            _bPassed = true;
            _szCompiled = SHGlobal.COK;

            switch (type)
            {
                case SHQuestRewardType.ITEM:
                case SHQuestRewardType.FACTION:
                    if (param1 == null || param2 == null) return Error(type.ToString() + "형식의 보상에 사용된 파라미터가 부적합합니다. (모든 인수가 명시되어야 함)");
                    break;
                case SHQuestRewardType.TITLE:
                case SHQuestRewardType.XP:
                case SHQuestRewardType.SILVER:
                    if (param1 == null || param2 != null) return Error(type.ToString() + "형식의 보상에 사용된 파라미터가 부적합합니다. (첫번째 인자만 사용됨)");
                    break;
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            switch (type)
            {
                case SHQuestRewardType.ITEM:    // 콤마로 구분된 아이템을 전부 검사한다.
                    try
                    {
                        if (param1 != null)
                        {
                            String[] szItemIDs = param1.Split(new Char[] { ',' });

                            foreach (String szID in szItemIDs)
                            {
                                int nID = Int32.Parse(szID.Trim());
                                if (!e.Items.IsValid(nID) || !e.Items[nID].Passed) return Error(type.ToString() + "형식의 보상에 사용된 아이템이 미생성 아이템이거나 오류를 가지고 있습니다. (" + nID.ToString() + ", 원본 문자열:" + param1 + ")");
                            }
                        }
                    }
                    catch
                    {
                        return Error(type.ToString() + "형식의 보상에 사용된 파라미터가 부적합합니다. (미생성 아이템이거나 오류를 가진 아이템)");
                    }
                    break;
                case SHQuestRewardType.TITLE:
                case SHQuestRewardType.XP:
                case SHQuestRewardType.SILVER:
                case SHQuestRewardType.FACTION:
                    break;
            }

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public object Clone()
        {
            SHQuestReward clone = new SHQuestReward();


            clone._type = this._type;
            clone.typeSpecified = this.typeSpecified;

            clone._param1 = this._param1;
            clone.param1Specified = this.param1Specified;

            clone._param2 = this._param2;
            clone.param2Specified = this.param2Specified;


            return clone;
        }

    }
    #endregion

    #region SHQuestRewardType
    [Serializable]
    public enum SHQuestRewardType
    {
        SILVER,
        XP,
        TITLE,
        ITEM,
        FACTION,
    }
    #endregion

    #region SHQuestRewards
    [Serializable]
    public sealed partial class SHQuestRewards : SHDataObject, ICloneable
    {
        private int _dialog;


        [XmlElement(Type = typeof(SHQuestReward), ElementName = "REWARD")]
        public ArrayList dataList;
        
        [XmlAttribute]
        public int dialog
        {
            get
            {
                return _dialog;
            }
            set
            {
                dialogSpecified = true;
                _dialog = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool dialogSpecified;
        
        [XmlAttribute]
        public string location;

        private string _rewarder;
        [XmlAttribute]
        public string rewarder
        {
            get { return _rewarder;  }
            set {
                String[] tmp = value.Split(new Char[] { ',' });
                if (tmp.Length != 2)
                {
                    throw new ArgumentException("rewarder의 형식이 잘못되어 있습니다.");
                }

                String val = tmp[0].Trim() + "," + tmp[1].Trim();
                _rewarder = val;
            }
        }

        public SHQuestRewards()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHQuestReward))]
        public void Add(SHQuestReward i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;
            _szCompiled = SHGlobal.COK;

            if (!dialogSpecified) return Error("퀘스트 보상(Rewards)을 위한 대화문(dialog)이 기술되어 있지 않습니다.");
            
            for (int i = 0; i < dataList.Count; i++)
            {
                SHQuestReward reward = (SHQuestReward)dataList[i];
                if (reward.Compile() == false) return Error(reward.CompiledMessage);
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            if (!e.Dialogs.IsValid(dialog) || !e.Dialogs[dialog].Passed)
            {
                _szCompiled = "퀘스트 종료를 위한 대화문이 정의되어 있지 않거나 오류가 있습니다.";
                _bPassed = false;
                return false;
            }

            for (int i = 0; i < dataList.Count; i++)
            {
                SHQuestReward reward = (SHQuestReward)dataList[i];
                if (reward.Build(e) == false) return Error(reward.CompiledMessage);
            }

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;

            ret = Properties.Resources.quest_reward_template.Replace("[[QUEST_REWARD_DIALOG_ID]]", dialog.ToString());

            string strDialogName = "";

            if (core.Dialogs.IsValid(dialog))
            {
                strDialogName = core.Dialogs[dialog].text;
            }

            ret = ret.Replace("[[QUEST_REWARD_DIALOG_NAME]]", strDialogName);

            ret = ret.Replace("[[QUEST_REWARD_REWARDER]]", _rewarder);

            foreach (SHQuestReward reward in dataList)
            {
                String szReward = String.Empty;

                //szReward = szReward.Replace("[[QUEST_REWARD_REWARDER]]", core.GetSafeString(obj.desc));

                ret += szReward;
            }

            return ret;
        }

        public object Clone()
        {
            SHQuestRewards clone = new SHQuestRewards();

            foreach (SHQuestReward obj in dataList)
            {
                clone.dataList.Add((SHQuestReward)obj.Clone());
            }


            clone._dialog = this._dialog;
            clone.dialogSpecified = this.dialogSpecified;
            clone.location = this.location;
            clone._rewarder = this._rewarder;

            return clone;
        }

    }
    #endregion

    public class QuestSaver
    {

    }
}
