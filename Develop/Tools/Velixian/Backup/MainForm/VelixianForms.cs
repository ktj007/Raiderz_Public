using System;
using System.Windows.Forms;
using System.Collections.Generic;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    public enum ValixianFormType
    {
        NPC             = 0,
        QUEST,
        CONDITION,
        ITEMCONDITION,
        ITEM,
        LOOTING,
        DIALOG,
        STRING,
        PAGE_MAX
    }

    public class VelixianForms
    {
        public Dictionary<Int32, VelixianForm> _TabPages;

        public VelixianForms()
        {
            _TabPages = new Dictionary<Int32, VelixianForm>();
        }

        public void Add(ToolStripComboBox cbFindType, EditForm _form, ValixianFormType editFormType, ListView lvMain, String keyword, bool bLoadingFailed, ToolStripMenuItem menu, TabInitializeHandler handler, TabSaveHandler save_handler)
        {
            int nIndex = (int)editFormType;

            VelixianForm newVellixianTab = new VelixianForm(_form, editFormType, lvMain, keyword, bLoadingFailed, menu, handler, save_handler);
            _TabPages.Add(nIndex, newVellixianTab);

            if (_form != null)
            {
                if (bLoadingFailed)
                {
                    _form.Text = _form.Text + "<Error>";
                }

                cbFindType.Items.Add(new VelixianFindType(_form.Text, _TabPages[nIndex]));
            }
        }

        public VelixianForm FindForm(String type)
        {
            String szType = type;
            VelixianForm MatchedTab = null;

            foreach (Int32 TabIndex in _TabPages.Keys)
            {
                MatchedTab = _TabPages[TabIndex];
                if (MatchedTab.Keyword == szType) break;
            }

            return MatchedTab;
        }

        public void ShowForm(String type)
        {
            String szType = type;
            VelixianForm MatchedTab = null;

            foreach (Int32 TabIndex in _TabPages.Keys)
            {
                MatchedTab = _TabPages[TabIndex];
                if (MatchedTab.Keyword == szType)
                {
                    Global._mainForm.ShowEditForm(MatchedTab);
                }
            }
        }

        public void ShowForm(int nIndex)
        {
            VelixianForm MatchedTab = null;

            if (nIndex < 0 || nIndex >= _TabPages.Keys.Count) return;

            MatchedTab = _TabPages[nIndex];
            Global._mainForm.ShowEditForm(MatchedTab);
        }

        public bool IsExistModifiedFiles()
        {
            for (int i = 0; i < (int)ValixianFormType.PAGE_MAX; i++)
            {
                if (_TabPages[i].Modified)
                {
                    return true;
                }
            }
            return false;
        }

    }



    public class VelixianForm
    {
        public EditForm form { get; set; }
        public ValixianFormType EditFormType { get; set; }
        public String Text { get; set; }
        public ListView MainList { get; set; }
        public String Keyword { get; set; }
        public ToolStripMenuItem Menu { get; set; }
        public TabInitializeHandler InitHandler { get; set; }
        public TabSaveHandler SaveHandler { get; set; }
        public bool LoadingFailed { get; set; }

        private Boolean _bModified = false;
        public Boolean Modified
        {
            get { return _bModified; }
            set { if (value) Updated = false; _bModified = value; }
        }
        public Boolean Updated { get; set; }

        public VelixianForm(EditForm _form, ValixianFormType editFormType, ListView lvMain, String keyword, bool bLoadingFailed, ToolStripMenuItem menu, TabInitializeHandler handler, TabSaveHandler save_handler)
        {
            form = _form;

            EditFormType = editFormType;
            Text = form.Text;
            MainList = lvMain;
            Keyword = keyword;
            Menu = menu;
            InitHandler = handler;
            SaveHandler = save_handler;
            LoadingFailed = bLoadingFailed;

            Modified = false;
            Updated = true;
        }


        /// <summary>
        /// 예외를 체크하여 저장한다.
        /// </summary>
        public void TrySave()
        {
            if (LoadingFailed) return;

            if (Modified && SaveHandler != null)
            {
                SaveHandler();
                form.Text = Text;
                Modified = false;

                if (!Updated && InitHandler != null)
                {
                    InitHandler();
                    Updated = true;
                }
            }
        }

        /// <summary>
        /// 이 탭에 연결된 문서가 수정되었음을 알리는 함수
        /// </summary>
        public void Touch()
        {
            if (LoadingFailed) return;

            Modified = true;
            form.Text = Text + "*";
        }

        /// <summary>
        /// 이 탭에 연결된 뷰를 업데이트한다.
        /// </summary>
        public void Update()
        {
            if (InitHandler != null) InitHandler();
            Updated = true;
        }
    }
}
