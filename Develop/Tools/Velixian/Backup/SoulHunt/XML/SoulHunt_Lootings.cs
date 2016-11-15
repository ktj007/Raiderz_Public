#region Using문
using System;
using System.ComponentModel;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;
#endregion

namespace SoulHunt
{
    [Serializable]
    public class SHLootable : SHDataObject
    {
        private double _rate;
        [XmlAttribute]
        [Category("공통"), DescriptionAttribute("이 전리품이 나올 확률. 0부터 100사이의 값으로 명시합니다. 이 값은 모든 공통속성에 우선적용됩니다."), DisplayName("(확률)")]
        public double rate
        {
            get { return _rate; }
            set { rateSpecified = (value != 0) ? true : false; _rate = value; }
        }
        [XmlIgnore]
        public bool rateSpecified;

        private int _min;
        [XmlAttribute]
        [Category("공통"), DescriptionAttribute("이 전리품이 나온다고 판정되었을 때 최소 얼만큼 나올지를 결정합니다."), DisplayName("최소치")]
        public int min
        {
            get { return _min; }
            set { minSpecified = (value != 0) ? true : false; _min = value; }
        }
        [XmlIgnore]
        public bool minSpecified;

        private int _max;
        [XmlAttribute]
        [Category("공통"), DescriptionAttribute("이 전리품이 나온다고 판정되었을 때 최대 얼만큼 나올지를 결정합니다."), DisplayName("최대치")]
        public int max
        {
            get { return _max; }
            set { maxSpecified = (value != 0) ? true : false; _max = value; }
        }
        [XmlIgnore]
        public bool maxSpecified;

        public override bool Compile()
        {
            _bPassed = true;
            _szCompiled = SHGlobal.COK;

            if (rate < 0 || rate > 100) return Error("확률 속성은 0과 100사이의 실수여야 합니다.");
            if (min > max) return Error("min속성이 max속성보다 큽니다.");

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = Properties.Resources.loot_element_template;

            ret = ret.Replace("[[LOOT_RATE]]", (rateSpecified) ? rate.ToString("##.#") + "%" : String.Empty);
            ret = ret.Replace("[[LOOT_MAX]]", (maxSpecified) ? max.ToString() : String.Empty);
            ret = ret.Replace("[[LOOT_MIN]]", (minSpecified) ? min.ToString() : String.Empty);

            return ret;
        }
    }

    [Serializable]
    public sealed class SHLootSilver : SHLootable
    {
        public override string ToBody(SHXmlCore e)
        {
            String ret = base.ToBody(e);

            ret = ret.Replace("[[LOOT_DETAIL]]", "실버");

            return ret;
        }
    }

    [Serializable]
    public sealed class SHLootItem : SHLootable
    {
        private int _id;
        [XmlAttribute]
        [Category("아이템"), DescriptionAttribute("전리품에 포함될 아이템 아이디를 기입합니다. 아이디 옆의 버튼을 누르시면 보다 쉽게 입력할 수 있습니다."), DisplayName("(아이템 ID)"), Editor(typeof(VCEItemEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public int id
        {
            get { return _id; }
            set { idSpecified = (value != 0) ? true : false; _id = value; }
        }
        [XmlIgnore]
        public bool idSpecified;

        private int _quest;
        [XmlAttribute]
        [Category("아이템"), DescriptionAttribute("만일 특정 퀘스트가 있을때에만 이 아이템이 드랍되길 원한다면 퀘스트 아이디를 명시합니다. 아이디 옆의 버튼을 누르시면 보다 쉽게 입력할 수 있습니다."), DisplayName("퀘스트"), Editor(typeof(VCEQuestEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public int quest
        {
            get { return _quest; }
            set { questSpecified = (value != 0) ? true : false; _quest = value; }
        }
        [XmlIgnore]
        public bool questSpecified;
        
        private int _bpart;
        [XmlAttribute]
        [Category("아이템"), DescriptionAttribute("아이템이 전리품에 포함될 때 몬스터의 특정 부위가 파괴시에만 가능하도록 하고 싶다면 브레이커블 파츠의 아이디를 명시합니다. 이 값은 1~4까지 유효합니다."), DisplayName("파괴부위")]
        public string bpart
        {
            get { return bpartSpecified ? _bpart.ToString() : "없음"; }
            set
            {
                if (value != String.Empty)
                {
                    bpartSpecified = true;
                    _bpart = System.Convert.ToInt32(value, 10);
                }
                else
                {
                    bpartSpecified = false;
                }
            }
        }

        [XmlIgnore]
        public bool bpartSpecified;

        public override bool Compile()
        {
            if (!base.Compile()) return false;
            if (!idSpecified) return Error("아이템의 아이디가 명시되지 않았습니다.");
            if (bpartSpecified && (_bpart < 1 || _bpart > 4)) return Error("파괴부위 아이디가 잘못 명시되었습니다. 파괴부위 아이디는 반드시 1부터 4 사이의 값을 가지도록 합니다.");

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed || !base.Build(e)) return false;

            if (questSpecified && (!e.Quests.IsValid(quest) || !e.Quests[quest].Passed)) return Error("명시된 퀘스트 속성의 아이디가 존재하지 않거나 문법오류를 가집니다. (" + quest.ToString() + ")");

            if (!e.Items.IsValid(id)) return Error("명시된 아이템이 존재하지 않습니다. (아이템 ID:" + id.ToString() + ")");
            if (!e.Items[id].Passed) return Error("명시된 아이템이 문법오류를 가집니다. (아이템 ID:" + id.ToString() + ")");

            return _bPassed;
        }

        public override string ToBody(SHXmlCore e)
        {
            String ret = base.ToBody(e);
            String detail = "[" + (e.Items.IsValid(id) ? e.GetAnchorString(e.Items[id].name, "ITEM", id.ToString(), true) : "(유효하지 않은)" + id.ToString()) + "]";

            if (questSpecified || bpartSpecified)
            {
                detail += " (단, ";
                
                if (questSpecified) detail += "퀘스트 [" + (e.Quests.IsValid(quest) ? e.GetAnchorString(e.Quests[quest].title, "QUEST", e.Quests[quest].id.ToString(), true) : "(유효하지 않은)" + quest.ToString()) + "] 수행중";
                if (questSpecified && bpartSpecified) detail += "이고 ";
                if (bpartSpecified) detail += "파츠 " + bpart + "번이 파괴";

                detail += "일때만)";
            }

            ret = ret.Replace("[[LOOT_DETAIL]]", detail);

            return ret;
        }
    }

    [Serializable]
    public enum SHLootSoulType
    {
        S,
        A,
        B,
        C,
    }

    [Serializable]
    public sealed class SHLootSoul : SHLootable
    {
        private SHLootSoulType _type;
        [XmlAttribute, Category("영혼"), DescriptionAttribute("어떤 등급의 영혼이 나올지 명시합니다."), DisplayName("(영혼등급)")]
        public SHLootSoulType type
        {
            get { return _type; }
            set { typeSpecified = true; _type = value; }
        }
        [XmlIgnore]
        public bool typeSpecified;


        private double _bpart1;
        [XmlAttribute, Category("영혼"), DescriptionAttribute("부위 1번이 파괴되었을 때, 영혼적출 확률을 얼만큼 수정할지 결정합니다."), DisplayName("파트 1 수정치")]
        public double bpart1
        {
            get { return _bpart1; }
            set { bpart1Specified = (value != 0) ? true : false; _bpart1 = value; }
        }
        [XmlIgnore]
        public bool bpart1Specified;

        private double _bpart2;
        [XmlAttribute, Category("영혼"), DescriptionAttribute("부위 2번이 파괴되었을 때, 영혼적출 확률을 얼만큼 수정할지 결정합니다."), DisplayName("파트 2 수정치")]
        public double bpart2
        {
            get { return _bpart2; }
            set { bpart2Specified = (value != 0) ? true : false; _bpart2 = value; }
        }
        [XmlIgnore]
        public bool bpart2Specified;

        private double _bpart3;
        [XmlAttribute, Category("영혼"), DescriptionAttribute("부위 3번이 파괴되었을 때, 영혼적출 확률을 얼만큼 수정할지 결정합니다."), DisplayName("파트 3 수정치")]
        public double bpart3
        {
            get { return _bpart3; }
            set { bpart3Specified = (value != 0) ? true : false; _bpart3 = value; }
        }
        [XmlIgnore]
        public bool bpart3Specified;

        private double _bpart4;
        [XmlAttribute, Category("영혼"), DescriptionAttribute("부위 4번이 파괴되었을 때, 영혼적출 확률을 얼만큼 수정할지 결정합니다."), DisplayName("파트 4 수정치")]
        public double bpart4
        {
            get { return _bpart4; }
            set { bpart4Specified = (value != 0) ? true : false; _bpart4 = value; }
        }
        [XmlIgnore]
        public bool bpart4Specified;

        public override bool Compile()
        {
            if (!base.Compile()) return false;
            if (!typeSpecified) return Error("영혼의 종류가 명시되지 않았습니다.");

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = base.ToBody(core);
            String detail = type.ToString() + "등급 영혼";

            double bpartmax = bpart1 + bpart2 + bpart3 + bpart4;
            detail += " (파트 파괴별 보정치:" + bpart1.ToString("##.#") + "/" + bpart2.ToString("##.#") + "/" + bpart3.ToString("##.#") + "/" + bpart4.ToString("##.#") + " - 최대 " + bpartmax + "% 보정)";

            ret = ret.Replace("[[LOOT_DETAIL]]", detail);

            return ret;
        }
    }

    [Serializable]
    public sealed class SHLoot : SHDataObject
    {
        private int _id;
        [XmlAttribute("id")]
        [DisplayName("(아이디)")]
        public int id {
            get { return _id; }
            set { idSpecified = (value != 0) ? true : false; _id = value; }
        }
        [XmlIgnore]
        public bool idSpecified;

        private string _comment;
        [XmlAttribute]
        [DisplayName("주석"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        private int _max;
        [XmlAttribute("max")]
        [DisplayName("(최대갯수)")]
        public int max {
            get { return _max; }
            set { maxSpecified = (value != 0) ? true : false; _max = value; }
        }
        [XmlIgnore]
        public bool maxSpecified;


        [XmlElement(Type = typeof(SHLootSilver), ElementName = "SILVER"), XmlElement(Type = typeof(SHLootSoul), ElementName = "SOUL"), XmlElement(Type = typeof(SHLootItem), ElementName = "ITEM")]
        public ArrayList dataList;

        [XmlInclude(typeof(SHLootSilver))]
        public void Add(SHLootSilver i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHLootItem))]
        public void Add(SHLootItem i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHLootSoul))]
        public void Add(SHLootSoul i)
        {
            dataList.Add(i);
        }

        public SHLoot()
        {
            dataList = new ArrayList();
        }

        public override bool Compile()
        {
            _bPassed = true;

            if (!idSpecified)
            {
                _szCompiled = "ID가 명시되지 않은 개체입니다.";
                _bPassed = false;
            }

            foreach (object obj in dataList)
            {
                if (obj.GetType() == typeof(SHLootSilver))
                {
                    SHLootSilver silver = (SHLootSilver)obj;
                    if (!silver.Compile()) return Error(silver.CompiledMessage);
                }
                else if (obj.GetType() == typeof(SHLootItem))
                {
                    SHLootItem item = (SHLootItem)obj;
                    if (!item.Compile()) return Error(item.CompiledMessage);
                }
                else if (obj.GetType() == typeof(SHLootSoul))
                {
                    SHLootSoul soul = (SHLootSoul)obj;
                    if (!soul.Compile()) return Error(soul.CompiledMessage);
                }
            }
            
            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            if (!_bPassed) return false;

            if (dataList.Count == 0) return Error("획득가능한 전리품이 없습니다.");

            foreach (object obj in dataList)
            {
                if (obj.GetType() == typeof(SHLootSilver))
                {
                    SHLootSilver silver = (SHLootSilver)obj;
                    if (!silver.Build(core)) return Error(silver.CompiledMessage);
                }
                else if (obj.GetType() == typeof(SHLootItem))
                {
                    SHLootItem item = (SHLootItem)obj;
                    if (!item.Build(core)) return Error(item.CompiledMessage);
                }
                else if (obj.GetType() == typeof(SHLootSoul))
                {
                    SHLootSoul soul = (SHLootSoul)obj;
                    if (!soul.Build(core)) return Error(soul.CompiledMessage);
                }
            }

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret;
            String elements = String.Empty;
            Int32 i = 0;

            ret = Properties.Resources.loot_template.Replace("[[LOOT_ID]]", id.ToString());
            ret = ret.Replace("[[LOOT_MAX]]", max.ToString());

            foreach (SHDataObject obj in dataList)
            {
                i++;
                elements += obj.ToBody(core);
            }

            ret = ret.Replace("[[LOOT_ELEMENTS]]", elements);

            return ret;
        }

        public string GetString(SHXmlCore e)
        {
            string ret = "";

            ret = "총 " + dataList.Count.ToString() + "개 종류, 최대 " + max.ToString() + "개";

            return ret;
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHLootingsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHLoot), ElementName = "LOOT")]
        public ArrayList dataList { get; set; }
        [XmlIgnoreAttribute]
        private Dictionary<int, SHLoot> Index;

        public override bool IsValid(String szIndex)
        {
            try
            {
                return Index.ContainsKey(Int32.Parse(szIndex));
            }
            catch
            {
                return false;
            }
        }

        public override bool IsValid(Int32 nIndex)
        {
            return Index.ContainsKey(nIndex);
        }

        public SHLoot this[int nKey]
        {
            get
            {
                try
                {
                    return Index[nKey];
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }

        public SHLootingsXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHLoot>();
        }

        [XmlInclude(typeof(SHLoot))]
        public void Add(SHLoot i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            Sort();

            foreach (SHLoot loot in dataList)
            {
                if (!loot.Compile()) _bPassed = false;
                Index[loot.id] = loot;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            foreach (SHLoot loot in dataList)
            {
                if (!loot.Build(e)) _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        class LootComparer : IComparer
        {
            public LootComparer()
            {
            }

            public int Compare(object x, object y)
            {
                SHLoot lFirst = (SHLoot)x;
                SHLoot lSecond = (SHLoot)y;

                return lFirst.id.CompareTo(lSecond.id);
            }
        }

        public override void Sort()
        {
            dataList.Sort(new LootComparer());
        }

    }
}