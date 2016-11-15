#region Using문

using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace SoulHunt
{
    #region SHItemAddRemoveType
    [Serializable]
    public enum SHItemAddRemoveType
    {
        ADD,
        REMOVE,
        FILL
    }
    #endregion


    #region SHItemAddRemove
    [Serializable]
    public sealed class SHItemAddRemove : ICloneable
    {
        [XmlElement(Type = typeof(SHItemAdd), ElementName = "ADD"), XmlElement(Type = typeof(SHItemRemove), ElementName = "REMOVE"), XmlElement(Type = typeof(SHItemFill), ElementName = "FILL")]
        public ArrayList dataList;

        public SHItemAddRemove()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHItemAdd))]
        public void Add(SHItemAdd i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHItemRemove))]
        public void Add(SHItemRemove i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHItemFill))]
        public void Add(SHItemFill i)
        {
            dataList.Add(i);
        }
        public static SHItemAddRemoveBase NewItem(SHItemAddRemoveType nType)
        {
            switch (nType)
            {
                case SHItemAddRemoveType.ADD: return new SHItemAdd();
                case SHItemAddRemoveType.REMOVE: return new SHItemRemove();
                case SHItemAddRemoveType.FILL: return new SHItemFill();
            }
            return null;
        }

        public object Clone()
        {
            SHItemAddRemove clone = new SHItemAddRemove();

            foreach (SHItemAddRemoveBase obj in dataList)
            {
                clone.dataList.Add((SHItemAddRemoveBase)obj.Clone());
            }

            return clone;
        }
        
    }
    #endregion

    public class SHItemAddRemoveBase : ICloneable
    {
        private int _id;
        private int _amount;

        [XmlAttribute]
        public int id
        {
            get
            {
                return _id;
            }
            set
            {
                idSpecified = true;
                _id = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool idSpecified;

        [XmlAttribute]
        public int amount
        {
            get
            {
                return _amount;
            }
            set
            {
                amountSpecified = true;
                _amount = value;
            }
        }

        [XmlIgnoreAttribute]
        public bool amountSpecified;

        public virtual SHItemAddRemoveType GetItemType() { return SHItemAddRemoveType.ADD; }

        public virtual SHItemAddRemoveBase NewThis() { return new SHItemAddRemoveBase(); }
        public virtual object Clone()
        {
            SHItemAddRemoveBase clone = NewThis();

            clone._id = this._id;
            clone.idSpecified = this.idSpecified;

            clone._amount = this._amount;
            clone.amountSpecified = this.amountSpecified;

            return clone;
        }

    }

    #region SHItemAdd
    [Serializable]
    public sealed class SHItemAdd : SHItemAddRemoveBase
    {
        public override SHItemAddRemoveType GetItemType() { return SHItemAddRemoveType.ADD; }
        public override SHItemAddRemoveBase NewThis() { return new SHItemAdd(); }
    }
    #endregion

    #region SHItemRemove
    [Serializable]
    public sealed class SHItemRemove : SHItemAddRemoveBase
    {
        public override SHItemAddRemoveType GetItemType() { return SHItemAddRemoveType.REMOVE; }
        public override SHItemAddRemoveBase NewThis() { return new SHItemRemove(); }
    }
    #endregion

    #region SHItemFill
    [Serializable]
    public sealed class SHItemFill : SHItemAddRemoveBase
    {
        public override SHItemAddRemoveType GetItemType() { return SHItemAddRemoveType.FILL; }
        public override SHItemAddRemoveBase NewThis() { return new SHItemFill(); }
    }
    #endregion
}
