using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    public partial class EditForm_Quest : EditForm
    {
        internal SHXmlCore xmlCore;
        protected QuestListViewController m_ListViewController;

        //////////////////////////////////////////////////////////////////////////

        public class QuestListViewController : BaseListViewController
        {
            public QuestListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHQuest quest = (SHQuest)(selDataObject);

                String szCond = "";

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                quest.Compile();
                quest.Build(m_XmlCore);

                if (quest.condition != 0 && quest.conditionSpecified)
                {
                    try
                    {
                        szCond = m_XmlCore.Conditions[quest.condition].GetString(m_XmlCore);
                    }
                    catch
                    {
                        szCond = quest.condition.ToString() + " (Conditions have not been used)";
                    }
                }

                lvi.Text = quest.id.ToString();
                lvi.SubItems.AddRange(new String[] 
            {
                (quest.title != null) ? m_XmlCore.GetSafeString(quest.title) : String.Empty,
                (quest.category != null) ? m_XmlCore.GetSafeString(quest.category) : String.Empty,
                (quest.comment != null) ? m_XmlCore.GetSafeString(quest.comment) : String.Empty,
                (quest.desc != null) ? m_XmlCore.GetSafeString(quest.desc) : String.Empty,
                szCond
            });

                lvi.Tag = quest;
                if (!quest.Passed) lvi.ImageIndex = 7;

                lvi.ForeColor = (quest.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (quest.Passed) ? Color.White : Color.Yellow;

            }

            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHQuest)) return 0;

                SHQuest selQuest = (SHQuest)listViewItemTag;

                if (selQuest != null)
                {
                    return selQuest.id;
                }
                return 0;
            }

            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHQuest quest = (SHQuest)(dataObject);

                if (strHead.ToLower() == "id")  return ExistFilterString(strFilter, quest.id.ToString());
                if (strHead.ToLower() == "title" || strHead == "제목")          return ExistFilterString(strFilter, quest.title);
                if (strHead.ToLower() == "category" || strHead == "카테고리") return ExistFilterString(strFilter, quest.category);

                if (ExistFilterString(strFilter, quest.id.ToString())) return true;
                if (ExistFilterString(strFilter, quest.title)) return true;
                if (ExistFilterString(strFilter, quest.desc)) return true;
                if (ExistFilterString(strFilter, quest.category)) return true;

                return false;
            }

            protected override bool _PasteDataObject(SHDataObject tarDataObject)
            {
                if (m_XmlCore.Quests.IsValid(m_nCopyObjectID) == false)
                {
                    return false;
                }

                SHQuest srcQuest = m_XmlCore.Quests[m_nCopyObjectID];
                SHQuest tarQuest = (SHQuest)(tarDataObject);

                tarQuest.Assign(srcQuest);

                Global._VelixianForms.FindForm("QUEST").Touch();

                return true;
            }

        }

        //////////////////////////////////////////////////////////////////////////


        #region QuestBaseController Class
        public abstract class QuestBaseController
        {
            protected SourceGrid.Grid m_Grid;
            protected PropertyGrid m_PropertyGrid;

            protected void Selection_SelectionChanged(object sender, SourceGrid.RangeRegionChangedEventArgs e)
            {
                int selectedRow = -1;

                if (sender is SourceGrid.Selection.SelectionBase)
                {
                    SourceGrid.Selection.SelectionBase sel = (sender as SourceGrid.Selection.SelectionBase);
                    selectedRow = (sender as SourceGrid.Selection.SelectionBase).ActivePosition.Row;
                }

                if (selectedRow < 0) return;

                RefreshPropertyGrid(selectedRow);
            }
            public void RefreshPropertyGrid(int selectedRow)
            {
                if (m_Grid.Rows[selectedRow].Tag != null)
                {
                    m_PropertyGrid.SelectedObject = m_Grid.Rows[selectedRow].Tag;
                    m_PropertyGrid.Tag = m_Grid.Rows[selectedRow];
                }
            }
            public abstract void LoadGrid();
        }
        #endregion

        #region QuestObjectiveController 클래스
        public class QuestObjectiveController : QuestBaseController
        {
            private const int COLUMN_COUNT = 3;

            public class ObjectiveValueChangedEvent : SourceGrid.Cells.Controllers.ControllerBase
            {
                private QuestObjectiveController m_GridController;
                public ObjectiveValueChangedEvent(QuestObjectiveController gridController)
                {
                    m_GridController = gridController;
                }

                public override void OnValueChanged(SourceGrid.CellContext sender, EventArgs e)
                {
                    base.OnValueChanged(sender, e);

                    SHQuest quest = (SHQuest)sender.Grid.Tag;
                    if (quest == null) return;

                    int selectedRow = sender.Position.Row;

                    SourceGrid.Grid grid = (sender.Grid as SourceGrid.Grid);

                    SHQuestObjective obj = (SHQuestObjective)(grid.Rows[selectedRow].Tag);

                    if (sender.Position.Column == 0)
                    {
                        obj.id = (int)sender.Value;
                    }
                    else if (sender.Position.Column == 1)
                    {
                        obj.desc = (string)sender.Value;
                    }
                    else if (sender.Position.Column == 2)
                    {
                        obj.type = (SHQuestType)sender.Value;
                    }

                    m_GridController.RefreshPropertyGrid(selectedRow);

                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public QuestObjectiveController(SourceGrid.Grid grid, PropertyGrid propertyGrid)
            {
                m_Grid = grid;
                m_PropertyGrid = propertyGrid;
            }

            public override void LoadGrid()
            {
                m_Grid.Selection.SelectionChanged += new SourceGrid.RangeRegionChangedEventHandler(Selection_SelectionChanged);
                m_Grid.Selection.EnableMultiSelection = true;

                m_Grid.Redim(1, COLUMN_COUNT);

                m_Grid[0, 0] = new SourceGrid.Cells.ColumnHeader("ID");
                m_Grid[0, 1] = new SourceGrid.Cells.ColumnHeader("Desc");
                m_Grid[0, 2] = new SourceGrid.Cells.ColumnHeader("Type");

                m_Grid.Columns[0].Width = 60;
                m_Grid.Columns[1].Width = 160;
                m_Grid.Columns[2].Width = 120;
            }
            public void AssignGrid(SHQuest quest)
            {
                if (quest == null) return;

                m_Grid.Tag = quest;

                m_Grid.Redim(1, COLUMN_COUNT);

                int nRowCount = 1;
                if (quest.Objectives != null)
                {
                    nRowCount = quest.Objectives.dataList.Count + 1;
                }

                m_Grid.Redim(nRowCount, COLUMN_COUNT);

                if (quest.Objectives == null) return;

                for (int r = 0; r < quest.Objectives.dataList.Count; r++)
                {
                    SHQuestObjective obj = (SHQuestObjective)quest.Objectives.dataList[r];

                    AssignNewGrid(r + 1, obj);
                }
            }

            public void AssignGridRow(SourceGrid.GridRow gridRow, SHQuestObjective obj)
            {
                if (gridRow == null || obj == null) return;

                (gridRow.Grid.GetCell(gridRow.Index, 0) as SourceGrid.Cells.Cell).Value = obj.id;
                (gridRow.Grid.GetCell(gridRow.Index, 1) as SourceGrid.Cells.Cell).Value = obj.desc;
                (gridRow.Grid.GetCell(gridRow.Index, 2) as SourceGrid.Cells.Cell).Value = obj.type;
            }

            private void AssignNewGrid(int nRowIndex, SHQuestObjective obj)
            {
                m_Grid.Rows[nRowIndex].Tag = obj;

                m_Grid[nRowIndex, 0] = new SourceGrid.Cells.Cell(obj.id, typeof(int));
                m_Grid[nRowIndex, 1] = new SourceGrid.Cells.Cell(obj.desc, typeof(string));
                m_Grid[nRowIndex, 2] = new SourceGrid.Cells.Cell(obj.type, typeof(SHQuestType));

                //m_Grid[r + 1, 0].Editor.EnableEdit = false;
                m_Grid[nRowIndex, 0].AddController(new ObjectiveValueChangedEvent(this));
                m_Grid[nRowIndex, 1].AddController(new ObjectiveValueChangedEvent(this));
                m_Grid[nRowIndex, 2].AddController(new ObjectiveValueChangedEvent(this));
            }

            public void AddObjective()
            {
                SHQuestObjective newObj = new SHQuestObjective();

                newObj.id = 1;
                newObj.desc = String.Empty;
                newObj.type = SHQuestType.DESTROY;
                newObj.public_progress = "self";
                newObj.param1 = "0";

                if (m_Grid.Tag != null && m_Grid.Tag.GetType() == typeof(SHQuest))
                {
                    SHQuest quest = (SHQuest)m_Grid.Tag;

                    quest.Objectives.dataList.Add(newObj);

                    AssignGrid(quest);
                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public void DelObjective()
            {
                if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    int selectedRow = -1;
                    selectedRow = m_Grid.Selection.ActivePosition.Row;

                    if (selectedRow < 0) return;

                    SHQuest quest = (SHQuest)m_Grid.Tag;
                    SHQuestObjective obj = (SHQuestObjective)m_Grid.Rows[selectedRow].Tag;

                    if (quest != null && obj != null)
                    {
                        quest.Objectives.dataList.Remove(obj);

                        AssignGrid(quest);
                        Global._VelixianForms.FindForm("QUEST").Touch();
                    }
                }

            }
        }
        #endregion

        #region QuestRewardController 클래스
        public class QuestRewardController : QuestBaseController
        {
            private TextBox m_tbDialog;
            private TextBox m_tbRewarder;

            private const int COLUMN_COUNT = 3;

            public class RewardValueChangedEvent : SourceGrid.Cells.Controllers.ControllerBase
            {
                private QuestRewardController m_GridController;
                public RewardValueChangedEvent(QuestRewardController gridController)
                {
                    m_GridController = gridController;
                }

                public override void OnValueChanged(SourceGrid.CellContext sender, EventArgs e)
                {
                    base.OnValueChanged(sender, e);

                    SHQuest quest = (SHQuest)sender.Grid.Tag;
                    if (quest == null) return;

                    int selectedRow = sender.Position.Row;

                    SourceGrid.Grid grid = (sender.Grid as SourceGrid.Grid);

                    SHQuestReward reward = (SHQuestReward)(grid.Rows[selectedRow].Tag);

                    if (sender.Position.Column == 0)
                    {
                        reward.type = (SHQuestRewardType)sender.Value;
                    }
                    else if (sender.Position.Column == 1)
                    {
                        reward.param1 = (string)sender.Value;
                    }
                    else if (sender.Position.Column == 2)
                    {
                        reward.param2 = (string)sender.Value;
                    }

                    m_GridController.RefreshPropertyGrid(selectedRow);

                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public QuestRewardController(SourceGrid.Grid grid, PropertyGrid propertyGrid, TextBox tbDialog, TextBox tbRewarder)
            {
                m_Grid = grid;
                m_PropertyGrid = propertyGrid;
                m_tbDialog = tbDialog;
                m_tbRewarder = tbRewarder;
            }

            public override void LoadGrid()
            {
                m_Grid.Selection.SelectionChanged += new SourceGrid.RangeRegionChangedEventHandler(Selection_SelectionChanged);
                m_Grid.Selection.EnableMultiSelection = true;

                m_Grid.Redim(1, COLUMN_COUNT);

                m_Grid[0, 0] = new SourceGrid.Cells.ColumnHeader("Type");
                m_Grid[0, 1] = new SourceGrid.Cells.ColumnHeader("Param1");
                m_Grid[0, 2] = new SourceGrid.Cells.ColumnHeader("Param2");

                m_Grid.Columns[0].Width = 80;
                m_Grid.Columns[1].Width = 120;
                m_Grid.Columns[2].Width = 120;
            }
            public void AssignGrid(SHQuest quest)
            {
                if (quest == null) return;

                m_Grid.Tag = quest;

                m_Grid.Redim(1, COLUMN_COUNT);

                int nRowCount = 1;
                if (quest.Rewards != null)
                {
                    nRowCount = quest.Rewards.dataList.Count + 1;
                }

                m_Grid.Redim(nRowCount, COLUMN_COUNT);

                if (quest.Rewards == null) return;

                for (int r = 0; r < quest.Rewards.dataList.Count; r++)
                {
                    SHQuestReward obj = (SHQuestReward)quest.Rewards.dataList[r];

                    AssignNewGrid(r + 1, obj);
                }
            }
            public void AssignTextBoxs(SHQuest quest)
            {
                if (quest == null) return;

                m_tbDialog.Tag = null;
                m_tbRewarder.Tag = null;

                m_tbDialog.Text = quest.Rewards.dialog.ToString();
                m_tbRewarder.Text = quest.Rewards.rewarder;

                m_tbDialog.Tag = quest.Rewards;
                m_tbRewarder.Tag = quest.Rewards;
            }

            public void AssignGridRow(SourceGrid.GridRow gridRow, SHQuestReward obj)
            {
                if (gridRow == null || obj == null) return;

                (gridRow.Grid.GetCell(gridRow.Index, 0) as SourceGrid.Cells.Cell).Value = obj.type;
                (gridRow.Grid.GetCell(gridRow.Index, 1) as SourceGrid.Cells.Cell).Value = obj.param1;
                (gridRow.Grid.GetCell(gridRow.Index, 2) as SourceGrid.Cells.Cell).Value = obj.param2;
            }

            private void AssignNewGrid(int nRowIndex, SHQuestReward obj)
            {
                m_Grid.Rows[nRowIndex].Tag = obj;

                m_Grid[nRowIndex, 0] = new SourceGrid.Cells.Cell(obj.type, typeof(SHQuestRewardType));
                m_Grid[nRowIndex, 1] = new SourceGrid.Cells.Cell(obj.param1, typeof(string));
                m_Grid[nRowIndex, 2] = new SourceGrid.Cells.Cell(obj.param2, typeof(string));

                //m_Grid[r + 1, 0].Editor.EnableEdit = false;
                m_Grid[nRowIndex, 0].AddController(new RewardValueChangedEvent(this));
                m_Grid[nRowIndex, 1].AddController(new RewardValueChangedEvent(this));
                m_Grid[nRowIndex, 2].AddController(new RewardValueChangedEvent(this));
            }

            public void AddReward()
            {
                SHQuestReward newObj = new SHQuestReward();

                newObj.type = SHQuestRewardType.SILVER;
                newObj.param1 = "";

                if (m_Grid.Tag != null && m_Grid.Tag.GetType() == typeof(SHQuest))
                {
                    SHQuest quest = (SHQuest)m_Grid.Tag;

                    quest.Rewards.dataList.Add(newObj);

                    AssignGrid(quest);
                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public void DelReward()
            {
                if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    int selectedRow = -1;
                    selectedRow = m_Grid.Selection.ActivePosition.Row;

                    if (selectedRow < 0) return;

                    SHQuest quest = (SHQuest)m_Grid.Tag;
                    SHQuestReward obj = (SHQuestReward)m_Grid.Rows[selectedRow].Tag;

                    if (quest != null && obj != null)
                    {
                        quest.Rewards.dataList.Remove(obj);

                        AssignGrid(quest);
                        Global._VelixianForms.FindForm("QUEST").Touch();
                    }
                }
            }
            public void OnDialog_TextChanged()
            {
                if (m_tbDialog.Tag != null)
                {
                    SHQuestRewards rewards = (SHQuestRewards)m_tbDialog.Tag;

                    int nDialogText = 0;
                    try { nDialogText = Convert.ToInt32(m_tbDialog.Text); }
                    catch (Exception e) { return;  }

                    rewards.dialog = nDialogText;

                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }
            public void OnRewarder_TextChanged()
            {
                if (m_tbRewarder.Tag != null)
                {
                    SHQuestRewards rewards = (SHQuestRewards)m_tbRewarder.Tag;

                    String[] tmp = m_tbRewarder.Text.Split(new Char[] { ',' });
                    if (tmp.Length != 2)
                    {
                        return;
                    }

                    rewards.rewarder = m_tbRewarder.Text;

                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }
        }
        #endregion

        #region QuestItemController 클래스
        public class QuestItemController : QuestBaseController
        {
            private const int COLUMN_COUNT = 3;

            public class ItemValueChangedEvent : SourceGrid.Cells.Controllers.ControllerBase
            {
                private QuestItemController m_GridController;
                public ItemValueChangedEvent(QuestItemController gridController)
                {
                    m_GridController = gridController;
                }

                public override void OnValueChanged(SourceGrid.CellContext sender, EventArgs e)
                {
                    base.OnValueChanged(sender, e);

                    SHQuest quest = (SHQuest)sender.Grid.Tag;
                    if (quest == null) return;

                    int selectedRow = sender.Position.Row;

                    SourceGrid.Grid grid = (sender.Grid as SourceGrid.Grid);

                    SHItemAddRemoveBase item = (SHItemAddRemoveBase)(grid.Rows[selectedRow].Tag);

                    if (sender.Position.Column == 0)
                    {
                        SHItemAddRemoveBase newItem = SHItemAddRemove.NewItem((SHItemAddRemoveType)sender.Value);
                        newItem.id = item.id;
                        newItem.amount = item.amount;

                        quest.Items.dataList[selectedRow-1] = newItem;
                    }
                    else if (sender.Position.Column == 1)
                    {
                        item.id = (int)sender.Value;
                    }
                    else if (sender.Position.Column == 2)
                    {
                        item.amount = (int)sender.Value;
                    }

                    m_GridController.RefreshPropertyGrid(selectedRow);

                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public QuestItemController(SourceGrid.Grid grid, PropertyGrid propertyGrid)
            {
                m_Grid = grid;
                m_PropertyGrid = propertyGrid;
            }

            public override void LoadGrid()
            {
                m_Grid.Selection.SelectionChanged += new SourceGrid.RangeRegionChangedEventHandler(Selection_SelectionChanged);
                m_Grid.Selection.EnableMultiSelection = true;

                m_Grid.Redim(1, COLUMN_COUNT);

                m_Grid[0, 0] = new SourceGrid.Cells.ColumnHeader("Type");
                m_Grid[0, 1] = new SourceGrid.Cells.ColumnHeader("ID");
                m_Grid[0, 2] = new SourceGrid.Cells.ColumnHeader("Amount");

                m_Grid.Columns[0].Width = 80;
                m_Grid.Columns[1].Width = 60;
                m_Grid.Columns[2].Width = 60;
            }
            public void AssignGrid(SHQuest quest)
            {
                if (quest == null) return;

                m_Grid.Tag = quest;

                m_Grid.Redim(1, COLUMN_COUNT);

                int nRowCount = 1;
                if (quest.Items != null)
                {
                    nRowCount = quest.Items.dataList.Count + 1;
                }

                m_Grid.Redim(nRowCount, COLUMN_COUNT);

                if (quest.Items == null) return;

                for (int r = 0; r < quest.Items.dataList.Count; r++)
                {
                    SHItemAddRemoveBase obj = (SHItemAddRemoveBase)quest.Items.dataList[r];

                    AssignNewGrid(r + 1, obj);
                }
            }

            public void AssignGridRow(SourceGrid.GridRow gridRow, SHItemAddRemoveBase item)
            {
                if (gridRow == null || item == null) return;

                (gridRow.Grid.GetCell(gridRow.Index, 0) as SourceGrid.Cells.Cell).Value = item.GetItemType();
                (gridRow.Grid.GetCell(gridRow.Index, 1) as SourceGrid.Cells.Cell).Value = item.id;
                (gridRow.Grid.GetCell(gridRow.Index, 2) as SourceGrid.Cells.Cell).Value = item.amount;
            }

            private void AssignNewGrid(int nRowIndex, SHItemAddRemoveBase item)
            {
                m_Grid.Rows[nRowIndex].Tag = item;

                m_Grid[nRowIndex, 0] = new SourceGrid.Cells.Cell(item.GetItemType(), typeof(SHItemAddRemoveType));
                m_Grid[nRowIndex, 1] = new SourceGrid.Cells.Cell(item.id, typeof(int));
                m_Grid[nRowIndex, 2] = new SourceGrid.Cells.Cell(item.amount, typeof(int));

                //m_Grid[r + 1, 0].Editor.EnableEdit = false;
                m_Grid[nRowIndex, 0].AddController(new ItemValueChangedEvent(this));
                m_Grid[nRowIndex, 1].AddController(new ItemValueChangedEvent(this));
                m_Grid[nRowIndex, 2].AddController(new ItemValueChangedEvent(this));
            }

            public void AddItem()
            {
                SHItemAdd newItem = new SHItemAdd();

                newItem.id = 0;
                newItem.amount = 0;

                if (m_Grid.Tag != null && m_Grid.Tag.GetType() == typeof(SHQuest))
                {
                    SHQuest quest = (SHQuest)m_Grid.Tag;

                    quest.Items.dataList.Add(newItem);

                    AssignGrid(quest);
                    Global._VelixianForms.FindForm("QUEST").Touch();
                }
            }

            public void DelItem()
            {
                if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    int selectedRow = -1;
                    selectedRow = m_Grid.Selection.ActivePosition.Row;

                    if (selectedRow < 0) return;

                    SHQuest quest = (SHQuest)m_Grid.Tag;
                    SHItemAddRemoveBase obj = (SHItemAddRemoveBase)m_Grid.Rows[selectedRow].Tag;

                    if (quest != null && obj != null)
                    {
                        quest.Items.dataList.Remove(obj);

                        AssignGrid(quest);
                        Global._VelixianForms.FindForm("QUEST").Touch();
                    }
                }

            }
        }
        #endregion

        private QuestObjectiveController m_ObjectiveGridController;
        private QuestRewardController m_RewardGridController;
        private QuestItemController m_ItemGridController;


        public EditForm_Quest()
        {
            InitializeComponent();
            wbQuestDetail.ScriptErrorsSuppressed = true;

            m_ObjectiveGridController = new QuestObjectiveController(gridQuestObjective, pgQuestElement);
            m_RewardGridController = new QuestRewardController(gridQuestReward, pgQuestElement, tbRewardsDialog, tbRewardsRewarder);
            m_ItemGridController = new QuestItemController(gridQuestItem, pgQuestElement);
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (wbQuestDetail.ObjectForScripting == null) wbQuestDetail.ObjectForScripting = mainForm;

            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.QUEST, lvQuests, "QUEST", xmlCore.Quests.LoadingFailed, null, null, xmlCore.SaveQuests);
            }

            m_ListViewController = new QuestListViewController(xmlCore, xmlCore.Quests, xmlCore.Quests.dataList, lvQuests, cbQuestFilter);
            m_ListViewController.InitListView(null);

            SHListViewUtil.SelectIndex(lvQuests, 0);
        }

        private void lvQuests_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvQuests.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvQuests.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHQuest))
                {
                    SHQuest quest = (SHQuest)lvi.Tag;
                    if (quest != null)
                    {
                        pgQuest.SelectedObject = quest;
                        pgQuest.Tag = lvi;

                        ProcessQuest(quest);

                        RefreshQuestDetail(quest);

                        pgQuestElement.SelectedObject = null;
                        pgQuestElement.Tag = null;

                        Global._mainForm.SetStatusLabelText(quest.CompiledMessage);
                    }
                }
            }

            m_ListViewController.OnSelectedIndexChanged();
        }

        private void ProcessQuest(SHQuest quest)
        {
            if (quest == null) return;

            m_ObjectiveGridController.AssignGrid(quest);
            m_RewardGridController.AssignGrid(quest);
            m_RewardGridController.AssignTextBoxs(quest);
            m_ItemGridController.AssignGrid(quest);
        }

        private void EditForm_Quest_KeyDown(object sender, KeyEventArgs e)
        {
            if (OnEditFormKeyDown(sender, e) == true) return;

            if (e.KeyCode == Keys.F3)
            {
                cbQuestFilter.Focus();
            }
        }

        private void RefreshQuestDetail(SHQuest quest)
        {
            if (quest != null)
            {
                wbQuestDetail.DocumentText = quest.GetHTML(xmlCore);
            }
        }
        private void RefreshQuestDetail()
        {
            if (lvQuests.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvQuests.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHQuest))
                {
                    SHQuest quest = (SHQuest)lvi.Tag;

                    if (quest != null)
                    {
                        RefreshQuestDetail(quest);
                    }
                }
            }
        }

        private void pgQuest_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (pgQuest.Tag.GetType() == typeof(ListViewItem))
            {
                SHQuest quest = (SHQuest)(pgQuest.SelectedObject as SHQuest);
                if (quest != null)
                {
                    if (e.ChangedItem.Label == "(Username)" && quest.id != (int)e.OldValue)
                    {
                        m_ListViewController.SortListView(quest);
                    }
                }

                m_ListViewController.SetListText(pgQuest.Tag as ListViewItem, pgQuest.SelectedObject as SHQuest);
            }

            RefreshQuestDetail();

            Global._VelixianForms.FindForm("QUEST").Touch();
        }

        private void EditForm_Quest_Load(object sender, EventArgs e)
        {
            this.lvQuests.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);

            m_ObjectiveGridController.LoadGrid();
            m_RewardGridController.LoadGrid();
            m_ItemGridController.LoadGrid();
        }

        private void QuestElement_ValueChanged(SourceGrid.GridRow gridRow)
        {
            if (gridRow != null)
            {
                if (gridRow.Tag.GetType() == typeof(SHQuestObjective))
                {
                    SHQuestObjective obj = (SHQuestObjective)gridRow.Tag;

                    m_ObjectiveGridController.AssignGridRow(gridRow, obj);
                }
                else if (gridRow.Tag.GetType() == typeof(SHQuestReward))
                {
                    SHQuestReward reward = (SHQuestReward)gridRow.Tag;

                    m_RewardGridController.AssignGridRow(gridRow, reward);
                }
                else if (gridRow.Tag.GetType() == typeof(SHItemAddRemove))
                {
                    SHItemAddRemoveBase item = (SHItemAddRemoveBase)gridRow.Tag;

                    m_ItemGridController.AssignGridRow(gridRow, item);
                }

            }

            Global._VelixianForms.FindForm("QUEST").Touch();
        }

        private void pgQuestElement_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (pgQuestElement.Tag != null)
            {
                SourceGrid.GridRow gridRow = pgQuestElement.Tag as SourceGrid.GridRow;
                QuestElement_ValueChanged(gridRow);
            }
        }

        private void objectiveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_ObjectiveGridController.AddObjective();
        }

        private void subObjectiveDel_Click(object sender, EventArgs e)
        {
            m_ObjectiveGridController.DelObjective();
        }

        private void subRewardAdd_Click(object sender, EventArgs e)
        {
            m_RewardGridController.AddReward();
        }

        private void subRewardDel_Click(object sender, EventArgs e)
        {
            m_RewardGridController.DelReward();
        }

        private void subItemAdd_Click(object sender, EventArgs e)
        {
            m_ItemGridController.AddItem();
        }

        private void subItemDel_Click(object sender, EventArgs e)
        {
            m_ItemGridController.DelItem();
        }

        private void tbRewardsDialog_TextChanged(object sender, EventArgs e)
        {
            m_RewardGridController.OnDialog_TextChanged();

            lvQuests.SuspendLayout();

            m_ListViewController.SetListText(pgQuest.Tag as ListViewItem, pgQuest.SelectedObject as SHQuest);

            lvQuests.ResumeLayout();
        }

        private void tbRewardsRewarder_TextChanged(object sender, EventArgs e)
        {
            m_RewardGridController.OnRewarder_TextChanged();

            lvQuests.SuspendLayout();
            m_ListViewController.SetListText(pgQuest.Tag as ListViewItem, pgQuest.SelectedObject as SHQuest);
            lvQuests.ResumeLayout();
        }

        private void subQuestAdd_Click(object sender, EventArgs e)
        {
            int nSelQuestID = m_ListViewController.MakeNewID();

            SHQuest newQuest = NewQuest(nSelQuestID);
            xmlCore.Quests.Add(newQuest);

            m_ListViewController.AddItem(newQuest);

            Global._VelixianForms.FindForm("QUEST").Touch();

        }

        private void subQuestDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                if (lvQuests.SelectedItems.Count > 0)
                {
                    ListViewItem lvi = lvQuests.SelectedItems[0];
                    if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHQuest))
                    {
                        xmlCore.Quests.dataList.Remove(lvi.Tag);
                        lvi.Remove();
                    }
                }
            }

            Global._VelixianForms.FindForm("QUEST").Touch();
        }

        private void subQuest_Opening(object sender, CancelEventArgs e)
        {
            if (m_ListViewController.IsCopied())
            {
                subQuestPaste.Enabled = true;
            }
            else
            {
                subQuestPaste.Enabled = false;                
            }
        }

        private void subQuestCopy_Click(object sender, EventArgs e)
        {
            m_ListViewController.CopyDataObject();
        }

        private SHQuest NewQuest(int nSelQuestID)
        {
            SHQuest newQuest = new SHQuest();
            newQuest.Objectives = new SHQuestObjectives();
            newQuest.Rewards = new SHQuestRewards();
            newQuest.Items = new SHItemAddRemove();

            newQuest.id = nSelQuestID;
            newQuest.title = "[New String]";
            newQuest.comment = "";
            newQuest.desc = "";
            newQuest.repeatable = false;

            return newQuest;
        }

        private void subQuestPaste_Click(object sender, EventArgs e)
        {
            m_ListViewController.PasteDataObject();
        }

        private void lvQuests_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Quests");
        }

        private void cbQuestFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }
        //////////////////////////////////////////////////////////////////////////

    }
}
