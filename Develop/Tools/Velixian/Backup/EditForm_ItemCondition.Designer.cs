namespace Velixian
{
    partial class EditForm_ItemCondition
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
            this.splitIConds = new System.Windows.Forms.SplitContainer();
            this.cbIConditionFilter = new System.Windows.Forms.ComboBox();
            this.lvIConditions = new System.Windows.Forms.ListView();
            this.colICondID = new System.Windows.Forms.ColumnHeader();
            this.colICondComment = new System.Windows.Forms.ColumnHeader();
            this.subItemConditions = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subItemCondsAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuICondsCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuICondsPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.subItemCondsDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitIConds2 = new System.Windows.Forms.SplitContainer();
            this.tvIConditionDetail = new System.Windows.Forms.TreeView();
            this.pgIConditionElement = new System.Windows.Forms.PropertyGrid();
            this.subItemCondition = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subItemCondAdd_And = new System.Windows.Forms.ToolStripMenuItem();
            this.subItemCondAdd_Or = new System.Windows.Forms.ToolStripMenuItem();
            this.subItemCondAdd_Cond = new System.Windows.Forms.ToolStripMenuItem();
            this.subItemCondDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitIConds.Panel1.SuspendLayout();
            this.splitIConds.Panel2.SuspendLayout();
            this.splitIConds.SuspendLayout();
            this.subItemConditions.SuspendLayout();
            this.splitIConds2.Panel1.SuspendLayout();
            this.splitIConds2.Panel2.SuspendLayout();
            this.splitIConds2.SuspendLayout();
            this.subItemCondition.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitIConds
            // 
            this.splitIConds.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitIConds.Location = new System.Drawing.Point(0, 0);
            this.splitIConds.Name = "splitIConds";
            // 
            // splitIConds.Panel1
            // 
            this.splitIConds.Panel1.Controls.Add(this.cbIConditionFilter);
            this.splitIConds.Panel1.Controls.Add(this.lvIConditions);
            // 
            // splitIConds.Panel2
            // 
            this.splitIConds.Panel2.Controls.Add(this.splitIConds2);
            this.splitIConds.Size = new System.Drawing.Size(883, 616);
            this.splitIConds.SplitterDistance = 321;
            this.splitIConds.TabIndex = 1;
            // 
            // cbIConditionFilter
            // 
            this.cbIConditionFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbIConditionFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbIConditionFilter.FormattingEnabled = true;
            this.cbIConditionFilter.ItemHeight = 21;
            this.cbIConditionFilter.Items.AddRange(new object[] {
            ""});
            this.cbIConditionFilter.Location = new System.Drawing.Point(12, 9);
            this.cbIConditionFilter.Name = "cbIConditionFilter";
            this.cbIConditionFilter.Size = new System.Drawing.Size(294, 29);
            this.cbIConditionFilter.TabIndex = 9;
            this.cbIConditionFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbIConditionFilter_KeyDown);
            // 
            // lvIConditions
            // 
            this.lvIConditions.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvIConditions.AutoArrange = false;
            this.lvIConditions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colICondID,
            this.colICondComment});
            this.lvIConditions.ContextMenuStrip = this.subItemConditions;
            this.lvIConditions.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvIConditions.FullRowSelect = true;
            this.lvIConditions.HideSelection = false;
            this.lvIConditions.Location = new System.Drawing.Point(0, 47);
            this.lvIConditions.MultiSelect = false;
            this.lvIConditions.Name = "lvIConditions";
            this.lvIConditions.Size = new System.Drawing.Size(321, 569);
            this.lvIConditions.TabIndex = 3;
            this.lvIConditions.UseCompatibleStateImageBehavior = false;
            this.lvIConditions.View = System.Windows.Forms.View.Details;
            this.lvIConditions.SelectedIndexChanged += new System.EventHandler(this.lvIConditions_SelectedIndexChanged);
            this.lvIConditions.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvIConditions_KeyDown);
            // 
            // colICondID
            // 
            this.colICondID.Text = "ID";
            this.colICondID.Width = 86;
            // 
            // colICondComment
            // 
            this.colICondComment.Text = "Comment";
            this.colICondComment.Width = 300;
            // 
            // subItemConditions
            // 
            this.subItemConditions.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subItemCondsAdd,
            this.toolStripMenuItem1,
            this.mnuICondsCopy,
            this.mnuICondsPaste,
            this.toolStripMenuItem2,
            this.subItemCondsDel});
            this.subItemConditions.Name = "subConditions";
            this.subItemConditions.Size = new System.Drawing.Size(139, 104);
            this.subItemConditions.Opening += new System.ComponentModel.CancelEventHandler(this.subItemConditions_Opening);
            // 
            // subItemCondsAdd
            // 
            this.subItemCondsAdd.Name = "subItemCondsAdd";
            this.subItemCondsAdd.Size = new System.Drawing.Size(138, 22);
            this.subItemCondsAdd.Text = "추가(&A)";
            this.subItemCondsAdd.Click += new System.EventHandler(this.subItemCondsAdd_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(135, 6);
            // 
            // mnuICondsCopy
            // 
            this.mnuICondsCopy.Name = "mnuICondsCopy";
            this.mnuICondsCopy.Size = new System.Drawing.Size(138, 22);
            this.mnuICondsCopy.Text = "복사(&C)";
            this.mnuICondsCopy.Click += new System.EventHandler(this.mnuICondsCopy_Click);
            // 
            // mnuICondsPaste
            // 
            this.mnuICondsPaste.Name = "mnuICondsPaste";
            this.mnuICondsPaste.Size = new System.Drawing.Size(138, 22);
            this.mnuICondsPaste.Text = "붙여넣기(&P)";
            this.mnuICondsPaste.Click += new System.EventHandler(this.mnuICondsPaste_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(135, 6);
            // 
            // subItemCondsDel
            // 
            this.subItemCondsDel.Name = "subItemCondsDel";
            this.subItemCondsDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subItemCondsDel.Size = new System.Drawing.Size(138, 22);
            this.subItemCondsDel.Text = "삭제(&D)";
            this.subItemCondsDel.Click += new System.EventHandler(this.subItemCondsDel_Click);
            // 
            // splitIConds2
            // 
            this.splitIConds2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitIConds2.Location = new System.Drawing.Point(0, 0);
            this.splitIConds2.Name = "splitIConds2";
            this.splitIConds2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitIConds2.Panel1
            // 
            this.splitIConds2.Panel1.Controls.Add(this.tvIConditionDetail);
            // 
            // splitIConds2.Panel2
            // 
            this.splitIConds2.Panel2.Controls.Add(this.pgIConditionElement);
            this.splitIConds2.Size = new System.Drawing.Size(558, 616);
            this.splitIConds2.SplitterDistance = 295;
            this.splitIConds2.TabIndex = 0;
            // 
            // tvIConditionDetail
            // 
            this.tvIConditionDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvIConditionDetail.Location = new System.Drawing.Point(0, 0);
            this.tvIConditionDetail.Name = "tvIConditionDetail";
            this.tvIConditionDetail.Size = new System.Drawing.Size(558, 295);
            this.tvIConditionDetail.TabIndex = 4;
            this.tvIConditionDetail.MouseUp += new System.Windows.Forms.MouseEventHandler(this.tvConditionDetail_MouseUp);
            this.tvIConditionDetail.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvCondDetail_AfterSelect);
            // 
            // pgIConditionElement
            // 
            this.pgIConditionElement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgIConditionElement.Location = new System.Drawing.Point(0, 0);
            this.pgIConditionElement.Name = "pgIConditionElement";
            this.pgIConditionElement.Size = new System.Drawing.Size(558, 317);
            this.pgIConditionElement.TabIndex = 3;
            this.pgIConditionElement.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgConditionElement_PropertyValueChanged);
            // 
            // subItemCondition
            // 
            this.subItemCondition.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subItemCondAdd_And,
            this.subItemCondAdd_Or,
            this.subItemCondAdd_Cond,
            this.subItemCondDel});
            this.subItemCondition.Name = "subCondition";
            this.subItemCondition.Size = new System.Drawing.Size(178, 92);
            this.subItemCondition.Opening += new System.ComponentModel.CancelEventHandler(this.subItemCondition_Opening);
            // 
            // subItemCondAdd_And
            // 
            this.subItemCondAdd_And.Name = "subItemCondAdd_And";
            this.subItemCondAdd_And.Size = new System.Drawing.Size(177, 22);
            this.subItemCondAdd_And.Text = "And 추가(&A)";
            this.subItemCondAdd_And.Click += new System.EventHandler(this.subItemCondAdd_And_Click);
            // 
            // subItemCondAdd_Or
            // 
            this.subItemCondAdd_Or.Name = "subItemCondAdd_Or";
            this.subItemCondAdd_Or.Size = new System.Drawing.Size(177, 22);
            this.subItemCondAdd_Or.Text = "Or 추가(&A)";
            this.subItemCondAdd_Or.Click += new System.EventHandler(this.subItemCondAdd_Or_Click);
            // 
            // subItemCondAdd_Cond
            // 
            this.subItemCondAdd_Cond.Name = "subItemCondAdd_Cond";
            this.subItemCondAdd_Cond.Size = new System.Drawing.Size(177, 22);
            this.subItemCondAdd_Cond.Text = "Cond 추가(&A)";
            this.subItemCondAdd_Cond.Click += new System.EventHandler(this.subItemCondAdd_Cond_Click);
            // 
            // subItemCondDel
            // 
            this.subItemCondDel.Name = "subItemCondDel";
            this.subItemCondDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subItemCondDel.Size = new System.Drawing.Size(177, 22);
            this.subItemCondDel.Text = "조건문 삭제(&D)";
            this.subItemCondDel.Click += new System.EventHandler(this.subItemCondDel_Click);
            // 
            // EditForm_ItemCondition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(883, 616);
            this.Controls.Add(this.splitIConds);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_ItemCondition";
            this.Text = "Item Conditions";
            this.Load += new System.EventHandler(this.EditForm_ItemCondition_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_ItemCondition_KeyDown);
            this.splitIConds.Panel1.ResumeLayout(false);
            this.splitIConds.Panel2.ResumeLayout(false);
            this.splitIConds.ResumeLayout(false);
            this.subItemConditions.ResumeLayout(false);
            this.splitIConds2.Panel1.ResumeLayout(false);
            this.splitIConds2.Panel2.ResumeLayout(false);
            this.splitIConds2.ResumeLayout(false);
            this.subItemCondition.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitIConds;
        private System.Windows.Forms.ListView lvIConditions;
        private System.Windows.Forms.ColumnHeader colICondID;
        private System.Windows.Forms.ColumnHeader colICondComment;
        private System.Windows.Forms.SplitContainer splitIConds2;
        private System.Windows.Forms.TreeView tvIConditionDetail;
        private System.Windows.Forms.PropertyGrid pgIConditionElement;
        private System.Windows.Forms.ContextMenuStrip subItemConditions;
        private System.Windows.Forms.ToolStripMenuItem subItemCondsAdd;
        private System.Windows.Forms.ToolStripMenuItem subItemCondsDel;
        private System.Windows.Forms.ContextMenuStrip subItemCondition;
        private System.Windows.Forms.ToolStripMenuItem subItemCondAdd_And;
        private System.Windows.Forms.ToolStripMenuItem subItemCondAdd_Or;
        private System.Windows.Forms.ToolStripMenuItem subItemCondAdd_Cond;
        private System.Windows.Forms.ToolStripMenuItem subItemCondDel;
        private System.Windows.Forms.ComboBox cbIConditionFilter;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem mnuICondsCopy;
        private System.Windows.Forms.ToolStripMenuItem mnuICondsPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
    }
}