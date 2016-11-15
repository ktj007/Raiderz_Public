#region Includes
using System;
using System.ComponentModel;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;
#endregion

namespace SoulHunt
{
    #region SHItem
    [Serializable, DefaultProperty("id")]
    public sealed class SHItem : SHDataObject
    {
        private int _id;

        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("ID Value."), DisplayName("(ID)")]
        public int id
        {
            get { return _id; }
            set { if (value == 0) idSpecified = false; else idSpecified = true; _id = value; }
        }
        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("The name of the item."), DisplayName("(Name)")]
        public string name { get; set; }
        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("The Item's Description."), DisplayName("Desc"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string desc { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("The name of the mesh used."), DisplayName("(Mesh Name)")]
        public string mesh_name { get; set; }
        [XmlAttribute, Category("Model Info"), DescriptionAttribute("Irrelevant."), DisplayName("(Ignore)")]
        [TypeConverter(typeof(SHCT_BoolConverter))]
        public string mesh_name_by_sex { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("First node name."), DisplayName("Node 1 Name")]
        public string node_name { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("Second node name"), DisplayName("Node 2 Name")]
        public string node_name2 { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("Path of the mesh (category\name"), DisplayName("Mesh Path")]
        public string mesh_path { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("Parent Node Name"), DisplayName("Parent")]
        public string parent_name { get; set; }
        [XmlAttribute]
        [Category("Model Info"), DescriptionAttribute("Texture Color Code"), DisplayName("Texture Color")]
        public string texcolor { get; set; }
        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("The Item Type 0-NA 1-WPN 2-ARMR 3-HSE 4-USE 5-AMM 6-INVEN 7-GATH 8-ENST 9-DYE 10-ATNST 11-COST"), DisplayName("(Type)")]
        public string type { get; set; }
        
        private int _condition;
        [XmlAttribute]
        [Category("Basic Info"), DescriptionAttribute("The Condition Info"), DisplayName("Condition")]
        public int condition
        {
            get { return _condition; }
            set { conditionSpecified = (value != 0) ? true : false; _condition = value; }
        }
        [XmlIgnoreAttribute]
        public bool conditionSpecified;


        [XmlAttribute]
        public string usable_type;
        [XmlAttribute]
        public string usable_param;


        [XmlAttribute]
        public string sex { get; set; }
        [XmlAttribute]
        public string slot { get; set; }
        [XmlAttribute]
        public string sub_slot { get; set; }
        [XmlAttribute]
        public string weapon_type { get; set; }
        [XmlAttribute]
        public string equipment_type { get; set; }
        [XmlAttribute]
        public string questitem { get; set; }
        [XmlAttribute]
        public string innitem_npc { get; set; }
        [XmlAttribute]
        public string claim_required { get; set; }
        [XmlAttribute]
        public string tradable { get; set; }
        [XmlAttribute]
        public string sellable { get; set; }
        [XmlAttribute]
        public string unique { get; set; }

        private int _buyprice;
        [XmlAttribute]
        public int buyprice {
            get { return _buyprice; }
            set { buypriceSpecified = (value != 0) ? true : false; _buyprice = value; }
        }
        [XmlIgnore]
        public bool buypriceSpecified;

        private int _sellprice;
        [XmlAttribute]
        public int sellprice
        {
            get { return _sellprice; }
            set { sellpriceSpecified = (value != 0) ? true : false; _sellprice = value; }
        }
        [XmlIgnoreAttribute]
        public bool sellpriceSpecified;

        private int _stacksize;
        [XmlAttribute]
        public int stacksize
        {
            get { return _stacksize; }
            set { stacksizeSpecified = (value != 0) ? true : false; _stacksize = value; }
        }
        [XmlIgnoreAttribute]
        public bool stacksizeSpecified;

        private int _weight;
        [XmlAttribute]
        public int weight
        {
            get { return _weight; }
            set { weightSpecified = (value != 0) ? true : false; _weight = value; }
        }
        [XmlIgnoreAttribute]
        public bool weightSpecified;

        private int _max_durability;
        [XmlAttribute]
        public int max_durability
        {
            get { return _max_durability; }
            set { max_durabilitySpecified = (value != 0) ? true : false; _max_durability = value; }
        }
        [XmlIgnoreAttribute]
        public bool max_durabilitySpecified;

        private int _container_depth;
        [XmlAttribute]
        public int container_depth
        {
            get { return _container_depth; }
            set { container_depthSpecified = (value != 0) ? true : false; _container_depth = value; }
        }
        [XmlIgnoreAttribute]
        public bool container_depthSpecified;

        private int _min_damage;
        [XmlAttribute]
        public int min_damage
        {
            get { return _min_damage; }
            set { min_damageSpecified = (value != 0) ? true : false; _min_damage = value; }
        }
        [XmlIgnoreAttribute]
        public bool min_damageSpecified;

        private int _max_damage;
        [XmlAttribute]
        public int max_damage
        {
            get { return _max_damage; }
            set { max_damageSpecified = (value != 0) ? true : false; _max_damage = value; }
        }
        [XmlIgnoreAttribute]
        public bool max_damageSpecified;

        private int _AP;
        [XmlAttribute]
        public int AP
        {
            get { return _AP; }
            set { APSpecified = (value != 0) ? true : false; _AP = value; }
        }
        [XmlIgnoreAttribute]
        public bool APSpecified;

        private double _craft_optionrate;
        [XmlAttribute]
        public double craft_optionrate
        {
            get { return _craft_optionrate; }
            set { craft_optionrateSpecified = (value != 0) ? true : false; _craft_optionrate = value; }
        }
        [XmlIgnoreAttribute]
        public bool craft_optionrateSpecified;

        private int _use_talent;
        [XmlAttribute]
        public int use_talent
        {
            get { return _use_talent; }
            set { use_talentSpecified = (value != 0) ? true : false; _use_talent = value; }
        }
        [XmlIgnoreAttribute]
        public bool use_talentSpecified;

        private int _use_quest;
        [XmlAttribute]
        public int use_quest
        {
            get { return _use_quest; }
            set { use_questSpecified = (value != 0) ? true : false; _use_quest = value; }
        }
        [XmlIgnoreAttribute]
        public bool use_questSpecified;

        [XmlAttribute]
        public string use_ani { get; set; }
        [XmlAttribute]
        public string use_fx { get; set; }
        [XmlAttribute]
        public string SwordTrailEffect { get; set; }
        [XmlAttribute]
        public string soul_type { get; set; }

        private int _soul_maxquantity;
        [XmlAttribute]
        public int soul_maxquantity
        {
            get { return _soul_maxquantity; }
            set { soul_maxquantitySpecified = (value != 0) ? true : false; _soul_maxquantity = value; }
        }
        [XmlIgnoreAttribute]
        public bool soul_maxquantitySpecified;

        [XmlAttribute]
        public string soul_stoneitem { get; set; }

        public SHItem()
        {
            mesh_name_by_sex = "False";
        }

        public override bool Compile()
        {
            _bPassed = (idSpecified && (name != null)) ? true : false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            if (conditionSpecified && !e.ItemConditions.IsValid(condition)) return Error("Invalid Condition. (Condition(ITEM) ID:" + condition.ToString() + ")");
            if (use_talentSpecified && !e.Talents.IsValid(use_talent)) return Error("Invalud Talent. (ID:" + use_talent.ToString() + ")");
            if (use_questSpecified && !e.Quests.IsValid(use_quest)) return Error("Invalid Quest. (ID:" + use_quest.ToString() + ")");

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
    public sealed class SHItemsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHItem), ElementName = "ITEM")]
        public ArrayList dataList;
        
        private Dictionary<int, SHItem> Index;

        public SHItem this[int nIndex]
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

        public SHItem this[string szIndex]
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

        public override bool IsValid(String szIndex)
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

        public override bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
        }

        public SHItemsXml():base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHItem>();
        }

        [XmlInclude(typeof(SHItem))]
        public void Add(SHItem i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            foreach(SHItem item in dataList) if (item.Compile()) Index[item.id] = item; else _bPassed = false;
            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            foreach (SHItem item in dataList) if (!item.Build(e)) return Error(item.CompiledMessage);

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}