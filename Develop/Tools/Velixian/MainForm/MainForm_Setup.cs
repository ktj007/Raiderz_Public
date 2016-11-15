using System;
using System.Drawing;
using System.Windows.Forms;
using System.Configuration;
using System.Collections;
using SoulHunt;

namespace Velixian
{
    public partial class MainForm
    {
        #region 환경설정
        
        public void LoadTabEnvironment(int nIndex)
        {
/*
            VelixianTab MatchedTab = null;
            int nTmp = 0;

            if (_Tabs._TabPages.TryGetValue(nIndex, out MatchedTab))
            {
                if (MatchedTab.MainList != null)
                {
                    for (int i = 0; i < MatchedTab.MainList.Columns.Count; i++)
                    {
                        ColumnHeader head = MatchedTab.MainList.Columns[i];
                        if (SHGlobal.GetSetting(MatchedTab.MainList.Name + "Col" + i.ToString(), ref nTmp)) head.Width = nTmp;
                    }

                    SplitContainer splitContainer = MatchedTab.MainList.Parent.Parent as SplitContainer;
                    if (splitContainer != null && SHGlobal.GetSetting(splitContainer.Name, ref nTmp)) splitContainer.SplitterDistance = nTmp;

                    if (splitContainer != null)
                    {
                        // 왼쪽 패널의 Child를 검색하여 스플리터가 있으면 환경값 로딩
                        for (int i = 0; i < splitContainer.Panel1.Controls.Count; i++)
                        {
                            SplitContainer childContainer = splitContainer.Panel1.Controls[i] as SplitContainer;
                            if (childContainer != null && SHGlobal.GetSetting(childContainer.Name, ref nTmp)) childContainer.SplitterDistance = nTmp;
                        }

                        // 오른쪽 패널의 Child를 검색하여 스플리터가 있으면 환경값 로딩
                        for (int i = 0; i < splitContainer.Panel2.Controls.Count; i++)
                        {
                            SplitContainer childContainer = splitContainer.Panel2.Controls[i] as SplitContainer;
                            if (childContainer != null && SHGlobal.GetSetting(childContainer.Name, ref nTmp)) childContainer.SplitterDistance = nTmp;
                        }
                    }
                }
            }
 */
        }

        public void SaveTabEnvironment(int nIndex)
        {
/*
            VelixianTab MatchedTab = null;

            if (_Tabs._TabPages.TryGetValue(nIndex, out MatchedTab))
            {
                if (MatchedTab.MainList != null)
                {
                    for (int i = 0; i < MatchedTab.MainList.Columns.Count; i++)
                    {
                        ColumnHeader head = MatchedTab.MainList.Columns[i];
                        SHGlobal.SetSetting(MatchedTab.MainList.Name + "Col" + i.ToString(), head.Width);
                    }

                    SplitContainer splitContainer = MatchedTab.MainList.Parent.Parent as SplitContainer;
                    if (splitContainer != null)
                    {
                        SHGlobal.SetSetting(splitContainer.Name, splitContainer.SplitterDistance);

                        // 왼쪽 패널의 Child를 검색해서 스플리터가 있으면 환경값 저장
                        for (int i = 0; i < splitContainer.Panel1.Controls.Count; i++)
                        {
                            SplitContainer childContainer = splitContainer.Panel1.Controls[i] as SplitContainer;
                            if (childContainer != null) SHGlobal.SetSetting(childContainer.Name, childContainer.SplitterDistance);
                        }

                        // 오른쪽 패널의 Child를 검색하여 스플리터가 있으면 환경값 저장
                        for (int i = 0; i < splitContainer.Panel2.Controls.Count; i++)
                        {
                            SplitContainer childContainer = splitContainer.Panel2.Controls[i] as SplitContainer;
                            if (childContainer != null) SHGlobal.SetSetting(childContainer.Name, childContainer.SplitterDistance);
                        }
                    }
                }
            }
 */
            SHGlobal.SaveSetting();
        }

        public void LoadEnvironment()
        {
            int nTmp = 0;

            if (SHGlobal.GetSetting("Width", ref nTmp)) Width = nTmp;
            if (SHGlobal.GetSetting("Height", ref nTmp)) Height = nTmp;

            if (SHGlobal.GetSetting("Left", ref nTmp)) Left = nTmp;
            if (SHGlobal.GetSetting("Top", ref nTmp)) Top = nTmp;

            // 기본값은 Unchecked이다. 따라서 값을 읽어서 그 값이 1이 된 경우에만 체크한다.
            mnuViewHideNoLoot.CheckState = (SHGlobal.GetSetting("mnuViewHideNoLoot", ref nTmp) && nTmp == 1) ? CheckState.Checked : CheckState.Unchecked;
            mnuViewHideNoninteract.CheckState = (SHGlobal.GetSetting("mnuViewHideNoninteract", ref nTmp) && nTmp == 1) ? CheckState.Checked : CheckState.Unchecked;

            // 기본값은 Checked이다. 따라서 값을 읽어서 그 값이 0이 된 경우에만 끈다.
            mnuViewMonster.CheckState = (SHGlobal.GetSetting("mnuViewMonster", ref nTmp) && nTmp == 0) ? CheckState.Unchecked : CheckState.Checked;
            mnuViewObject.CheckState = (SHGlobal.GetSetting("mnuViewObject", ref nTmp) && nTmp == 0) ? CheckState.Unchecked : CheckState.Checked;
            mnuViewNPC.CheckState = (SHGlobal.GetSetting("mnuViewNPC", ref nTmp) && nTmp == 0) ? CheckState.Unchecked : CheckState.Checked;

            Option.AutoMonitorFile = (SHGlobal.GetSetting("AutoMonitorFile", ref nTmp) && nTmp == 0) ? false : true;
        }

        private void SaveEnvironment()
        {
            SHGlobal.SetSetting("ProjectFolder", SHGlobal.GetFolder());

            SHGlobal.SetSetting("AutoMonitorFile", (Option.AutoMonitorFile == true) ? 1 : 0);

            if (WindowState == FormWindowState.Normal)
            {
                SHGlobal.SetSetting("Width", Width);
                SHGlobal.SetSetting("Height", Height);
                SHGlobal.SetSetting("Left", Left);
                SHGlobal.SetSetting("Top", Top);
            }

            SHGlobal.SetSetting("mnuViewHideNoLoot", (mnuViewHideNoLoot.CheckState == CheckState.Checked) ? 1 : 0);
            SHGlobal.SetSetting("mnuViewHideNoninteract", (mnuViewHideNoninteract.CheckState == CheckState.Checked) ? 1 : 0);
            SHGlobal.SetSetting("mnuViewMonster", (mnuViewMonster.CheckState == CheckState.Checked) ? 1 : 0);
            SHGlobal.SetSetting("mnuViewObject", (mnuViewObject.CheckState == CheckState.Checked) ? 1 : 0);
            SHGlobal.SetSetting("mnuViewNPC", (mnuViewNPC.CheckState == CheckState.Checked) ? 1 : 0);

            for (int i = 0; i < (int)ValixianFormType.PAGE_MAX; i++) SaveTabEnvironment(i);


            SHGlobal.SaveSetting();
        }
        #endregion
    }
}