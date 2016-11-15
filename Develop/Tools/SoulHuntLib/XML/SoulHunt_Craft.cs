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
    #region SHCraft
    [Serializable, DefaultProperty("id")]
    public sealed class SHCraft : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("크래프트의 아이디. 이 값은 유일해야합니다. 단, 본 프로그램은 이에 관련된 유효성 검사를 하지 않습니다."), DisplayName("(아이디)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        //private int _dialog;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("다이얼로그의 아이디"), DisplayName("(다이얼로그)")]
        public int dialog  { get; set; }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("아이템 생산 요율"), DisplayName("(요뉼)")]
        public float makemod { get; set; }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("우호도"), DisplayName("(우호도)")]
        public int faction { get; set; }

        [XmlAttribute]
        public string type;
        [XmlAttribute]
        public bool hidden;
    
        //#region SHRecipe
        [XmlElement(Type = typeof(SHRecipeString), ElementName = "RECIPE")]
        public ArrayList dataList;

        public ArrayList RecipeIDList;

        [XmlInclude(typeof(SHRecipeString))]
        public void Add(SHRecipeString i)
        {
            dataList.Add(i);
        }
        //#endregion

        public override bool Compile()
        {
            _bPassed = true;

            if (!idSpecified)
            {
                _szCompiled = "ID가 명시되지 않은 개체입니다.";
                _bPassed = false;
            }

            foreach (SHRecipeString recipestr in dataList)
            {
                foreach(int id in recipestr.RecipeIDList)
                {
                    RecipeIDList.Add(id);
                }
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

    #region SHCraftXml
    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHCraftXml : SHDataObject
    {
        [XmlElement(Type = typeof(SHCraft), ElementName = "CRAFT")]
        public ArrayList dataList;
        
        private Dictionary<int, SHCraft> Index;

        public SHCraft this[int nIndex]
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

        public SHCraft this[string szIndex]
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

        public SHCraftXml():base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHCraft>();
        }

        //[XmlInclude(typeof(SHCraft))]
        //public void Add(SHCraft i)
        //{
        //    dataList.Add(i);
        //}

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHCraft item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            foreach (SHCraft item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
    #endregion

    //#region SHRecipe
    //[Serializable]
    //public sealed class SHRecipe
    //{
    //    [XmlAttribute]
    //    public int id;
    //}
    //#endregion

    #region SHRecipeString
    [Serializable]
    public sealed class SHRecipeString
    {
        public ArrayList RecipeIDList;

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
                    string[] recipes = _id.Split('~');

                    if (recipes.Length != 2) return;

                    int nBeingNumber = Convert.ToInt32(recipes[0]);
                    int nEndNumber = Convert.ToInt32(recipes[1]);

                    for (int i = nBeingNumber; nEndNumber >= i; i++)
                    {
                        RecipeIDList.Add(i);
                    }
                }
                else if (bExistComma == true)
                {
                    string[] recipes = _id.Split(',');

                    foreach (string s in recipes)
                    {
                        RecipeIDList.Add(Convert.ToInt32(s));
                    }
                }
                else
                {
                    RecipeIDList.Add(Convert.ToInt32(_id)); 
                }
            }
        }
    }
    #endregion
}
