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
    [Serializable, DefaultProperty("id")]
    public sealed class SHFieldNPC
    {
        private string[] _Type;
        private string[] _ICon;

        [XmlAttribute]
        public int id;
        [XmlAttribute]
        public string name;
        [XmlAttribute]
        public float x;
        [XmlAttribute]
        public float y;
        [XmlAttribute]
        public float z;
        [XmlAttribute]
        public string type1
        {
            get { return _Type[0]; }
            set { _Type[0] = value; }
        }
        [XmlAttribute]
        public string icon1
        {
            get { return _ICon[0]; }
            set { _ICon[0] = value; }
        }
        [XmlAttribute]
        public string type2
        {
            get { return _Type[1]; }
            set { _Type[1] = value; }
        }
        [XmlAttribute]
        public string icon2
        {
            get { return _ICon[1]; }
            set { _ICon[1] = value; }
        }

        public SHFieldNPC()
        {
            _Type = new string[2];
            _ICon = new string[2];
        }

    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHFieldNPCXml
    {
        [XmlElement(Type = typeof(SHFieldNPC), ElementName = "NPC")]
        public ArrayList dataList;

        public SHFieldNPCXml()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHFieldNPC))]
        public void Add(SHFieldNPC i)
        {
            dataList.Add(i);
        }
    }
}
