#region Using문
using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;
using cmlNET;

#endregion

namespace SoulHunt
{
    [Serializable]
    public sealed class SHString : SHDataObject
    {
        private string _String;

        [XmlAttribute("crc32")]
        public string Crc32 { get; set; }

        [XmlAttribute("key")]
        public string Key { get; set; }

        [XmlAttribute("string")]
        public string String 
        {
            get
            {
                return _String;
            }
            set
            {
                _String = value;

                String szCrc32 = "";
                if (_String != null)
                {
                    szCrc32 = _String.Trim(" ".ToCharArray());
                }
                Crc32 = GetCrc32(szCrc32);
            }
        }

        public override bool Compile()
        {
            _bPassed = true;

            if (Key == null || String == null) _bPassed = false;
            if (Key == null || Key == String.Empty) _szCompiled = "Key가 비어있습니다.";
            if (String == null || String == String.Empty) _szCompiled = "키 값 " + Key + "의 문자열이 비어있습니다.";

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

        public override string ToString()
        {
            return String;
        }

        private String GetCrc32(String value)
        {
            MCrc32 crc = new MCrc32();

            byte[] input = System.Text.Encoding.UTF8.GetBytes(value);
            crc.ComputeHash(input);

            return String.Format("{0:x8}", crc.CrcValue);
        }

        public void MakeCRC32()
        {
            String value = String.Trim(" ".ToCharArray());
            Crc32 = GetCrc32(value);
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHStringXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHString), ElementName = "STR")]
        public ArrayList dataList { get; set; }
        [XmlIgnoreAttribute]
        private Dictionary<string, SHString> Index;

        public override bool IsValid(string szIndex)
        {
            return Index.ContainsKey(szIndex);
        }

        public SHString this[string szKey]
        {
            get
            {
                try
                {
                    return Index[szKey];
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }

        public SHStringXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<string, SHString>();
        }

        [XmlInclude(typeof(SHString))]
        public void Add(SHString i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHString str in dataList)
            {
                if (!str.Compile()) _bPassed = false;
                Index[str.Key] = str;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

 	        return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return true;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            dataList.Clear();
        }
    }
}