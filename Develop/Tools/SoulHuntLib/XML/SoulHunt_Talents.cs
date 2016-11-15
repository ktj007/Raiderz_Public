using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

namespace SoulHunt
{
    [Serializable]
    public sealed class SHTalent : SHDataObject
    {
        [XmlAttribute("id")]
        public int id { get; set; }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute("name")]
        public string name { get; set; }

        [XmlAttribute("Style")]
        public string style;

        public override bool Compile()
        {
            _bPassed = true;

            if (!idSpecified || name == null) return Error("id가 없거나 이름이 정해져있지 않은 탤런트");

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
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHTalentsXml : SHDataObject
    {
        private Dictionary<int, SHTalent> Index;

        [XmlElement(Type = typeof(SHTalent), ElementName = "TALENT")]
        public ArrayList dataList;
        
        public SHTalent this[int nIndex]
        {
            get {
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

        public SHTalent this[string szIndex]
        {
            get {
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

        public SHTalentsXml()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHTalent>();
        }

        [XmlInclude(typeof(SHTalent))]
        public void Add(SHTalent i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHTalent talent in dataList)
            {
                if (!talent.Compile()) _bPassed = false;
                Index[talent.id] = talent;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore core)
        {
            return true;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}
