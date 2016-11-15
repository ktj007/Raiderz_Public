using System;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Collections;
using SoulHunt;

namespace Velixian
{
    /// <summary>
    /// 여기에는 HTML에서 사용할 내부 객체들을 정의한다.
    /// </summary>
    public partial class MainForm
    {
        /// <summary>
        /// type으로 지정된 탭을 활성화시킨다.
        /// </summary>
        public void Activate(String type)
        {
            Global._VelixianForms.ShowForm(type);
        }

        /// <summary>
        /// 자바스크립트 호출용 인터페이스
        /// </summary>
        /// <param name="type">탭페이지 Keyword</param>
        /// <param name="key">검색할 ID를 문자열로 담은 것</param>

        public void Navigate(String type, String id)
        {
            String szType = type.ToUpper();
            Boolean bFound = false;
            VelixianForm MatchedTab = null;

            foreach (Int32 TabIndex in _Forms._TabPages.Keys)
            {
                MatchedTab = _Forms._TabPages[TabIndex];
                if (MatchedTab.Keyword == szType)
                {
                    MatchedTab.form.Show(dockPanel);

                    bFound = true;
                    break;
                }
            }

            if (bFound)
            {
                ListView searchView = MatchedTab.MainList;
                Int32 nSearchIndex = 0;
                ListViewItem lvi = null;

                while ((lvi = searchView.FindItemWithText(id, false, 0, true)) != null)
                {
                    if (lvi.Text == id) break;
                    nSearchIndex = lvi.Index;
                    lvi = null;
                }

                if (lvi != null)
                {
                    SHListViewUtil.SelectIndex(searchView, lvi.Index);
                }
                else
                {
                    MessageBox.Show("잘못된 링크를 참조하고 있습니다.", Application.ProductName);
                }
            }
        }

        public void CopyString(String szMessage)
        {
            System.Windows.Clipboard.SetText(szMessage, System.Windows.TextDataFormat.Text);
            Global.Output(String.Format("문자열이 복사되었습니다({0}).\n", szMessage));
        }
    }
}