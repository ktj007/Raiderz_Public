using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

namespace CombatMonitor.XML
{
    [Serializable]
    public sealed class XMLTalentHitSegmentElement : XMLObject
    {
        public override bool Cooking()
        {
            return true;
        }
    }

    [Serializable]
    public sealed class XMLTalentHitElement : XMLObject
    {
        [XmlElement(Type = typeof(XMLTalentHitSegmentElement), ElementName = "HitSegment")]
        public ArrayList dataList;

        [XmlAttribute("id")]
        public int id = 0;

        public int SegmentQty = 0;

        public override bool Cooking()
        {
            SegmentQty = dataList.Count;
            return true;
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class XMLTalentHit : XMLObject
    {
        private Dictionary<int, XMLTalentHitElement> Index;

        [XmlElement(Type = typeof(XMLTalentHitElement), ElementName = "TALENT_HIT")]
        public ArrayList dataList;

        public XMLTalentHitElement Get(int key)
        {
            XMLTalentHitElement e;
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

        public XMLTalentHit()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, XMLTalentHitElement>();
        }

        [XmlInclude(typeof(XMLTalentHitElement))]
        public void Add(XMLTalentHitElement i)
        {
            dataList.Add(i);
        }

        public override bool Cooking()
        {
            _bPassed = true;

            foreach (XMLTalentHitElement talent in dataList)
            {
                if (!talent.Cooking()) _bPassed = false;
                Index[talent.id] = talent;
            }

            _strMessage = (_bPassed) ? "OK" : "ERROR";

            return _bPassed;
        }
    }
}
