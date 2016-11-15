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
    #region SHNPCShop

    [Serializable]
    public sealed class SHNPCShopItem : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("항목"), DescriptionAttribute("항목에 포함될 아이템 아이디를 기입합니다. 아이디 옆의 버튼을 누르시면 보다 쉽게 입력할 수 있습니다."), DisplayName("(아이템 ID)"), Editor(typeof(VCEItemEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public int id
        {
            get { return _id; }
            set { idSpecified = (value != 0) ? true : false; _id = value; }
        }
        public bool idSpecified;

        public override string ToBody(SHXmlCore e)
        {
            throw new NotImplementedException();
        }
        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;
            return _bPassed;
        }

        public override bool Compile()
        {
            _bPassed = true;

            if (!idSpecified)
            {
                _szCompiled = "ID가 명시되지 않은 개체입니다.";
                _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

    }

    [Serializable, DefaultProperty("id")]
    public sealed class SHNPCShop : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("상점의 아이디. 이 값은 유일해야합니다. 단, 본 프로그램은 이에 관련된 유효성 검사를 하지 않습니다."), DisplayName("(아이디)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("다이얼로그의 아이디"), DisplayName("(다이얼로그)")]
        public int dialog { get; set; }

        [XmlAttribute]
        public float buymod { get; set; }
        [XmlAttribute]
        public float sellmod { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("우호도"), DisplayName("(우호도)")]
        public int faction { get; set; }
        [XmlAttribute]
        public bool repairable { get; set; }

        [XmlElement(Type = typeof(SHItemString), ElementName = "ITEM")]
        public ArrayList dataList;

        public bool IsLicensable(SHItemsXml Items, SHTalentsXml Talents)
        {
            foreach (SHItemString itemstring in dataList)
            {
                foreach (int itemID in itemstring.ItemIDList)
                {
                    if (Items.IsValid(itemID) == false) continue;

                    SHItem item = Items[itemID];
                    if(item.usable_type == "talent_train")
                    {
                        int talentid = Convert.ToInt32(item.usable_param);
                        if (Talents.IsValid(talentid) == false) continue;

                        SHTalent talentinfo = Talents[talentid];
                        if (talentinfo.style == "license") return true;                        
                    }
                }
            }

            return false;
        }

        public SHNPCShop() : base()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHItemString))]
        public void Add(SHItemString i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            if (!idSpecified)
            {
                _szCompiled = "ID가 명시되지 않은 개체입니다.";
                _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            if (dataList.Count == 0) return Error("레시피가 없습니다.");

            //if (conditionSpecified && !e.ItemConditions.IsValid(condition)) return Error("명시된 아이템이 유효하지 않습니다. (" + condition.ToString() + ")");
            //if (use_talentSpecified && !e.Talents.IsValid(use_talent)) return Error("명시된 탤런트가 유효하지 않습니다. (" + use_talent.ToString() + ")");
            //if (use_questSpecified && !e.Quests.IsValid(use_quest)) return Error("명시된 퀘스트가 유효하지 않습니다. (" + use_quest.ToString() + ")");

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
    #endregion

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHNPCShopXml : SHDataObject
    {
        [XmlElement(Type = typeof(SHNPCShop), ElementName = "NPCSHOP")]
        public ArrayList dataList;

        private Dictionary<int, SHNPCShop> Index;

        public SHNPCShop this[int nIndex]
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

        public SHNPCShop this[string szIndex]
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

        public bool IsValid(String szIndex)
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

        public bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
        }

        public SHNPCShopXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHNPCShop>();
        }

        [XmlInclude(typeof(SHNPCShop))]
        public void Add(SHNPCShop i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHNPCShop item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            foreach (SHNPCShop item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }

    #region SHItemString
    [Serializable]
    public sealed class SHItemString
    {
        public ArrayList ItemIDList;

        private string _id;
        [XmlAttribute]
        public string id
        {
            get { return _id; }
            set
            {
                _id = value;

                bool bExistTilde = _id.Contains("~");
                bool bExistComma = _id.Contains(",");

                if (bExistTilde == true)
                {
                    //ms-help://MS.VSCC.v90/MS.MSDNQTR.v90.ko/dv_csref/html/729c2923-4169-41c6-9c90-ef176c1e2953.htm
                    string[] items = _id.Split('~');

                    if (items.Length != 2) return;

                    int nBeingNumber = Convert.ToInt32(items[0]);
                    int nEndNumber = Convert.ToInt32(items[1]);

                    for (int i = nBeingNumber; nEndNumber >= i; i++)
                    {
                        ItemIDList.Add(i);
                    }
                }
                else if (bExistComma == true)
                {
                    string[] recipes = _id.Split(',');

                    foreach (string s in recipes)
                    {
                        ItemIDList.Add(Convert.ToInt32(s));
                    }
                }
                else
                {
                    ItemIDList.Add(Convert.ToInt32(_id));
                }
            }
        }
    }
    #endregion
}
