namespace Velixian
{
    partial class EditForm_Dialog
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
            this.subDialogDetail = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subDialogDetailAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.subDialogDetailDel = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.subDialogDetailAddItemAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.subDialogDetailAddItemRemove = new System.Windows.Forms.ToolStripMenuItem();
            this.subDialogDetailAddItemFill = new System.Windows.Forms.ToolStripMenuItem();
            this.subDialog = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.subDialogAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.subDialogDel = new System.Windows.Forms.ToolStripMenuItem();
            this.splitDialog = new System.Windows.Forms.SplitContainer();
            this.cbDialogFilter = new System.Windows.Forms.ComboBox();
            this.lvDialogs = new System.Windows.Forms.ListView();
            this.colDialogsID = new System.Windows.Forms.ColumnHeader();
            this.colDialogsText = new System.Windows.Forms.ColumnHeader();
            this.colDialogsType = new System.Windows.Forms.ColumnHeader();
            this.splitDialog2 = new System.Windows.Forms.SplitContainer();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.wbDialogDetail = new System.Windows.Forms.WebBrowser();
            this.tvDialogDetail = new System.Windows.Forms.TreeView();
            this.pgDialogElement = new System.Windows.Forms.PropertyGrid();
            this.subDialogDetail.SuspendLayout();
            this.subDialog.SuspendLayout();
            this.splitDialog.Panel1.SuspendLayout();
            this.splitDialog.Panel2.SuspendLayout();
            this.splitDialog.SuspendLayout();
            this.splitDialog2.Panel1.SuspendLayout();
            this.splitDialog2.Panel2.SuspendLayout();
            this.splitDialog2.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // subDialogDetail
            // 
            this.subDialogDetail.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subDialogDetailAdd,
            this.subDialogDetailDel,
            this.toolStripMenuItem1,
            this.subDialogDetailAddItemAdd,
            this.subDialogDetailAddItemRemove,
            this.subDialogDetailAddItemFill});
            this.subDialogDetail.Name = "subDialog";
            this.subDialogDetail.Size = new System.Drawing.Size(242, 120);
            this.subDialogDetail.Opening += new System.ComponentModel.CancelEventHandler(this.subDialogDetail_Opening);
            // 
            // subDialogDetailAdd
            // 
            this.subDialogDetailAdd.Name = "subDialogDetailAdd";
            this.subDialogDetailAdd.Size = new System.Drawing.Size(241, 22);
            this.subDialogDetailAdd.Text = "Add Item SELECT or SAY (&A)";
            this.subDialogDetailAdd.Click += new System.EventHandler(this.subDialogDetailAdd_Click);
            // 
            // subDialogDetailDel
            // 
            this.subDialogDetailDel.Name = "subDialogDetailDel";
            this.subDialogDetailDel.ShortcutKeyDisplayString = "Del";
            this.subDialogDetailDel.Size = new System.Drawing.Size(241, 22);
            this.subDialogDetailDel.Text = "Delete(&D)";
            this.subDialogDetailDel.Click += new System.EventHandler(this.subDialogDetailDel_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(238, 6);
            // 
            // subDialogDetailAddItemAdd
            // 
            this.subDialogDetailAddItemAdd.Name = "subDialogDetailAddItemAdd";
            this.subDialogDetailAddItemAdd.Size = new System.Drawing.Size(241, 22);
            this.subDialogDetailAddItemAdd.Text = "ITEM Add";
            this.subDialogDetailAddItemAdd.Click += new System.EventHandler(this.subDialogDetailAddItemAdd_Click);
            // 
            // subDialogDetailAddItemRemove
            // 
            this.subDialogDetailAddItemRemove.Name = "subDialogDetailAddItemRemove";
            this.subDialogDetailAddItemRemove.Size = new System.Drawing.Size(241, 22);
            this.subDialogDetailAddItemRemove.Text = "ITEM Remove";
            this.subDialogDetailAddItemRemove.Click += new System.EventHandler(this.subDialogDetailAddItemRemove_Click);
            // 
            // subDialogDetailAddItemFill
            // 
            this.subDialogDetailAddItemFill.Name = "subDialogDetailAddItemFill";
            this.subDialogDetailAddItemFill.Size = new System.Drawing.Size(241, 22);
            this.subDialogDetailAddItemFill.Text = "ITEM Fill";
            this.subDialogDetailAddItemFill.Click += new System.EventHandler(this.subDialogDetailAddItemFill_Click);
            // 
            // subDialog
            // 
            this.subDialog.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.subDialogAdd,
            this.subDialogDel});
            this.subDialog.Name = "subDialog";
            this.subDialog.Size = new System.Drawing.Size(178, 48);
            this.subDialog.Opening += new System.ComponentModel.CancelEventHandler(this.subDialog_Opening);
            // 
            // subDialogAdd
            // 
            this.subDialogAdd.Name = "subDialogAdd";
            this.subDialogAdd.Size = new System.Drawing.Size(177, 22);
            this.subDialogAdd.Text = "Add Dialog(&A)";
            this.subDialogAdd.Click += new System.EventHandler(this.subDialogAdd_Click);
            // 
            // subDialogDel
            // 
            this.subDialogDel.Name = "subDialogDel";
            this.subDialogDel.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.subDialogDel.Size = new System.Drawing.Size(177, 22);
            this.subDialogDel.Text = "Delete Dialog(&D)";
            this.subDialogDel.Click += new System.EventHandler(this.subDialogDel_Click);
            // 
            // splitDialog
            // 
            this.splitDialog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitDialog.Location = new System.Drawing.Point(0, 0);
            this.splitDialog.Name = "splitDialog";
            // 
            // splitDialog.Panel1
            // 
            this.splitDialog.Panel1.Controls.Add(this.cbDialogFilter);
            this.splitDialog.Panel1.Controls.Add(this.lvDialogs);
            // 
            // splitDialog.Panel2
            // 
            this.splitDialog.Panel2.Controls.Add(this.splitDialog2);
            this.splitDialog.Size = new System.Drawing.Size(943, 563);
            this.splitDialog.SplitterDistance = 454;
            this.splitDialog.TabIndex = 2;
            // 
            // cbDialogFilter
            // 
            this.cbDialogFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDialogFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbDialogFilter.FormattingEnabled = true;
            this.cbDialogFilter.ItemHeight = 21;
            this.cbDialogFilter.Items.AddRange(new object[] {
            ""});
            this.cbDialogFilter.Location = new System.Drawing.Point(12, 9);
            this.cbDialogFilter.Name = "cbDialogFilter";
            this.cbDialogFilter.Size = new System.Drawing.Size(429, 29);
            this.cbDialogFilter.TabIndex = 10;
            this.cbDialogFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbDialogFilter_KeyDown);
            // 
            // lvDialogs
            // 
            this.lvDialogs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvDialogs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colDialogsID,
            this.colDialogsText,
            this.colDialogsType});
            this.lvDialogs.ContextMenuStrip = this.subDialog;
            this.lvDialogs.FullRowSelect = true;
            this.lvDialogs.HideSelection = false;
            this.lvDialogs.Location = new System.Drawing.Point(0, 47);
            this.lvDialogs.MultiSelect = false;
            this.lvDialogs.Name = "lvDialogs";
            this.lvDialogs.Size = new System.Drawing.Size(454, 516);
            this.lvDialogs.TabIndex = 1;
            this.lvDialogs.UseCompatibleStateImageBehavior = false;
            this.lvDialogs.View = System.Windows.Forms.View.Details;
            this.lvDialogs.SelectedIndexChanged += new System.EventHandler(this.lvDialogs_SelectedIndexChanged);
            this.lvDialogs.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvDialogs_KeyDown);
            // 
            // colDialogsID
            // 
            this.colDialogsID.Text = "ID";
            this.colDialogsID.Width = 78;
            // 
            // colDialogsText
            // 
            this.colDialogsText.Text = "Text";
            this.colDialogsText.Width = 177;
            // 
            // colDialogsType
            // 
            this.colDialogsType.Text = "Type";
            this.colDialogsType.Width = 261;
            // 
            // splitDialog2
            // 
            this.splitDialog2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitDialog2.Location = new System.Drawing.Point(0, 0);
            this.splitDialog2.Name = "splitDialog2";
            this.splitDialog2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitDialog2.Panel1
            // 
            this.splitDialog2.Panel1.Controls.Add(this.splitContainer1);
            // 
            // splitDialog2.Panel2
            // 
            this.splitDialog2.Panel2.Controls.Add(this.pgDialogElement);
            this.splitDialog2.Size = new System.Drawing.Size(485, 563);
            this.splitDialog2.SplitterDistance = 328;
            this.splitDialog2.TabIndex = 0;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.wbDialogDetail);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tvDialogDetail);
            this.splitContainer1.Size = new System.Drawing.Size(485, 328);
            this.splitContainer1.SplitterDistance = 111;
            this.splitContainer1.TabIndex = 1;
            // 
            // wbDialogDetail
            // 
            this.wbDialogDetail.AllowWebBrowserDrop = false;
            this.wbDialogDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbDialogDetail.IsWebBrowserContextMenuEnabled = false;
            this.wbDialogDetail.Location = new System.Drawing.Point(0, 0);
            this.wbDialogDetail.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbDialogDetail.Name = "wbDialogDetail";
            this.wbDialogDetail.Size = new System.Drawing.Size(485, 111);
            this.wbDialogDetail.TabIndex = 1;
            this.wbDialogDetail.WebBrowserShortcutsEnabled = false;
            // 
            // tvDialogDetail
            // 
            this.tvDialogDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvDialogDetail.HideSelection = false;
            this.tvDialogDetail.Location = new System.Drawing.Point(0, 0);
            this.tvDialogDetail.Name = "tvDialogDetail";
            this.tvDialogDetail.ShowRootLines = false;
            this.tvDialogDetail.Size = new System.Drawing.Size(485, 213);
            this.tvDialogDetail.TabIndex = 1;
            this.tvDialogDetail.DoubleClick += new System.EventHandler(this.tvDialogDetail_DoubleClick);
            this.tvDialogDetail.MouseUp += new System.Windows.Forms.MouseEventHandler(this.tvDialogDetail_MouseUp);
            this.tvDialogDetail.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvDialogDetail_AfterSelect);
            this.tvDialogDetail.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tvDialogDetail_KeyDown);
            // 
            // pgDialogElement
            // 
            this.pgDialogElement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgDialogElement.Location = new System.Drawing.Point(0, 0);
            this.pgDialogElement.Name = "pgDialogElement";
            this.pgDialogElement.Size = new System.Drawing.Size(485, 231);
            this.pgDialogElement.TabIndex = 0;
            this.pgDialogElement.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgDialogElement_PropertyValueChanged);
            // 
            // EditForm_Dialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(943, 563);
            this.Controls.Add(this.splitDialog);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_Dialog";
            this.Text = "Dialogs";
            this.Load += new System.EventHandler(this.EditForm_Dialog_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_Dialog_KeyDown);
            this.subDialogDetail.ResumeLayout(false);
            this.subDialog.ResumeLayout(false);
            this.splitDialog.Panel1.ResumeLayout(false);
            this.splitDialog.Panel2.ResumeLayout(false);
            this.splitDialog.ResumeLayout(false);
            this.splitDialog2.Panel1.ResumeLayout(false);
            this.splitDialog2.Panel2.ResumeLayout(false);
            this.splitDialog2.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip subDialogDetail;
        private System.Windows.Forms.ToolStripMenuItem subDialogDetailAdd;
        private System.Windows.Forms.ToolStripMenuItem subDialogDetailDel;
        private System.Windows.Forms.ContextMenuStrip subDialog;
        private System.Windows.Forms.ToolStripMenuItem subDialogAdd;
        private System.Windows.Forms.ToolStripMenuItem subDialogDel;
        private System.Windows.Forms.SplitContainer splitDialog;
        private System.Windows.Forms.ListView lvDialogs;
        private System.Windows.Forms.ColumnHeader colDialogsID;
        private System.Windows.Forms.ColumnHeader colDialogsText;
        private System.Windows.Forms.ColumnHeader colDialogsType;
        private System.Windows.Forms.SplitContainer splitDialog2;
        private System.Windows.Forms.PropertyGrid pgDialogElement;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView tvDialogDetail;
        private System.Windows.Forms.WebBrowser wbDialogDetail;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem subDialogDetailAddItemAdd;
        private System.Windows.Forms.ToolStripMenuItem subDialogDetailAddItemRemove;
        private System.Windows.Forms.ToolStripMenuItem subDialogDetailAddItemFill;
        private System.Windows.Forms.ComboBox cbDialogFilter;
    }
}