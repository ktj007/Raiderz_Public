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
    #region SHField
    [Serializable, DefaultProperty("id")]
    public sealed class SHField : SHDataObject
    {
        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("필드의 아이디. 이 값은 유일해야합니다. 단, 본 프로그램은 이에 관련된 유효성 검사를 하지 않습니다."), DisplayName("(아이디)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("필드의 이름"), DisplayName("(필드이름)")]
        public string name { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("생성채널 수."), DisplayName("채널 갯수")]
        public int default_channel { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("."), DisplayName("")]
        public bool validation { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("."), DisplayName("")]
        public bool ailod_enable { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("."), DisplayName("")]
        public bool show_map{ get; set; }

        public override bool Compile()
        {
            _bPassed = true;

            //foreach (SHCraft item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            //_szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            // foreach (SHCraft item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

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
    public sealed class SHFieldListXml : SHDataObject
    {
        [XmlElement("FIELDLIST")]
        public SHFieldList fieldlist;

        [XmlElement("FIELDLIST_DEVELOP")]
        public SHFieldList fieldlist_dev;

        public ArrayList dataList;

        private Dictionary<int, SHField> Index;

        public SHField this[int nIndex]
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

        public SHField this[string szIndex]
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

        public SHFieldListXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHField>();
        }

        //[XmlInclude(typeof(SHFieldList))]
        //public void Add(SHFieldList i)
        //{
        //    dataList.Add(i);
        //}

        public override bool Compile()
        {
            _bPassed = true;

            if (null != fieldlist)
            {
                fieldlist.Compile();
                foreach (SHField field in fieldlist.dataList)
                {
                    if (field.Compile())
                    {
                        Index[field.id] = field;
                        dataList.Add(field);
                    }
                    else _bPassed = false;
                }

                foreach (SHFieldGroup fieldgroup in fieldlist.groupdataList)
                {
                    foreach (SHField field in fieldgroup.dataList)
                    {
                        if (field.Compile())
                        {
                            Index[field.id] = field;
                            dataList.Add(field);
                        }
                        else _bPassed = false;
                    }
                }
            }

            if (null != fieldlist_dev)
            {
                fieldlist_dev.Compile();

                foreach (SHField field in fieldlist_dev.dataList)
                {
                    if (field.Compile())
                    {
                        Index[field.id] = field;
                        dataList.Add(field);
                    }
                    else _bPassed = false;
                }

                foreach (SHFieldGroup fieldgroup in fieldlist_dev.groupdataList)
                {
                    foreach (SHField field in fieldgroup.dataList)
                    {
                        if (field.Compile())
                        {
                            Index[field.id] = field;
                            dataList.Add(field);
                        }
                        else _bPassed = false;
                    }
                }
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            //foreach (SHCraft item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public ArrayList GetArrayList()
        {
            try
            {
                return dataList;
            }
            catch
            {
                return null;
            }
        }
    }

    #region SHFieldList
    [Serializable]
    public sealed class SHFieldList : SHDataObject
    {
        [XmlElement(Type = typeof(SHField), ElementName = "FIELD")]
        public ArrayList dataList;

        [XmlElement(Type = typeof(SHFieldGroup), ElementName = "FIELDGROUP")]
        public ArrayList groupdataList;

        private Dictionary<int, SHField> Index;

        public SHField this[int nIndex]
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

        public SHField this[string szIndex]
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

        public SHFieldList() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHField>();
        }

        [XmlInclude(typeof(SHField))]
        public void Add(SHField i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHField item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;

            foreach (SHFieldGroup fieldgroup in groupdataList)
            {
                foreach (SHField field in fieldgroup.dataList)
                {
                    if (field.Compile()) Index[field.id] = field;
                    else _bPassed = false;
                }
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            // foreach (SHCraft item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
    #endregion

    #region SHFieldGroup
    [Serializable]
    public sealed class SHFieldGroup : SHDataObject
    {
        [XmlElement(Type = typeof(SHField), ElementName = "FIELD")]
        public ArrayList dataList;
 
        private Dictionary<int, SHField> Index;

        private int _id;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("필드의 아이디. 이 값은 유일해야합니다. 단, 본 프로그램은 이에 관련된 유효성 검사를 하지 않습니다."), DisplayName("(아이디)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("필드의 이름"), DisplayName("(필드이름)")]
        public int start_field { get; set; }
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("시작 마커"), DisplayName("(시작 마커)")]
        public int start_marker { get; set; }

        public SHField this[int nIndex]
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

        public SHField this[string szIndex]
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

        public SHFieldGroup() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHField>();
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach (SHField item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            //_szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            // foreach (SHCraft item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
    #endregion
}
