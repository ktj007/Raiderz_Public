using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

namespace CombatMonitor.XML
{
    [Serializable]
    public sealed class XMLActAniTime : XMLObject
    {
        [XmlAttribute("weapontype")]
        public int weapontype;
        [XmlAttribute("act_animation_time")]
        public double act_animation_time;

        public override bool Cooking()
        {
            return true;
        }
    }

    [Serializable]
    public sealed class XMLTalentExtElement : XMLObject
    {
        private Dictionary<int, double> Index;

        [XmlElement(Type = typeof(XMLActAniTime), ElementName = "ActAniTime")]
        public ArrayList dataList;

        [XmlAttribute("id")]
        public int id;

        public XMLTalentExtElement()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, double>();
        }

        public override bool Cooking()
        {
            _bPassed = true;

            foreach (XMLActAniTime e in dataList)
            {
                if (!e.Cooking()) _bPassed = false;
                Index[e.weapontype] = e.act_animation_time;
            }

//            if (!idSpecified || name == null) return Error("id가 없거나 이름이 정해져있지 않은 탤런트");

            return _bPassed;
        }

        public double this[int nIndex]
        {
            get
            {
                double fRet = 0.0;
                if (!Index.TryGetValue(nIndex, out fRet))
                    return 0.0;
                return fRet;
            }
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class XMLTalentExt : XMLObject
    {
         private Dictionary<int, XMLTalentExtElement> Index;

        [XmlElement(Type = typeof(XMLTalentExtElement), ElementName = "TALENT")]
        public ArrayList dataList;
        
        public XMLTalentExtElement this[int nIndex]
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

        public XMLTalentExtElement this[string szIndex]
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

        public XMLTalentExt()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, XMLTalentExtElement>();
        }

        [XmlInclude(typeof(XMLTalentExtElement))]
        public void Add(XMLTalentExtElement i)
        {
            dataList.Add(i);
        }

        public override bool Cooking()
        {
            _bPassed = true;

            foreach (XMLTalentExtElement talent in dataList)
            {
                if (!talent.Cooking()) _bPassed = false;
                Index[talent.id] = talent;
            }

            _strMessage = (_bPassed) ? "OK" : "ERROR";

            return _bPassed;
        }
    }
}
