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
    #region SHDialogsXml

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHDialogsXml : SHXmlDataObject
    {
        [XmlElement(Type = typeof(SHDialog), ElementName = "DIALOG")]
        public ArrayList dataList;

        private Dictionary<int, SHDialog> Index;
        
        public SHDialog this[int nIndex]
        {
            get
            {
                try { return Index[nIndex]; }
                catch (Exception e) { throw e; }
            }
        }

        public SHDialog this[string szIndex]
        {
            get
            {
                try { return Index[Convert.ToInt32(szIndex)]; }
                catch (Exception e) { throw e; }
            }
        }

        public override bool IsValid(int nIndex)
        {
            return Index.ContainsKey(nIndex);
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

        public SHDialogsXml() : base()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, SHDialog>();
        }

        [XmlInclude(typeof(SHDialog))]
        public void Add(SHDialog i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            Sort();

            foreach (SHDialog dlg in dataList)
            {
                Index[dlg.id] = dlg;
                if (!dlg.Compile()) _bPassed = false;
            }

            _szCompiled = (_bPassed) ? SHGlobal.COK : SHGlobal.CERROR;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            foreach (SHDialog dlg in dataList)
            {
                dlg.Cook(e);
            }

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }

        public override string ToString()
        {
            return base.ToString();
        }

        class DialogComparer : IComparer
        {
            public DialogComparer()
            {
            }

            public int Compare(object x, object y)
            {
                SHDialog dlgFirst = (SHDialog)x;
                SHDialog dlgSecond = (SHDialog)y;

                return dlgFirst.id.CompareTo(dlgSecond.id);
            }
        }

        public override void Sort()
        {
            dataList.Sort(new DialogComparer());
        }
    }

    #endregion

    #region SHDialog

    [Serializable]
    public sealed class SHDialog : SHDataObject
    {
        private int _id;
        private int _exit;

        public SHDialog()
        {

        }

        [XmlElement("SAY")]
        public SHDialogSay Say;

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("대화문의 아이디값입니다. 이 값은 반드시 유일해야합니다."), DisplayName("(아이디)")]
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

        private string _text;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("대화를 시작하기 위한 대화를 설정합니다."), DisplayName("대화 시작문항"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string text
        {
            get { return _text; }
            set { _text = (value != String.Empty) ? value : null; }
        }

        private string _comment;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("주석을 설정 합니다."), DisplayName("주석"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("구체적인 대화 없이 텍스트 출력후 곧바로 종료하고 싶다면 이 값을 기입합니다."), DisplayName("반환값")]
        public int exit {
            get
            {
                return _exit;
            }
            set
            {
                exitSpecified = (value == 0) ? false : true;
                _exit = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool exitSpecified;

        public override bool Compile()
        {
            _szCompiled = SHGlobal.COK;
            _bPassed = true;

            if (text == null) return Error("text가 비어있습니다!");
            if (text != null && Say == null && !text.StartsWith("$$") && text.Length < 5) return Error(text + " (경고: 너무 짧은 문장!)");
            if (_bPassed && !exitSpecified && Say == null) return Error("오류가 존재하는 대화문. SAY로 시작되지 않으면서 DIALOG에 exit 코드가 없음.");
            if (_bPassed && Say != null && !Say.Compile()) return Error(Say.CompiledMessage);

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;
            String szNPC;

            ret = Properties.Resources.dialog_template.Replace("[[ID]]", _id.ToString());


            ret = ret.Replace("[[DIALOG_TEXT]]", core.GetSafeString(this.text));

            foreach (SHNPC npc in core.NPCs.dataList)
            {
                if (npc.Type.ToString().ToLower() == "monster") continue;

                string[] _iAct = npc.GetIAct();
                for (int i = 0; i < 10; i++)
                {
                    if (core.Dialogs.IsValid(_iAct[i]) && this.id == Int32.Parse(_iAct[i]))
                    {
                        szNPC = Properties.Resources.dialog_npc_template.Replace("[[NPC_NAME]]", npc.Name);

                        szNPC = szNPC.Replace("[[NPC_ID]]", npc.id.ToString());

                        ret += szNPC;

                        break;
                    }
                }
            }

            if (this.Say != null)
            {
                ret += this.Say.ToBody(core);
            }

            return ret;
        }

        public override string ToString()
        {
            string ret;

            if (exitSpecified)
            {
                if (exit != 0)
                {
                    ret = "EXIT(" + exit + ")";
                }
                else
                {
                    ret = "주의: EXIT가 0으로 명시되었습니다.";
                }
            } else {
                if (Say != null) ret = Say.text; else ret = _szCompiled;
            }

            return ret;
        }

        public void Cook(SHXmlCore core)
        {
        }
    }

    #endregion

    #region SHDialogSay

    [Serializable]
    public sealed class SHDialogSay : SHDataObject
    {
        [XmlElement(Type = typeof(SHDialogSelect), ElementName = "SELECT")]
        public ArrayList dataList;

        private string _text;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("NPC의 대화문입니다."), DisplayName("대화내용"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string text {
            get { return _text; }
            set { _text = (value != String.Empty) ? value : null; }
        }

        private string _comment;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("주석을 설정 합니다."), DisplayName("주석"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        public SHDialogSay()
        {
            dataList = new ArrayList();
        }

        [XmlInclude(typeof(SHDialogSelect))]
        public void Add(SHDialogSelect i)
        {
            dataList.Add(i);
        }

        public override bool Compile()
        {
            _bPassed = true;

            if (text != null && text.Length < 5 && !text.StartsWith("$$"))
            {
                _szCompiled = text + " (경고: 너무 짧은 문장!)";
                _bPassed = true;
            }

            if (_bPassed)
            {
                for (int i = 0; i < dataList.Count; i++)
                {
                    SHDialogSelect sel = (SHDialogSelect)dataList[i];
                    if (!sel.Compile())
                    {
                        _bPassed = false;
                        _szCompiled = sel.CompiledMessage;
                        break;
                    }
                }
                if (dataList.Count < 1)
                {
                    _bPassed = false;
                    _szCompiled = "대화문은 반드시 SELECT로 종료되어야 합니다. 이 대화문은 SAY에서 종결되었습니다.";
                }
            }

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            String ret = String.Empty;
            ret = SHWebUtil.ReplaceDialogTextForWeb(this.text);

            ret = Properties.Resources.dialog_preview_element_template.Replace("[[DIALOG_TEXT]]", ret);

            foreach (SHDialogSelect select in dataList)
            {
                if (select == null) continue;

                ret += select.ToBody(core);
            }

            return ret;
        }
    }

    #endregion

    #region SHDialogSelect

    [Serializable]
    public sealed class SHDialogSelect : SHDataObject
    {
        [XmlElement("ITEM")]
        public SHItemAddRemove Item;

        [XmlElement("SAY")]
        public SHDialogSay Say;

        private string _text;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("선택을 위한 텍스트를 설정합니다."), DisplayName("선택문"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string text
        {
            get { return _text; }
            set { _text = (value != String.Empty) ? value : null; }
        }

        private string _comment;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("주석을 설정 합니다."), DisplayName("주석"), Editor(typeof(VCETextEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string comment
        {
            get { return _comment; }
            set { _comment = (value != String.Empty) ? value : null; }
        }

        private int _exit;
        [XmlAttribute]
        [Category("기본"), DescriptionAttribute("선택을 하고나서 반환되는 값을 설정합니다. 만일 이 값이 0이라면, 이 선택문은 반환값을 반환하지 않는 것으로 간주됩니다."), DisplayName("반환값")]
        public int exit {
            get
            {
                return _exit; 
            }
            set
            {
                exitSpecified = (value == 0) ? false : true;
                _exit = value;
            }
        }
        [XmlIgnoreAttribute]
        public bool exitSpecified;

        public override bool Compile()
        {
            _bPassed = true;

            if (_bPassed && Say != null && !Say.Compile()) return Error(Say.CompiledMessage);

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            if (Say == null) return "";

            return Say.ToBody(core);
        }
    }
    #endregion
}
