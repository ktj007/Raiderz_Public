namespace Velixian
{
    partial class EditForm_Looting
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
            this.subLootings = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subLootingsAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.subLootingsDel = new System.Windows.Forms.ToolStripMenuItem();
            this.subLooting = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subLootAddSilver = new System.Windows.Forms.ToolStripMenuItem();
            this.subLootAddItem = new System.Windows.Forms.ToolStripMenuItem();
            this.subLootAddSoul = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.subLootDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitLooting = new System.Windows.Forms.SplitContainer();
            this.cbLootFilter = new System.Windows.Forms.ComboBox();
            this.lvLooting = new System.Windows.Forms.ListView();
            this.colLootingID = new System.Windows.Forms.ColumnHeader();
            this.colLootingComment = new System.Windows.Forms.ColumnHeader();
            this.colLootingTalent = new System.Windows.Forms.ColumnHeader();
            this.colLootingMax = new System.Windows.Forms.ColumnHeader();
            this.colLootingItemCount = new System.Windows.Forms.ColumnHeader();
            this.splitLooting2 = new System.Windows.Forms.SplitContainer();
            this.pgLoot = new System.Windows.Forms.PropertyGrid();
            this.splitLooting3 = new System.Windows.Forms.SplitContainer();
            this.lvLootList = new System.Windows.Forms.ListView();
            this.colLDType = new System.Windows.Forms.ColumnHeader();
            this.colLDRate = new System.Windows.Forms.ColumnHeader();
            this.colLDMin = new System.Windows.Forms.ColumnHeader();
            this.colLDMax = new System.Windows.Forms.ColumnHeader();
            this.pgLootableDetail = new System.Windows.Forms.PropertyGrid();
            this.subLootings.SuspendLayout();
            this.subLooting.SuspendLayout();
            this.splitLooting.Panel1.SuspendLayout();
            this.splitLooting.Panel2.SuspendLayout();
            this.splitLooting.SuspendLayout();
            this.splitLooting2.Panel1.SuspendLayout();
            this.splitLooting2.Panel2.SuspendLayout();
            this.splitLooting2.SuspendLayout();
            this.splitLooting3.Panel1.SuspendLayout();
            this.splitLooting3.Panel2.SuspendLayout();
            this.splitLooting3.SuspendLayout();
            this.SuspendLayout();
            // 
            // subLootings
            // 
            this.subLootings.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subLootingsAdd,
            this.subLootingsDel});
            this.subLootings.Name = "subLootings";
            this.subLootings.Size = new System.Drawing.Size(205, 48);
            this.subLootings.Opening += new System.ComponentModel.CancelEventHandler(this.subLootings_Opening);
            // 
            // subLootingsAdd
            // 
            this.subLootingsAdd.Name = "subLootingsAdd";
            this.subLootingsAdd.Size = new System.Drawing.Size(204, 22);
            this.subLootingsAdd.Text = "Add Loot(&A)";
            this.subLootingsAdd.Click += new System.EventHandler(this.subLootingsAdd_Click);
            // 
            // subLootingsDel
            // 
            this.subLootingsDel.Name = "subLootingsDel";
            this.subLootingsDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subLootingsDel.Size = new System.Drawing.Size(204, 22);
            this.subLootingsDel.Text = "Delete Loot(&D)";
            this.subLootingsDel.Click += new System.EventHandler(this.subLootingsDel_Click);
            // 
            // subLooting
            // 
            this.subLooting.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subLootAddSilver,
            this.subLootAddItem,
            this.subLootAddSoul,
            this.toolStripMenuItem2,
            this.subLootDel});
            this.subLooting.Name = "subLooting";
            this.subLooting.Size = new System.Drawing.Size(165, 98);
            this.subLooting.Opening += new System.ComponentModel.CancelEventHandler(this.subLooting_Opening);
            // 
            // subLootAddSilver
            // 
            this.subLootAddSilver.Name = "subLootAddSilver";
            this.subLootAddSilver.Size = new System.Drawing.Size(164, 22);
            this.subLootAddSilver.Text = "Add Silver";
            this.subLootAddSilver.Click += new System.EventHandler(this.subLootAddSilver_Click);
            // 
            // subLootAddItem
            // 
            this.subLootAddItem.Name = "subLootAddItem";
            this.subLootAddItem.Size = new System.Drawing.Size(164, 22);
            this.subLootAddItem.Text = "Add Item";
            this.subLootAddItem.Click += new System.EventHandler(this.subLootAddItem_Click);
            // 
            // subLootAddSoul
            // 
            this.subLootAddSoul.Name = "subLootAddSoul";
            this.subLootAddSoul.Size = new System.Drawing.Size(164, 22);
            this.subLootAddSoul.Text = "Add Souls";
            this.subLootAddSoul.Click += new System.EventHandler(this.subLootAddSoul_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(161, 6);
            // 
            // subLootDel
            // 
            this.subLootDel.Name = "subLootDel";
            this.subLootDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subLootDel.Size = new System.Drawing.Size(164, 22);
            this.subLootDel.Text = "Delete";
            this.subLootDel.Click += new System.EventHandler(this.subLootDel_Click);
            // 
            // splitLooting
            // 
            this.splitLooting.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitLooting.Location = new System.Drawing.Point(0, 0);
            this.splitLooting.Name = "splitLooting";
            // 
            // splitLooting.Panel1
            // 
            this.splitLooting.Panel1.Controls.Add(this.cbLootFilter);
            this.splitLooting.Panel1.Controls.Add(this.lvLooting);
            // 
            // splitLooting.Panel2
            // 
            this.splitLooting.Panel2.Controls.Add(this.splitLooting2);
            this.splitLooting.Size = new System.Drawing.Size(881, 553);
            this.splitLooting.SplitterDistance = 563;
            this.splitLooting.TabIndex = 2;
            // 
            // cbLootFilter
            // 
            this.cbLootFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbLootFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbLootFilter.FormattingEnabled = true;
            this.cbLootFilter.ItemHeight = 21;
            this.cbLootFilter.Items.AddRange(new object[] {
            ""});
            this.cbLootFilter.Location = new System.Drawing.Point(12, 9);
            this.cbLootFilter.Name = "cbLootFilter";
            this.cbLootFilter.Size = new System.Drawing.Size(537, 29);
            this.cbLootFilter.TabIndex = 10;
            this.cbLootFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbLootFilter_KeyDown);
            // 
            // lvLooting
            // 
            this.lvLooting.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvLooting.AutoArrange = false;
            this.lvLooting.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colLootingID,
            this.colLootingComment,
            this.colLootingTalent,
            this.colLootingMax,
            this.colLootingItemCount});
            this.lvLooting.ContextMenuStrip = this.subLootings;
            this.lvLooting.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvLooting.FullRowSelect = true;
            this.lvLooting.HideSelection = false;
            this.lvLooting.Location = new System.Drawing.Point(0, 47);
            this.lvLooting.MultiSelect = false;
            this.lvLooting.Name = "lvLooting";
            this.lvLooting.Size = new System.Drawing.Size(563, 506);
            this.lvLooting.TabIndex = 5;
            this.lvLooting.UseCompatibleStateImageBehavior = false;
            this.lvLooting.View = System.Windows.Forms.View.Details;
            this.lvLooting.SelectedIndexChanged += new System.EventHandler(this.lvLooting_SelectedIndexChanged);
            this.lvLooting.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvLooting_KeyDown);
            // 
            // colLootingID
            // 
            this.colLootingID.Text = "ID";
            this.colLootingID.Width = 95;
            // 
            // colLootingComment
            // 
            this.colLootingComment.Text = "Comment";
            this.colLootingComment.Width = 237;
            // 
            // colLootingTalent
            // 
            this.colLootingTalent.Text = "Talent";
            this.colLootingTalent.Width = 120;
            // 
            // colLootingMax
            // 
            this.colLootingMax.Text = "Max";
            this.colLootingMax.Width = 80;
            // 
            // colLootingItemCount
            // 
            this.colLootingItemCount.Text = "Item Count";
            // 
            // splitLooting2
            // 
            this.splitLooting2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitLooting2.Location = new System.Drawing.Point(0, 0);
            this.splitLooting2.Name = "splitLooting2";
            this.splitLooting2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitLooting2.Panel1
            // 
            this.splitLooting2.Panel1.Controls.Add(this.pgLoot);
            // 
            // splitLooting2.Panel2
            // 
            this.splitLooting2.Panel2.Controls.Add(this.splitLooting3);
            this.splitLooting2.Size = new System.Drawing.Size(314, 553);
            this.splitLooting2.SplitterDistance = 115;
            this.splitLooting2.TabIndex = 0;
            // 
            // pgLoot
            // 
            this.pgLoot.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgLoot.HelpVisible = false;
            this.pgLoot.Location = new System.Drawing.Point(0, 0);
            this.pgLoot.Name = "pgLoot";
            this.pgLoot.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgLoot.Size = new System.Drawing.Size(314, 115);
            this.pgLoot.TabIndex = 0;
            this.pgLoot.ToolbarVisible = false;
            this.pgLoot.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgLoot_PropertyValueChanged);
            // 
            // splitLooting3
            // 
            this.splitLooting3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitLooting3.Location = new System.Drawing.Point(0, 0);
            this.splitLooting3.Name = "splitLooting3";
            this.splitLooting3.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitLooting3.Panel1
            // 
            this.splitLooting3.Panel1.Controls.Add(this.lvLootList);
            // 
            // splitLooting3.Panel2
            // 
            this.splitLooting3.Panel2.Controls.Add(this.pgLootableDetail);
            this.splitLooting3.Size = new System.Drawing.Size(314, 434);
            this.splitLooting3.SplitterDistance = 170;
            this.splitLooting3.TabIndex = 0;
            // 
            // lvLootList
            // 
            this.lvLootList.AutoArrange = false;
            this.lvLootList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colLDType,
            this.colLDRate,
            this.colLDMin,
            this.colLDMax});
            this.lvLootList.ContextMenuStrip = this.subLooting;
            this.lvLootList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvLootList.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvLootList.FullRowSelect = true;
            this.lvLootList.HideSelection = false;
            this.lvLootList.Location = new System.Drawing.Point(0, 0);
            this.lvLootList.MultiSelect = false;
            this.lvLootList.Name = "lvLootList";
            this.lvLootList.Size = new System.Drawing.Size(314, 170);
            this.lvLootList.TabIndex = 7;
            this.lvLootList.UseCompatibleStateImageBehavior = false;
            this.lvLootList.View = System.Windows.Forms.View.Details;
            this.lvLootList.SelectedIndexChanged += new System.EventHandler(this.lvLootList_SelectedIndexChanged);
            this.lvLootList.DoubleClick += new System.EventHandler(this.lvLootList_DoubleClick);
            this.lvLootList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvLootList_KeyDown);
            // 
            // colLDType
            // 
            this.colLDType.Text = "Type";
            this.colLDType.Width = 200;
            // 
            // colLDRate
            // 
            this.colLDRate.Text = "Rate";
            this.colLDRate.Width = 58;
            // 
            // colLDMin
            // 
            this.colLDMin.Text = "Min";
            this.colLDMin.Width = 61;
            // 
            // colLDMax
            // 
            this.colLDMax.Text = "Max";
            this.colLDMax.Width = 58;
            // 
            // pgLootableDetail
            // 
            this.pgLootableDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgLootableDetail.Location = new System.Drawing.Point(0, 0);
            this.pgLootableDetail.Name = "pgLootableDetail";
            this.pgLootableDetail.Size = new System.Drawing.Size(314, 260);
            this.pgLootableDetail.TabIndex = 0;
            this.pgLootableDetail.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgLootableDetail_PropertyValueChanged);
            // 
            // EditForm_Looting
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(881, 553);
            this.Controls.Add(this.splitLooting);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_Looting";
            this.Text = "Loot";
            this.Load += new System.EventHandler(this.EditForm_Looting_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_Looting_KeyDown);
            this.subLootings.ResumeLayout(false);
            this.subLooting.ResumeLayout(false);
            this.splitLooting.Panel1.ResumeLayout(false);
            this.splitLooting.Panel2.ResumeLayout(false);
            this.splitLooting.ResumeLayout(false);
            this.splitLooting2.Panel1.ResumeLayout(false);
            this.splitLooting2.Panel2.ResumeLayout(false);
            this.splitLooting2.ResumeLayout(false);
            this.splitLooting3.Panel1.ResumeLayout(false);
            this.splitLooting3.Panel2.ResumeLayout(false);
            this.splitLooting3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip subLootings;
        private System.Windows.Forms.ToolStripMenuItem subLootingsAdd;
        private System.Windows.Forms.ToolStripMenuItem subLootingsDel;
        private System.Windows.Forms.ContextMenuStrip subLooting;
        private System.Windows.Forms.ToolStripMenuItem subLootAddSilver;
        private System.Windows.Forms.ToolStripMenuItem subLootAddItem;
        private System.Windows.Forms.ToolStripMenuItem subLootAddSoul;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem subLootDel;
        private System.Windows.Forms.SplitContainer splitLooting;
        private System.Windows.Forms.ListView lvLooting;
        private System.Windows.Forms.ColumnHeader colLootingID;
        private System.Windows.Forms.ColumnHeader colLootingComment;
        private System.Windows.Forms.ColumnHeader colLootingTalent;
        private System.Windows.Forms.ColumnHeader colLootingMax;
        private System.Windows.Forms.ColumnHeader colLootingItemCount;
        private System.Windows.Forms.SplitContainer splitLooting2;
        private System.Windows.Forms.PropertyGrid pgLoot;
        private System.Windows.Forms.SplitContainer splitLooting3;
        private System.Windows.Forms.ListView lvLootList;
        private System.Windows.Forms.ColumnHeader colLDType;
        private System.Windows.Forms.ColumnHeader colLDRate;
        private System.Windows.Forms.ColumnHeader colLDMin;
        private System.Windows.Forms.ColumnHeader colLDMax;
        private System.Windows.Forms.PropertyGrid pgLootableDetail;
        private System.Windows.Forms.ComboBox cbLootFilter;
    }
}