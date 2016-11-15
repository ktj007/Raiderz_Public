namespace Velixian
{
    partial class EditForm_Quest
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.cbQuestFilter = new System.Windows.Forms.ComboBox();
            this.lvQuests = new System.Windows.Forms.ListView();
            this.colQuestID = new System.Windows.Forms.ColumnHeader();
            this.colQuestTitle = new System.Windows.Forms.ColumnHeader();
            this.colQuestCategory = new System.Windows.Forms.ColumnHeader();
            this.colQuestComment = new System.Windows.Forms.ColumnHeader();
            this.colQuestDesc = new System.Windows.Forms.ColumnHeader();
            this.colQuestCond = new System.Windows.Forms.ColumnHeader();
            this.subQuest = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subQuestAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.subQuestCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.subQuestPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.subQuestDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.wbQuestDetail = new System.Windows.Forms.WebBrowser();
            this.pgQuest = new System.Windows.Forms.PropertyGrid();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.gridQuestObjective = new SourceGrid.Grid();
            this.subObjective = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subObjectiveAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.subObjectiveDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer5 = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbRewardsRewarder = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tbRewardsDialog = new System.Windows.Forms.TextBox();
            this.gridQuestReward = new SourceGrid.Grid();
            this.subReward = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subRewardAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.subRewardDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer6 = new System.Windows.Forms.SplitContainer();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.gridQuestItem = new SourceGrid.Grid();
            this.subItem = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subItemAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.subItemDel = new System.Windows.Forms.ToolStripMenuItem();
            this.pgQuestElement = new System.Windows.Forms.PropertyGrid();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.subQuest.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.subObjective.SuspendLayout();
            this.splitContainer5.Panel1.SuspendLayout();
            this.splitContainer5.Panel2.SuspendLayout();
            this.splitContainer5.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.subReward.SuspendLayout();
            this.splitContainer6.Panel1.SuspendLayout();
            this.splitContainer6.Panel2.SuspendLayout();
            this.splitContainer6.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.subItem.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.cbQuestFilter);
            this.splitContainer1.Panel1.Controls.Add(this.lvQuests);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(1164, 681);
            this.splitContainer1.SplitterDistance = 313;
            this.splitContainer1.TabIndex = 4;
            // 
            // cbQuestFilter
            // 
            this.cbQuestFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbQuestFilter.DisplayMember = "Broken Mast";
            this.cbQuestFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbQuestFilter.FormattingEnabled = true;
            this.cbQuestFilter.ItemHeight = 21;
            this.cbQuestFilter.Items.AddRange(new object[] {
            "",
            "category:Ingen",
            "category:Teress Plains",
            "category:Riode",
            "category:에다산",
            "category:Broken Mast",
            "category:Rengot Village",
            "category:마코트 부락",
            "category:Hero's Lane",
            "category:Contaminated Garden",
            "category:Catacombs",
            "category:Challenger"});
            this.cbQuestFilter.Location = new System.Drawing.Point(12, 9);
            this.cbQuestFilter.Name = "cbQuestFilter";
            this.cbQuestFilter.Size = new System.Drawing.Size(285, 29);
            this.cbQuestFilter.TabIndex = 6;
            this.cbQuestFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbQuestFilter_KeyDown);
            // 
            // lvQuests
            // 
            this.lvQuests.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvQuests.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colQuestID,
            this.colQuestTitle,
            this.colQuestCategory,
            this.colQuestComment,
            this.colQuestDesc,
            this.colQuestCond});
            this.lvQuests.ContextMenuStrip = this.subQuest;
            this.lvQuests.FullRowSelect = true;
            this.lvQuests.HideSelection = false;
            this.lvQuests.Location = new System.Drawing.Point(0, 47);
            this.lvQuests.MultiSelect = false;
            this.lvQuests.Name = "lvQuests";
            this.lvQuests.Size = new System.Drawing.Size(310, 634);
            this.lvQuests.TabIndex = 4;
            this.lvQuests.UseCompatibleStateImageBehavior = false;
            this.lvQuests.View = System.Windows.Forms.View.Details;
            this.lvQuests.SelectedIndexChanged += new System.EventHandler(this.lvQuests_SelectedIndexChanged);
            this.lvQuests.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvQuests_KeyDown);
            // 
            // colQuestID
            // 
            this.colQuestID.Text = "ID";
            this.colQuestID.Width = 69;
            // 
            // colQuestTitle
            // 
            this.colQuestTitle.Text = "Title";
            this.colQuestTitle.Width = 95;
            // 
            // colQuestCategory
            // 
            this.colQuestCategory.Text = "Category";
            // 
            // colQuestComment
            // 
            this.colQuestComment.Text = "Comment";
            this.colQuestComment.Width = 200;
            // 
            // colQuestDesc
            // 
            this.colQuestDesc.Text = "Desc";
            this.colQuestDesc.Width = 218;
            // 
            // colQuestCond
            // 
            this.colQuestCond.Text = "Conditions";
            this.colQuestCond.Width = 161;
            // 
            // subQuest
            // 
            this.subQuest.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subQuestAdd,
            this.toolStripMenuItem3,
            this.subQuestCopy,
            this.subQuestPaste,
            this.toolStripMenuItem1,
            this.subQuestDel});
            this.subQuest.Name = "subDialog";
            this.subQuest.Size = new System.Drawing.Size(139, 104);
            this.subQuest.Opening += new System.ComponentModel.CancelEventHandler(this.subQuest_Opening);
            // 
            // subQuestAdd
            // 
            this.subQuestAdd.Name = "subQuestAdd";
            this.subQuestAdd.Size = new System.Drawing.Size(138, 22);
            this.subQuestAdd.Text = "Add(&A)";
            this.subQuestAdd.Click += new System.EventHandler(this.subQuestAdd_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(135, 6);
            // 
            // subQuestCopy
            // 
            this.subQuestCopy.Name = "subQuestCopy";
            this.subQuestCopy.Size = new System.Drawing.Size(138, 22);
            this.subQuestCopy.Text = "Copy(&C)";
            this.subQuestCopy.Click += new System.EventHandler(this.subQuestCopy_Click);
            // 
            // subQuestPaste
            // 
            this.subQuestPaste.Name = "subQuestPaste";
            this.subQuestPaste.Size = new System.Drawing.Size(138, 22);
            this.subQuestPaste.Text = "Paste(&P)";
            this.subQuestPaste.Click += new System.EventHandler(this.subQuestPaste_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(135, 6);
            // 
            // subQuestDel
            // 
            this.subQuestDel.Name = "subQuestDel";
            this.subQuestDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subQuestDel.Size = new System.Drawing.Size(138, 22);
            this.subQuestDel.Text = "Delete(&D)";
            this.subQuestDel.Click += new System.EventHandler(this.subQuestDel_Click);
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.splitContainer3);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer4);
            this.splitContainer2.Size = new System.Drawing.Size(847, 681);
            this.splitContainer2.SplitterDistance = 508;
            this.splitContainer2.TabIndex = 0;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 0);
            this.splitContainer3.Name = "splitContainer3";
            this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.wbQuestDetail);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.pgQuest);
            this.splitContainer3.Size = new System.Drawing.Size(508, 681);
            this.splitContainer3.SplitterDistance = 344;
            this.splitContainer3.TabIndex = 0;
            // 
            // wbQuestDetail
            // 
            this.wbQuestDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbQuestDetail.Location = new System.Drawing.Point(0, 0);
            this.wbQuestDetail.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbQuestDetail.Name = "wbQuestDetail";
            this.wbQuestDetail.Size = new System.Drawing.Size(508, 344);
            this.wbQuestDetail.TabIndex = 0;
            // 
            // pgQuest
            // 
            this.pgQuest.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgQuest.Location = new System.Drawing.Point(0, 0);
            this.pgQuest.Name = "pgQuest";
            this.pgQuest.Size = new System.Drawing.Size(508, 333);
            this.pgQuest.TabIndex = 0;
            this.pgQuest.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgQuest_PropertyValueChanged);
            // 
            // splitContainer4
            // 
            this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer4.Location = new System.Drawing.Point(0, 0);
            this.splitContainer4.Name = "splitContainer4";
            this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.groupBox2);
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.splitContainer5);
            this.splitContainer4.Size = new System.Drawing.Size(335, 681);
            this.splitContainer4.SplitterDistance = 155;
            this.splitContainer4.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.gridQuestObjective);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(335, 155);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Objectives";
            // 
            // gridQuestObjective
            // 
            this.gridQuestObjective.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gridQuestObjective.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.gridQuestObjective.ContextMenuStrip = this.subObjective;
            this.gridQuestObjective.CustomSort = true;
            this.gridQuestObjective.EnableSort = true;
            this.gridQuestObjective.Location = new System.Drawing.Point(2, 20);
            this.gridQuestObjective.Name = "gridQuestObjective";
            this.gridQuestObjective.OptimizeMode = SourceGrid.CellOptimizeMode.ForRows;
            this.gridQuestObjective.SelectionMode = SourceGrid.GridSelectionMode.Row;
            this.gridQuestObjective.Size = new System.Drawing.Size(331, 133);
            this.gridQuestObjective.TabIndex = 2;
            this.gridQuestObjective.TabStop = true;
            this.gridQuestObjective.ToolTipText = "";
            // 
            // subObjective
            // 
            this.subObjective.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subObjectiveAdd,
            this.toolStripMenuItem2,
            this.subObjectiveDel});
            this.subObjective.Name = "subObjective";
            this.subObjective.Size = new System.Drawing.Size(152, 54);
            // 
            // subObjectiveAdd
            // 
            this.subObjectiveAdd.Name = "subObjectiveAdd";
            this.subObjectiveAdd.Size = new System.Drawing.Size(151, 22);
            this.subObjectiveAdd.Text = "Objective Add";
            this.subObjectiveAdd.Click += new System.EventHandler(this.objectiveToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(148, 6);
            // 
            // subObjectiveDel
            // 
            this.subObjectiveDel.Name = "subObjectiveDel";
            this.subObjectiveDel.Size = new System.Drawing.Size(151, 22);
            this.subObjectiveDel.Text = "Objective Delete";
            this.subObjectiveDel.Click += new System.EventHandler(this.subObjectiveDel_Click);
            // 
            // splitContainer5
            // 
            this.splitContainer5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer5.Location = new System.Drawing.Point(0, 0);
            this.splitContainer5.Name = "splitContainer5";
            this.splitContainer5.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer5.Panel1
            // 
            this.splitContainer5.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer5.Panel2
            // 
            this.splitContainer5.Panel2.Controls.Add(this.splitContainer6);
            this.splitContainer5.Size = new System.Drawing.Size(335, 522);
            this.splitContainer5.SplitterDistance = 155;
            this.splitContainer5.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.tbRewardsRewarder);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.tbRewardsDialog);
            this.groupBox1.Controls.Add(this.gridQuestReward);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(335, 155);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Rewards";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(34, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 12);
            this.label2.TabIndex = 7;
            this.label2.Text = "rewarder";
            // 
            // tbRewardsRewarder
            // 
            this.tbRewardsRewarder.Location = new System.Drawing.Point(91, 50);
            this.tbRewardsRewarder.Name = "tbRewardsRewarder";
            this.tbRewardsRewarder.Size = new System.Drawing.Size(163, 21);
            this.tbRewardsRewarder.TabIndex = 6;
            this.tbRewardsRewarder.TextChanged += new System.EventHandler(this.tbRewardsRewarder_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(34, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 12);
            this.label1.TabIndex = 5;
            this.label1.Text = "dialog";
            // 
            // tbRewardsDialog
            // 
            this.tbRewardsDialog.Location = new System.Drawing.Point(91, 20);
            this.tbRewardsDialog.Name = "tbRewardsDialog";
            this.tbRewardsDialog.Size = new System.Drawing.Size(163, 21);
            this.tbRewardsDialog.TabIndex = 4;
            this.tbRewardsDialog.TextChanged += new System.EventHandler(this.tbRewardsDialog_TextChanged);
            // 
            // gridQuestReward
            // 
            this.gridQuestReward.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gridQuestReward.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.gridQuestReward.ContextMenuStrip = this.subReward;
            this.gridQuestReward.CustomSort = true;
            this.gridQuestReward.EnableSort = true;
            this.gridQuestReward.Location = new System.Drawing.Point(2, 79);
            this.gridQuestReward.Name = "gridQuestReward";
            this.gridQuestReward.OptimizeMode = SourceGrid.CellOptimizeMode.ForRows;
            this.gridQuestReward.SelectionMode = SourceGrid.GridSelectionMode.Row;
            this.gridQuestReward.Size = new System.Drawing.Size(331, 74);
            this.gridQuestReward.TabIndex = 3;
            this.gridQuestReward.TabStop = true;
            this.gridQuestReward.ToolTipText = "";
            // 
            // subReward
            // 
            this.subReward.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subRewardAdd,
            this.toolStripSeparator1,
            this.subRewardDel});
            this.subReward.Name = "subObjective";
            this.subReward.Size = new System.Drawing.Size(141, 54);
            // 
            // subRewardAdd
            // 
            this.subRewardAdd.Name = "subRewardAdd";
            this.subRewardAdd.Size = new System.Drawing.Size(140, 22);
            this.subRewardAdd.Text = "Reward Add";
            this.subRewardAdd.Click += new System.EventHandler(this.subRewardAdd_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(137, 6);
            // 
            // subRewardDel
            // 
            this.subRewardDel.Name = "subRewardDel";
            this.subRewardDel.Size = new System.Drawing.Size(140, 22);
            this.subRewardDel.Text = "Reward Delete";
            this.subRewardDel.Click += new System.EventHandler(this.subRewardDel_Click);
            // 
            // splitContainer6
            // 
            this.splitContainer6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer6.Location = new System.Drawing.Point(0, 0);
            this.splitContainer6.Name = "splitContainer6";
            this.splitContainer6.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer6.Panel1
            // 
            this.splitContainer6.Panel1.Controls.Add(this.groupBox3);
            // 
            // splitContainer6.Panel2
            // 
            this.splitContainer6.Panel2.Controls.Add(this.pgQuestElement);
            this.splitContainer6.Size = new System.Drawing.Size(335, 363);
            this.splitContainer6.SplitterDistance = 155;
            this.splitContainer6.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.gridQuestItem);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox3.Location = new System.Drawing.Point(0, 0);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(335, 155);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Items";
            // 
            // gridQuestItem
            // 
            this.gridQuestItem.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gridQuestItem.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.gridQuestItem.ContextMenuStrip = this.subItem;
            this.gridQuestItem.CustomSort = true;
            this.gridQuestItem.EnableSort = true;
            this.gridQuestItem.Location = new System.Drawing.Point(3, 20);
            this.gridQuestItem.Name = "gridQuestItem";
            this.gridQuestItem.OptimizeMode = SourceGrid.CellOptimizeMode.ForRows;
            this.gridQuestItem.SelectionMode = SourceGrid.GridSelectionMode.Row;
            this.gridQuestItem.Size = new System.Drawing.Size(329, 125);
            this.gridQuestItem.TabIndex = 4;
            this.gridQuestItem.TabStop = true;
            this.gridQuestItem.ToolTipText = "";
            // 
            // subItem
            // 
            this.subItem.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subItemAdd,
            this.toolStripSeparator2,
            this.subItemDel});
            this.subItem.Name = "subObjective";
            this.subItem.Size = new System.Drawing.Size(126, 54);
            // 
            // subItemAdd
            // 
            this.subItemAdd.Name = "subItemAdd";
            this.subItemAdd.Size = new System.Drawing.Size(125, 22);
            this.subItemAdd.Text = "Item Add";
            this.subItemAdd.Click += new System.EventHandler(this.subItemAdd_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(122, 6);
            // 
            // subItemDel
            // 
            this.subItemDel.Name = "subItemDel";
            this.subItemDel.Size = new System.Drawing.Size(125, 22);
            this.subItemDel.Text = "Item Delete";
            this.subItemDel.Click += new System.EventHandler(this.subItemDel_Click);
            // 
            // pgQuestElement
            // 
            this.pgQuestElement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgQuestElement.Location = new System.Drawing.Point(0, 0);
            this.pgQuestElement.Name = "pgQuestElement";
            this.pgQuestElement.Size = new System.Drawing.Size(335, 204);
            this.pgQuestElement.TabIndex = 0;
            this.pgQuestElement.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgQuestElement_PropertyValueChanged);
            // 
            // EditForm_Quest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1164, 681);
            this.Controls.Add(this.splitContainer1);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_Quest";
            this.Text = "Quests";
            this.Load += new System.EventHandler(this.EditForm_Quest_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_Quest_KeyDown);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.subQuest.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            this.splitContainer3.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel2.ResumeLayout(false);
            this.splitContainer4.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.subObjective.ResumeLayout(false);
            this.splitContainer5.Panel1.ResumeLayout(false);
            this.splitContainer5.Panel2.ResumeLayout(false);
            this.splitContainer5.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.subReward.ResumeLayout(false);
            this.splitContainer6.Panel1.ResumeLayout(false);
            this.splitContainer6.Panel2.ResumeLayout(false);
            this.splitContainer6.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.subItem.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView lvQuests;
        private System.Windows.Forms.ColumnHeader colQuestID;
        private System.Windows.Forms.ColumnHeader colQuestTitle;
        private System.Windows.Forms.ColumnHeader colQuestDesc;
        private System.Windows.Forms.ColumnHeader colQuestCond;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.WebBrowser wbQuestDetail;
        private System.Windows.Forms.PropertyGrid pgQuest;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.SplitContainer splitContainer5;
        private System.Windows.Forms.SplitContainer splitContainer6;
        private System.Windows.Forms.PropertyGrid pgQuestElement;
        private System.Windows.Forms.ColumnHeader colQuestCategory;
        private System.Windows.Forms.ContextMenuStrip subObjective;
        private System.Windows.Forms.ToolStripMenuItem subObjectiveAdd;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem subObjectiveDel;
        private System.Windows.Forms.ContextMenuStrip subReward;
        private System.Windows.Forms.ToolStripMenuItem subRewardAdd;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem subRewardDel;
        private System.Windows.Forms.ContextMenuStrip subItem;
        private System.Windows.Forms.ToolStripMenuItem subItemAdd;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem subItemDel;
        private System.Windows.Forms.GroupBox groupBox1;
        private SourceGrid.Grid gridQuestReward;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbRewardsRewarder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbRewardsDialog;
        private System.Windows.Forms.GroupBox groupBox2;
        private SourceGrid.Grid gridQuestObjective;
        private System.Windows.Forms.GroupBox groupBox3;
        private SourceGrid.Grid gridQuestItem;
        private System.Windows.Forms.ContextMenuStrip subQuest;
        private System.Windows.Forms.ToolStripMenuItem subQuestAdd;
        private System.Windows.Forms.ToolStripMenuItem subQuestDel;
        private System.Windows.Forms.ColumnHeader colQuestComment;
        private System.Windows.Forms.ToolStripMenuItem subQuestCopy;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem subQuestPaste;
        private System.Windows.Forms.ComboBox cbQuestFilter;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
    }
}