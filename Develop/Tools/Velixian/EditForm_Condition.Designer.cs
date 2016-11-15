namespace Velixian
{
    partial class EditForm_Condition
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
            this.splitCond = new System.Windows.Forms.SplitContainer();
            this.cbConditionFilter = new System.Windows.Forms.ComboBox();
            this.lvConditions = new System.Windows.Forms.ListView();
            this.colCondID = new System.Windows.Forms.ColumnHeader();
            this.colCondComment = new System.Windows.Forms.ColumnHeader();
            this.subConditions = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subCondsAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.subCondCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.subCondPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.subCondsDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitCond2 = new System.Windows.Forms.SplitContainer();
            this.tvConditionDetail = new System.Windows.Forms.TreeView();
            this.pgConditionElement = new System.Windows.Forms.PropertyGrid();
            this.subCondition = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subCondAdd_And = new System.Windows.Forms.ToolStripMenuItem();
            this.subCondAdd_Or = new System.Windows.Forms.ToolStripMenuItem();
            this.subCondAdd_Cond = new System.Windows.Forms.ToolStripMenuItem();
            this.subCondDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitCond.Panel1.SuspendLayout();
            this.splitCond.Panel2.SuspendLayout();
            this.splitCond.SuspendLayout();
            this.subConditions.SuspendLayout();
            this.splitCond2.Panel1.SuspendLayout();
            this.splitCond2.Panel2.SuspendLayout();
            this.splitCond2.SuspendLayout();
            this.subCondition.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitCond
            // 
            this.splitCond.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitCond.Location = new System.Drawing.Point(0, 0);
            this.splitCond.Name = "splitCond";
            // 
            // splitCond.Panel1
            // 
            this.splitCond.Panel1.Controls.Add(this.cbConditionFilter);
            this.splitCond.Panel1.Controls.Add(this.lvConditions);
            // 
            // splitCond.Panel2
            // 
            this.splitCond.Panel2.Controls.Add(this.splitCond2);
            this.splitCond.Size = new System.Drawing.Size(848, 509);
            this.splitCond.SplitterDistance = 308;
            this.splitCond.TabIndex = 1;
            // 
            // cbConditionFilter
            // 
            this.cbConditionFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbConditionFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbConditionFilter.FormattingEnabled = true;
            this.cbConditionFilter.ItemHeight = 21;
            this.cbConditionFilter.Items.AddRange(new object[] {
            ""});
            this.cbConditionFilter.Location = new System.Drawing.Point(12, 9);
            this.cbConditionFilter.Name = "cbConditionFilter";
            this.cbConditionFilter.Size = new System.Drawing.Size(280, 29);
            this.cbConditionFilter.TabIndex = 8;
            this.cbConditionFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbConditionFilter_KeyDown);
            // 
            // lvConditions
            // 
            this.lvConditions.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvConditions.AutoArrange = false;
            this.lvConditions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colCondID,
            this.colCondComment});
            this.lvConditions.ContextMenuStrip = this.subConditions;
            this.lvConditions.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvConditions.FullRowSelect = true;
            this.lvConditions.HideSelection = false;
            this.lvConditions.Location = new System.Drawing.Point(0, 47);
            this.lvConditions.MultiSelect = false;
            this.lvConditions.Name = "lvConditions";
            this.lvConditions.Size = new System.Drawing.Size(308, 462);
            this.lvConditions.TabIndex = 2;
            this.lvConditions.UseCompatibleStateImageBehavior = false;
            this.lvConditions.View = System.Windows.Forms.View.Details;
            this.lvConditions.SelectedIndexChanged += new System.EventHandler(this.lvConditions_SelectedIndexChanged);
            this.lvConditions.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvConditions_KeyDown);
            // 
            // colCondID
            // 
            this.colCondID.Text = "ID";
            this.colCondID.Width = 86;
            // 
            // colCondComment
            // 
            this.colCondComment.Text = "Comment";
            this.colCondComment.Width = 300;
            // 
            // subConditions
            // 
            this.subConditions.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subCondsAdd,
            this.toolStripMenuItem2,
            this.subCondCopy,
            this.subCondPaste,
            this.toolStripMenuItem1,
            this.subCondsDel});
            this.subConditions.Name = "subConditions";
            this.subConditions.Size = new System.Drawing.Size(139, 104);
            this.subConditions.Opening += new System.ComponentModel.CancelEventHandler(this.subConditions_Opening);
            // 
            // subCondsAdd
            // 
            this.subCondsAdd.Name = "subCondsAdd";
            this.subCondsAdd.Size = new System.Drawing.Size(138, 22);
            this.subCondsAdd.Text = "Add(&A)";
            this.subCondsAdd.Click += new System.EventHandler(this.subCondsAdd_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(135, 6);
            // 
            // subCondCopy
            // 
            this.subCondCopy.Name = "subCondCopy";
            this.subCondCopy.Size = new System.Drawing.Size(138, 22);
            this.subCondCopy.Text = "Copy(&C)";
            this.subCondCopy.Click += new System.EventHandler(this.subCondCopy_Click);
            // 
            // subCondPaste
            // 
            this.subCondPaste.Name = "subCondPaste";
            this.subCondPaste.Size = new System.Drawing.Size(138, 22);
            this.subCondPaste.Text = "Paste(&P)";
            this.subCondPaste.Click += new System.EventHandler(this.subCondPaste_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(135, 6);
            // 
            // subCondsDel
            // 
            this.subCondsDel.Name = "subCondsDel";
            this.subCondsDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subCondsDel.Size = new System.Drawing.Size(138, 22);
            this.subCondsDel.Text = "Delete(&D)";
            this.subCondsDel.Click += new System.EventHandler(this.subCondsDel_Click);
            // 
            // splitCond2
            // 
            this.splitCond2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitCond2.Location = new System.Drawing.Point(0, 0);
            this.splitCond2.Name = "splitCond2";
            this.splitCond2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitCond2.Panel1
            // 
            this.splitCond2.Panel1.Controls.Add(this.tvConditionDetail);
            // 
            // splitCond2.Panel2
            // 
            this.splitCond2.Panel2.Controls.Add(this.pgConditionElement);
            this.splitCond2.Size = new System.Drawing.Size(536, 509);
            this.splitCond2.SplitterDistance = 251;
            this.splitCond2.TabIndex = 1;
            // 
            // tvConditionDetail
            // 
            this.tvConditionDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvConditionDetail.Location = new System.Drawing.Point(0, 0);
            this.tvConditionDetail.Name = "tvConditionDetail";
            this.tvConditionDetail.Size = new System.Drawing.Size(536, 251);
            this.tvConditionDetail.TabIndex = 4;
            this.tvConditionDetail.MouseUp += new System.Windows.Forms.MouseEventHandler(this.tvConditionDetail_MouseUp);
            this.tvConditionDetail.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvCondDetail_AfterSelect);
            // 
            // pgConditionElement
            // 
            this.pgConditionElement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgConditionElement.Location = new System.Drawing.Point(0, 0);
            this.pgConditionElement.Name = "pgConditionElement";
            this.pgConditionElement.Size = new System.Drawing.Size(536, 254);
            this.pgConditionElement.TabIndex = 5;
            this.pgConditionElement.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgConditionElement_PropertyValueChanged);
            // 
            // subCondition
            // 
            this.subCondition.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subCondAdd_And,
            this.subCondAdd_Or,
            this.subCondAdd_Cond,
            this.subCondDel});
            this.subCondition.Name = "subCondition";
            this.subCondition.Size = new System.Drawing.Size(178, 92);
            this.subCondition.Opening += new System.ComponentModel.CancelEventHandler(this.subCondition_Opening);
            // 
            // subCondAdd_And
            // 
            this.subCondAdd_And.Name = "subCondAdd_And";
            this.subCondAdd_And.Size = new System.Drawing.Size(177, 22);
            this.subCondAdd_And.Text = "And Add(&A)";
            this.subCondAdd_And.Click += new System.EventHandler(this.subCondAdd_And_Click);
            // 
            // subCondAdd_Or
            // 
            this.subCondAdd_Or.Name = "subCondAdd_Or";
            this.subCondAdd_Or.Size = new System.Drawing.Size(177, 22);
            this.subCondAdd_Or.Text = "Or Add(&A)";
            this.subCondAdd_Or.Click += new System.EventHandler(this.subCondAdd_Or_Click);
            // 
            // subCondAdd_Cond
            // 
            this.subCondAdd_Cond.Name = "subCondAdd_Cond";
            this.subCondAdd_Cond.Size = new System.Drawing.Size(177, 22);
            this.subCondAdd_Cond.Text = "Cond Add(&A)";
            this.subCondAdd_Cond.Click += new System.EventHandler(this.subCondAdd_Cond_Click);
            // 
            // subCondDel
            // 
            this.subCondDel.Name = "subCondDel";
            this.subCondDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subCondDel.Size = new System.Drawing.Size(177, 22);
            this.subCondDel.Text = "Delete Condition(&D)";
            this.subCondDel.Click += new System.EventHandler(this.subCondDel_Click);
            // 
            // EditForm_Condition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(848, 509);
            this.Controls.Add(this.splitCond);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_Condition";
            this.Text = "Conditions";
            this.Load += new System.EventHandler(this.EditForm_Condition_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_Condition_KeyDown);
            this.splitCond.Panel1.ResumeLayout(false);
            this.splitCond.Panel2.ResumeLayout(false);
            this.splitCond.ResumeLayout(false);
            this.subConditions.ResumeLayout(false);
            this.splitCond2.Panel1.ResumeLayout(false);
            this.splitCond2.Panel2.ResumeLayout(false);
            this.splitCond2.ResumeLayout(false);
            this.subCondition.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitCond;
        private System.Windows.Forms.ListView lvConditions;
        private System.Windows.Forms.ColumnHeader colCondID;
        private System.Windows.Forms.ColumnHeader colCondComment;
        private System.Windows.Forms.SplitContainer splitCond2;
        private System.Windows.Forms.TreeView tvConditionDetail;
        private System.Windows.Forms.PropertyGrid pgConditionElement;
        private System.Windows.Forms.ContextMenuStrip subCondition;
        private System.Windows.Forms.ToolStripMenuItem subCondAdd_And;
        private System.Windows.Forms.ToolStripMenuItem subCondAdd_Or;
        private System.Windows.Forms.ToolStripMenuItem subCondAdd_Cond;
        private System.Windows.Forms.ToolStripMenuItem subCondDel;
        private System.Windows.Forms.ContextMenuStrip subConditions;
        private System.Windows.Forms.ToolStripMenuItem subCondsAdd;
        private System.Windows.Forms.ToolStripMenuItem subCondsDel;
        private System.Windows.Forms.ComboBox cbConditionFilter;
        private System.Windows.Forms.ToolStripMenuItem subCondCopy;
        private System.Windows.Forms.ToolStripMenuItem subCondPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
    }
}