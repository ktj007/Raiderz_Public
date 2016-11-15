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
    #region SHItemRelatedNPCXml

    [Serializable]
    [XmlRoot("maiet")]   
    
    public sealed class SHItemRelatedNPCXml : SHDataObject
    {
        [XmlElement(Type = typeof(SHItemRelatedNPC_Item), ElementName = "ITEM")]
        public ArrayList dataList;

        private Dictionary<int, SHItemRelatedNPC_Item> Index;

        public SHItemRelatedNPC_Item this[int nIndex]
        {
            get
            {
                try { return Index[nIndex];  }
                catch { return null;  }
            }
        }

        public SHItemRelatedNPC_Item this[string szIndex]
        {
            get
            {
                try { return Index[Convert.ToInt32(szIndex)]; }
                catch { return null; }
            }
        }

        public bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
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

        public SHItemRelatedNPCXml()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHItemRelatedNPC_Item>();
        }

        [XmlInclude(typeof(SHItemRelatedNPC_Item))]
        public void Add(SHItemRelatedNPC_Item i)
        {
            dataList.Add(i);
            Index[i.id] = i;
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHItemRelatedNPC_Item itemDropNPC in dataList)
            {                
                if (!itemDropNPC.Compile()) _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    #endregion

    #region SHItemRelatedNPC_Item

    [Serializable]
    public sealed class SHItemRelatedNPC_Item : SHDataObject
    {
        private int _id;

        [XmlElement(Type = typeof(SHItemRelatedNPC_NPC), ElementName = "NPC")]
        public ArrayList dataList;

        private Dictionary<int, SHItemRelatedNPC_NPC> Index;

        public SHItemRelatedNPC_NPC this[int nIndex]
        {
            get
            {
                try
                {
                    return Index[nIndex];
                }
                catch
                {
                    return null;
                }
            }
        }

        public SHItemRelatedNPC_NPC this[string szIndex]
        {
            get
            {
                try
                {
                    return Index[Convert.ToInt32(szIndex)];
                }
                catch
                {
                    return null;
                }
            }
        }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("Item의 아이디값입니다."), DisplayName("(아이템 아이디)")]
        public int id
        {
            get
            {
                return _id;
            }
            set
            {
                _id = value;
            }
        }

        public SHItemRelatedNPC_Item()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHItemRelatedNPC_NPC>();
        }

        [XmlInclude(typeof(SHItemRelatedNPC_NPC))]
        public void Add(SHItemRelatedNPC_NPC i)
        {
            dataList.Add(i);
            Index[i.id] = i;
        }

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            foreach (SHItemRelatedNPC_NPC itemDropNPC_NPC in dataList)
            {
                if (!itemDropNPC_NPC.Compile())
                {
                    _bPassed = false;
                    _szCompiled = itemDropNPC_NPC.CompiledMessage;
                    break;
                }
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }

    #endregion

    #region SHItemRelatedNPC_NPC

    [Serializable]
    public sealed class SHItemRelatedNPC_NPC : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("NPC의 아이디값입니다."), DisplayName("(엔피씨 아이디)")]
        public int id
        {
            get
            {
                return _id;
            }
            set
            {
                _id = value;
            }
        }

        public override bool Compile()
        {
            _bPassed = true;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }

    #endregion

}
