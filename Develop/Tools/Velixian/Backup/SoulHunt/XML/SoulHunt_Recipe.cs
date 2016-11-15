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
    #region SHRecipe
    [Serializable, DefaultProperty("id")]
    public sealed class SHRecipe : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("레시피의 아이디. 이 값은 유일해야합니다. 단, 본 프로그램은 이에 관련된 유효성 검사를 하지 않습니다."), DisplayName("(아이디)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        private int _item;
        [XmlAttribute]
        public int item
        {
            get { return _item; }
            set { itemidSpecified = (value != 0) ? true : false; _item = value; }
        }
        [XmlIgnore]
        public bool itemidSpecified;

        [XmlAttribute]
        public int amount { get; set; }
        [XmlAttribute]
        public int mat1 { get; set; }
        [XmlAttribute]
        public int mat1_amount { get; set; }
        [XmlAttribute]
        public int mat2 { get; set; }
        [XmlAttribute]
        public int mat2_amount { get; set; }
        [XmlAttribute]
        public int mat3 { get; set; }
        [XmlAttribute]
        public int mat3_amount { get; set; }
        [XmlAttribute]
        public int mat4 { get; set; }
        [XmlAttribute]
        public int mat4_amount { get; set; }
        [XmlAttribute]
        public int price { get; set; }

        private int _condition;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("아이템 사용조건"), DisplayName("사용조건")]
        public int condition
        {
            get { return _condition; }
            set { conditionSpecified = (value != 0) ? true : false; _condition = value; }
        }
        [XmlIgnoreAttribute]
        public bool conditionSpecified;

        public override bool Compile()
        {
            _bPassed = (idSpecified && itemidSpecified) ? true : false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

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
    public sealed class SHRecipeXml : SHDataObject
    {
        [XmlElement(Type = typeof(SHRecipe), ElementName = "RECIPE")]
        public ArrayList dataList;

        private Dictionary<int, SHRecipe> Index;

        public SHRecipe this[int nIndex]
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

        public SHRecipe this[string szIndex]
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

        public SHRecipeXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHRecipe>();
        }

        //[XmlInclude(typeof(SHRecipe))]
        //public void Add(SHRecipe i)
        //{
        //    dataList.Add(i);
        //}

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHRecipe item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            foreach (SHRecipe item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}
