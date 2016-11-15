using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

namespace CombatMonitor.XML
{
    [Serializable]
    public sealed class XMLTalentElement : XMLObject
    {
        [XmlAttribute("id")]
        public int id;
        [XmlIgnoreAttribute]
        public bool idSpecified { get; set; }

        [XmlAttribute("name")]
        public string name;

        [XmlAttribute("Category")]
        public string Category;

        [XmlAttribute("SkillType")]
        public string SkillType;

        [XmlAttribute("NPC")]
        public string NPC;

        [XmlAttribute("Style")]
        public string style;

        [XmlAttribute("MinDamage")]
        public int MinDamage;

        [XmlAttribute("MaxDamage")]
        public int MaxDamage;

        [XmlAttribute("CoolTime")]
        public double Cooltime;

        [XmlAttribute("WeaponApplyRate")]
        public double WeaponApplyRate;
        
        public override bool Cooking()
        {
            _bPassed = true;

            if (!idSpecified || name == null) return Error("id가 없거나 이름이 정해져있지 않은 탤런트");

            return _bPassed;
        }

        public bool IsUsingByNPC()
        {
            if (NPC == null)
                return false;

            return NPC.Length >= 0;
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class XMLTalent : XMLObject
    {
        private Dictionary<int, XMLTalentElement> Index;

        [XmlElement(Type = typeof(XMLTalentElement), ElementName = "TALENT")]
        public ArrayList dataList;

        public XMLTalentElement Get(int key)
        {
            XMLTalentElement e;
            if (Index.TryGetValue(key, out e))
            {
                return e;
            }

            return null;
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

        public XMLTalent()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, XMLTalentElement>();
        }

        [XmlInclude(typeof(XMLTalentElement))]
        public void Add(XMLTalentElement i)
        {
            dataList.Add(i);
        }

        public override bool Cooking()
        {
            _bPassed = true;

            foreach (XMLTalentElement talent in dataList)
            {
                if (!talent.Cooking()) _bPassed = false;
                Index[talent.id] = talent;
            }

            _strMessage = (_bPassed) ? "OK" : "ERROR";

            return _bPassed;
        }
    }
}
