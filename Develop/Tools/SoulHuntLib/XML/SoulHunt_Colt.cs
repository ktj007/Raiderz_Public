#region Includes

using System;
using System.ComponentModel;
using System.Linq;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

#endregion

namespace SoulHunt
{
    #region SHColtXml

    [Serializable]
    [DebuggerStepThroughAttribute()]
    [XmlTypeAttribute(AnonymousType=true)]
    [XmlRootAttribute("maiet", IsNullable=false)]
    public sealed class SHColtXml : SHDataObject
    {
        private XmlNode _scriptData;
        private SHColt _coltData;

        [XmlElement("COLT")]
        public SHColt Colt
        {
            get { return this._coltData; }
            set { this._coltData = value; }
        }

        [XmlElement("SCRIPT")]
        public XmlNode Script
        {
            get { return this._scriptData; }
            set { this._scriptData = value; }
        }

        [XmlIgnore]
        public String ScriptData
        {
            get { return _scriptData.Value; }
            set
            {
                XmlDocument xmlDoc = new XmlDocument();
                this._scriptData = xmlDoc.CreateCDataSection(value);
            }
        }

        public Boolean HasScriptSection()
        {
            return (_scriptData != null);
        }

        public override string ToBody(SHXmlCore core)
        {
            return String.Empty;
        }

        public override bool Compile()
        {
            return true;
        }

        public override bool Build(SHXmlCore core)
        {
            return true;
        }
    }

    #endregion

    #region SHColt
    [Serializable]
    [DebuggerStepThroughAttribute()]
    [DesignerCategoryAttribute("code")]
    public sealed class SHColt
    {
        [XmlElementAttribute("AGGRO", typeof(SHColtAggroSection))]
        [XmlElementAttribute("COMBAT", typeof(SHColtCombatSection))]
        [XmlElementAttribute("IDLE", typeof(SHColtIdleSection))]
        [XmlElementAttribute("REACTION", typeof(SHColtReactionSection))]
        [XmlElementAttribute("STRESS", typeof(SHColtStressSection))]
        [XmlElementAttribute("VICTORY", typeof(SHColtVictorySection))]
        public ArrayList dataList;

        public SHColt()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHColtAggroSection))]
        public void Add(SHColtAggroSection i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtCombatSection))]
        public void Add(SHColtCombatSection i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtIdleSection))]
        public void Add(SHColtIdleSection i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtReactionSection))]
        public void Add(SHColtReactionSection i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtStressSection))]
        public void Add(SHColtStressSection i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtVictorySection))]
        public void Add(SHColtVictorySection i)
        {
            dataList.Add(i);
        }
    }
    #endregion

    #region SHColtSection - Each Parent Section
    public class SHColtSection
    {
        [XmlIgnore]
        protected SHColtSectionType _Type = SHColtSectionType.COMBAT;

        public void SetSectionType(SHColtSectionType SectionType)
        {
            _Type = SectionType;
        }

        public SHColtSectionType GetSectionType()
        {
            return _Type;
        }
    }
    #endregion
    
    #region SHColtCombatSection - Core Section
    [Serializable]
    public partial class SHColtCombatSection : SHColtSection
    {
        [XmlElementAttribute("CHECK", typeof(SHColtCheck))]
        [XmlElementAttribute("DEFAULT", typeof(SHColtDefClause))]
        public ArrayList dataList;

        private int tackleField;
        private bool tackleFieldSpecified;
        private int tackle_rateField;
        private bool tackle_rateFieldSpecified;
        private int rotateField;
        private bool rotateFieldSpecified;
        private int victoryField;
        private bool victoryFieldSpecified;

        public SHColtCombatSection()
        {
            _Type = SHColtSectionType.COMBAT;
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHColtCheck))]
        public void Add(SHColtCheck i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtDefClause))]
        public void Add(SHColtDefClause i)
        {
            dataList.Add(i);
        }

        [XmlAttributeAttribute()]
        public int tackle
        {
            get
            {
                return this.tackleField;
            }
            set
            {
                this.tackleField = value;
            }
        }

        [XmlIgnore]
        public bool tackleSpecified
        {
            get
            {
                return this.tackleFieldSpecified;
            }
            set
            {
                this.tackleFieldSpecified = value;
            }
        }

        [XmlAttributeAttribute()]
        public int tackle_rate
        {
            get
            {
                return this.tackle_rateField;
            }
            set
            {
                this.tackle_rateField = value;
            }
        }

        [XmlIgnore]
        public bool tackle_rateSpecified
        {
            get
            {
                return this.tackle_rateFieldSpecified;
            }
            set
            {
                this.tackle_rateFieldSpecified = value;
            }
        }

        [XmlAttributeAttribute()]
        public int rotate
        {
            get
            {
                return this.rotateField;
            }
            set
            {
                this.rotateField = value;
            }
        }

        [XmlIgnore]
        public bool rotateSpecified
        {
            get
            {
                return this.rotateFieldSpecified;
            }
            set
            {
                this.rotateFieldSpecified = value;
            }
        }

        [XmlAttributeAttribute()]
        public int victory
        {
            get
            {
                return this.victoryField;
            }
            set
            {
                this.victoryField = value;
            }
        }

        [XmlIgnore]
        public bool victorySpecified
        {
            get
            {
                return this.victoryFieldSpecified;
            }
            set
            {
                this.victoryFieldSpecified = value;
            }
        }
    }

    #endregion

    #region COMBAT Section - SHColtEventSection/SHColtAggroSection/SHColtIdleSection/SHColtReactionSection/SHColtStressSection/SHColtVictorySection

    [Serializable]
    [DebuggerStepThroughAttribute()]
    [DesignerCategoryAttribute("code")]
    public class SHColtEventSection : SHColtSection
    {
        [XmlElement(Type = typeof(SHColtCheck), ElementName = "CHECK")]
        [XmlElement(Type = typeof(SHColtDefClause), ElementName = "DEFAULT")]
        public ArrayList dataList;

        public SHColtEventSection()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHColtCheck))]
        public void Add(SHColtCheck i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtDefClause))]
        public void Add(SHColtDefClause i)
        {
            dataList.Add(i);
        }
    }

    [Serializable]
    public sealed class SHColtAggroSection : SHColtSection
    {
        public SHColtAggroSection() { _Type = SHColtSectionType.AGGRO; }
    }

    [Serializable]
    public sealed class SHColtIdleSection : SHColtSection
    {
        public SHColtIdleSection() { _Type = SHColtSectionType.IDLE; }
    }

    [Serializable]
    public sealed class SHColtReactionSection : SHColtSection
    {
        public SHColtReactionSection() { _Type = SHColtSectionType.REACTION; }
    }

    [Serializable]
    public sealed class SHColtStressSection : SHColtSection
    {
        public SHColtStressSection() { _Type = SHColtSectionType.STRESS; }
    }

    [Serializable]
    public sealed class SHColtVictorySection : SHColtSection
    {
        public SHColtVictorySection() { _Type = SHColtSectionType.VICTORY; }
    }

    #endregion

    #region ACTION Definition(Embedded Action List Supports to.)

    [Serializable]
    public partial class SHColtAction {
        [XmlElement(Type = typeof(SHColtSequencedAction), ElementName = "ACTION")]
        public ArrayList Actions;

        private SHColtActionType typeField;
        private bool typeFieldSpecified;
        private string param1Field;
        private string param2Field;
        private string param3Field;
        private bool stressField;
        private double durationField;
        private bool durationFieldSpecified;
        private int max_countField;
        private bool max_countFieldSpecified;
        private int rateField;
        private bool rateFieldSpecified;
        private bool auto_runField;
        private bool auto_runFieldSpecified;
        private int cycleField;
        private bool cycleFieldSpecified;
        
        public SHColtAction() {
            Actions = new ArrayList();
            this.stressField = true;
        }

        [XmlInclude(typeof(SHColtSequencedAction))]
        public void Add(SHColtSequencedAction i)
        {
            Actions.Add(i);
        }
        
        [XmlAttributeAttribute()]
        public SHColtActionType type {
            get {
                return this.typeField;
            }
            set {
                this.typeField = value;
            }
        }
        
        [XmlIgnore]
        public bool typeSpecified {
            get {
                return this.typeFieldSpecified;
            }
            set {
                this.typeFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param1 {
            get {
                return this.param1Field;
            }
            set {
                this.param1Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param2 {
            get {
                return this.param2Field;
            }
            set {
                this.param2Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param3 {
            get {
                return this.param3Field;
            }
            set {
                this.param3Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        [DefaultValueAttribute(true)]
        public bool stress {
            get {
                return this.stressField;
            }
            set {
                this.stressField = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public double duration {
            get {
                return this.durationField;
            }
            set {
                this.durationField = value;
            }
        }
        
        [XmlIgnore]
        public bool durationSpecified {
            get {
                return this.durationFieldSpecified;
            }
            set {
                this.durationFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public int max_count {
            get {
                return this.max_countField;
            }
            set {
                this.max_countField = value;
            }
        }
        
        [XmlIgnore]
        public bool max_countSpecified {
            get {
                return this.max_countFieldSpecified;
            }
            set {
                this.max_countFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public int rate {
            get {
                return this.rateField;
            }
            set {
                this.rateField = value;
            }
        }
        
        [XmlIgnore]
        public bool rateSpecified {
            get {
                return this.rateFieldSpecified;
            }
            set {
                this.rateFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public bool auto_run {
            get {
                return this.auto_runField;
            }
            set {
                this.auto_runField = value;
            }
        }
        
        [XmlIgnore]
        public bool auto_runSpecified {
            get {
                return this.auto_runFieldSpecified;
            }
            set {
                this.auto_runFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public int cycle {
            get {
                return this.cycleField;
            }
            set {
                this.cycleField = value;
            }
        }
        
        [XmlIgnore]
        public bool cycleSpecified {
            get {
                return this.cycleFieldSpecified;
            }
            set {
                this.cycleFieldSpecified = value;
            }
        }
    }
    
    [Serializable]
    public partial class SHColtSequencedAction {
        private SHColtActionType typeField;
        private bool typeFieldSpecified;
        private string param1Field;
        private string param2Field;
        private string param3Field;
        private bool stressField;
        private double durationField;
        private bool durationFieldSpecified;
        
        public SHColtSequencedAction() {
            this.stressField = true;
        }
        
        [XmlAttributeAttribute()]
        public SHColtActionType type {
            get {
                return this.typeField;
            }
            set {
                this.typeField = value;
            }
        }
        
        [XmlIgnore]
        public bool typeSpecified {
            get {
                return this.typeFieldSpecified;
            }
            set {
                this.typeFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param1 {
            get {
                return this.param1Field;
            }
            set {
                this.param1Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param2 {
            get {
                return this.param2Field;
            }
            set {
                this.param2Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param3 {
            get {
                return this.param3Field;
            }
            set {
                this.param3Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        [DefaultValueAttribute(true)]
        public bool stress {
            get {
                return this.stressField;
            }
            set {
                this.stressField = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public double duration {
            get {
                return this.durationField;
            }
            set {
                this.durationField = value;
            }
        }
        
        [XmlIgnore]
        public bool durationSpecified {
            get {
                return this.durationFieldSpecified;
            }
            set {
                this.durationFieldSpecified = value;
            }
        }
    }
    
    [Serializable]
    public enum SHColtActionType {
        nothing,
        talent,
        lua,
        flee,
        yell,
        guard,
        breakpart,
        adjust,
        damage,
        motionfactor,
        distance,
        follow,
        move,
        move_random,
        change_hitcapsule,
        say,
        shout,
        narration,
        aggro,
        group,
    }

    #endregion

    #region SHColtDefClause - DEFAULT Section(Clause)For definitions
    [Serializable]
    public partial class SHColtDefClause
    {
        [XmlElement(Type = typeof(SHColtAction), ElementName = "ACTION"), XmlElement(Type = typeof(SHColtCheck), ElementName = "CHECK")]
        public ArrayList dataList;

        public SHColtDefClause()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHColtAction))]
        public void Add(SHColtAction i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtCheck))]
        public void Add(SHColtCheck i)
        {
            dataList.Add(i);
        }
    }
    #endregion

    #region SHColtCheck - Initial Check
    [Serializable]
    public sealed class SHColtCheck {
        [XmlElementAttribute("ACTION", typeof(SHColtAction))]
        [XmlElementAttribute("CHECK", typeof(SHColtCheck))]
        public ArrayList dataList;

        private SHColtCheckType typeField;
        private bool typeFieldSpecified;
        private string param1Field;
        private string param2Field;

        public SHColtCheck()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHColtAction))]
        public void Add(SHColtAction i)
        {
            dataList.Add(i);
        }

        [XmlInclude(typeof(SHColtCheck))]
        public void Add(SHColtCheck i)
        {
            dataList.Add(i);
        }

        [XmlAttributeAttribute()]
        public SHColtCheckType type {
            get {
                return this.typeField;
            }
            set {
                this.typeField = value;
            }
        }
        
        [XmlIgnore]
        public bool typeSpecified {
            get {
                return this.typeFieldSpecified;
            }
            set {
                this.typeFieldSpecified = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param1 {
            get {
                return this.param1Field;
            }
            set {
                this.param1Field = value;
            }
        }
        
        [XmlAttributeAttribute()]
        public string param2 {
            get {
                return this.param2Field;
            }
            set {
                this.param2Field = value;
            }
        }
    }

    [Serializable]
    public enum SHColtCheckType
    {
        hp,
        en,
        sp,
        stress,
        breakpart,
        unbreakpart,
        damage,
        motionfactor,
        distance,
        angle,
        e_motionfactor,
        mode,
        state,
        hitcapsule,
        used_talent,
    }

    #endregion

    [Serializable]
    [XmlTypeAttribute(IncludeInSchema=false)]
    public enum SHColtSectionType {
        AGGRO,        
        COMBAT,        
        IDLE,        
        REACTION,        
        STRESS,        
        VICTORY,
    }
}
