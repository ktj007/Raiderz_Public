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
    public sealed class XMLItemElement : XMLObject
    {
        [XmlAttribute("id")]
        public int id = 0;

        [XmlAttribute("name")]
        public string name = "";

        public override bool Cooking()
        {
            return true;
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class XMLItem : XMLObject
    {
        private Dictionary<int, XMLItemElement> Index;

        [XmlElement(Type = typeof(XMLItemElement), ElementName = "ITEM")]
        public ArrayList dataList;

        public XMLItemElement Get(int key)
        {
            XMLItemElement e;
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

        public XMLItem()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, XMLItemElement>();
        }

        [XmlInclude(typeof(XMLItemElement))]
        public void Add(XMLItemElement i)
        {
            dataList.Add(i);
        }

        public override bool Cooking()
        {
            _bPassed = true;

            foreach (XMLItemElement talent in dataList)
            {
                if (!talent.Cooking()) _bPassed = false;
                Index[talent.id] = talent;
            }

            _strMessage = (_bPassed) ? "OK" : "ERROR";

            return _bPassed;
        }
    }
}

