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
    [Serializable, DefaultProperty("recipe")]
    public sealed class SHRecipeNPC : SHDataObject
    {
        private string[] _NPC;
        private string[] _FIELD;

        [XmlAttribute]
        public int recipe;
        
        [XmlAttribute]
        public string NPC1
        {
            get { return _NPC[0]; }
            set { _NPC[0] = value; }
        }
        [XmlAttribute]
        public string field1
        {
            get { return _FIELD[0]; }
            set { _FIELD[0] = value; }
        }

        [XmlAttribute]
        public string NPC2
        {
            get { return _NPC[1]; }
            set { _NPC[1] = value; }
        }
        [XmlAttribute]
        public string field2
        {
            get { return _FIELD[1]; }
            set { _FIELD[1] = value; }
        }

        [XmlAttribute]
        public string NPC3
        {
            get { return _NPC[2]; }
            set { _NPC[2] = value; }
        }
        [XmlAttribute]
        public string field3
        {
            get { return _FIELD[2]; }
            set { _FIELD[2] = value; }
        }

        [XmlAttribute]
        public string NPC4
        {
            get { return _NPC[3]; }
            set { _NPC[3] = value; }
        }
        [XmlAttribute]
        public string field4
        {
            get { return _FIELD[3]; }
            set { _FIELD[3] = value; }
        }

        [XmlAttribute]
        public string NPC5
        {
            get { return _NPC[4]; }
            set { _NPC[4] = value; }
        }
        [XmlAttribute]
        public string field5
        {
            get { return _FIELD[4]; }
            set { _FIELD[4] = value; }
        }

        [XmlAttribute]
        public string NPC6
        {
            get { return _NPC[5]; }
            set { _NPC[5] = value; }
        }
        [XmlAttribute]
        public string field6
        {
            get { return _FIELD[5]; }
            set { _FIELD[5] = value; }
        }

        [XmlAttribute]
        public string NPC7
        {
            get { return _NPC[6]; }
            set { _NPC[6] = value; }
        }
        [XmlAttribute]
        public string field7
        {
            get { return _FIELD[6]; }
            set { _FIELD[6] = value; }
        }

        [XmlAttribute]
        public string NPC8
        {
            get { return _NPC[7]; }
            set { _NPC[7] = value; }
        }
        [XmlAttribute]
        public string field8
        {
            get { return _FIELD[7]; }
            set { _FIELD[7] = value; }
        }

        [XmlAttribute]
        public string NPC9
        {
            get { return _NPC[8]; }
            set { _NPC[8] = value; }
        }
        [XmlAttribute]
        public string field9
        {
            get { return _FIELD[8]; }
            set { _FIELD[8] = value; }
        }

        [XmlAttribute]
        public string NPC10
        {
            get { return _NPC[9]; }
            set { _NPC[9] = value; }
        }
        [XmlAttribute]
        public string field10
        {
            get { return _FIELD[9]; }
            set { _FIELD[9] = value; }
        }

        public SHRecipeNPC()
        {
            _NPC = new string[10];
            _FIELD = new string[10];
        }
    
        public void Add(int Index, string NPCID, string FieldID)
        {
            _NPC[Index] = NPCID;
            _FIELD[Index] = FieldID;
        }

        public override bool Compile()
        {
            _bPassed = true;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHRecipeNPCXml : SHDataObject
    {
        [XmlElement(Type = typeof(SHRecipeNPC), ElementName = "RECIPENPC")]
        public ArrayList dataList;

        private Dictionary<int, SHRecipeNPC> Index;

        public SHRecipeNPC this[int nIndex]
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

        public SHRecipeNPC this[string szIndex]
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

        public SHRecipeNPCXml()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHRecipeNPC>();
        }

        [XmlInclude(typeof(SHRecipeNPC))]
        public void Add(SHRecipeNPC i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach( SHRecipeNPC npc in dataList)
            {
                if (npc.Compile()) Index[npc.recipe] = npc; else _bPassed = false;
            }

             return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}
