namespace Velixian
{
    partial class EditForm_Item
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
            this.splitItem = new System.Windows.Forms.SplitContainer();
            this.cbItemFilter = new System.Windows.Forms.ComboBox();
            this.lvItems = new System.Windows.Forms.ListView();
            this.colItemsID = new System.Windows.Forms.ColumnHeader();
            this.colItemsName = new System.Windows.Forms.ColumnHeader();
            this.colItemsDesc = new System.Windows.Forms.ColumnHeader();
            this.colItemsCond = new System.Windows.Forms.ColumnHeader();
            this.colItemsSex = new System.Windows.Forms.ColumnHeader();
            this.pgItemDetail = new System.Windows.Forms.PropertyGrid();
            this.splitItem.Panel1.SuspendLayout();
            this.splitItem.Panel2.SuspendLayout();
            this.splitItem.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitItem
            // 
            this.splitItem.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitItem.Location = new System.Drawing.Point(0, 0);
            this.splitItem.Name = "splitItem";
            // 
            // splitItem.Panel1
            // 
            this.splitItem.Panel1.Controls.Add(this.cbItemFilter);
            this.splitItem.Panel1.Controls.Add(this.lvItems);
            // 
            // splitItem.Panel2
            // 
            this.splitItem.Panel2.Controls.Add(this.pgItemDetail);
            this.splitItem.Size = new System.Drawing.Size(803, 521);
            this.splitItem.SplitterDistance = 529;
            this.splitItem.TabIndex = 1;
            // 
            // cbItemFilter
            // 
            this.cbItemFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbItemFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbItemFilter.FormattingEnabled = true;
            this.cbItemFilter.ItemHeight = 21;
            this.cbItemFilter.Items.AddRange(new object[] {
            ""});
            this.cbItemFilter.Location = new System.Drawing.Point(12, 9);
            this.cbItemFilter.Name = "cbItemFilter";
            this.cbItemFilter.Size = new System.Drawing.Size(502, 29);
            this.cbItemFilter.TabIndex = 9;
            this.cbItemFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbItemFilter_KeyDown);
            // 
            // lvItems
            // 
            this.lvItems.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvItems.AutoArrange = false;
            this.lvItems.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colItemsID,
            this.colItemsName,
            this.colItemsDesc,
            this.colItemsCond,
            this.colItemsSex});
            this.lvItems.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvItems.FullRowSelect = true;
            this.lvItems.HideSelection = false;
            this.lvItems.Location = new System.Drawing.Point(0, 47);
            this.lvItems.MultiSelect = false;
            this.lvItems.Name = "lvItems";
            this.lvItems.Size = new System.Drawing.Size(529, 474);
            this.lvItems.TabIndex = 4;
            this.lvItems.UseCompatibleStateImageBehavior = false;
            this.lvItems.View = System.Windows.Forms.View.Details;
            this.lvItems.SelectedIndexChanged += new System.EventHandler(this.lvItems_SelectedIndexChanged);
            this.lvItems.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvItems_KeyDown);
            // 
            // colItemsID
            // 
            this.colItemsID.Text = "ID";
            this.colItemsID.Width = 62;
            // 
            // colItemsName
            // 
            this.colItemsName.Text = "Name";
            this.colItemsName.Width = 129;
            // 
            // colItemsDesc
            // 
            this.colItemsDesc.Text = "Desc";
            this.colItemsDesc.Width = 230;
            // 
            // colItemsCond
            // 
            this.colItemsCond.Text = "Conditions";
            this.colItemsCond.Width = 88;
            // 
            // colItemsSex
            // 
            this.colItemsSex.Text = "Sex";
            // 
            // pgItemDetail
            // 
            this.pgItemDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgItemDetail.Location = new System.Drawing.Point(0, 0);
            this.pgItemDetail.Name = "pgItemDetail";
            this.pgItemDetail.Size = new System.Drawing.Size(270, 521);
            this.pgItemDetail.TabIndex = 0;
            // 
            // EditForm_Item
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(803, 521);
            this.Controls.Add(this.splitItem);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_Item";
            this.Text = "Items";
            this.Load += new System.EventHandler(this.EditForm_Item_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_Item_KeyDown);
            this.splitItem.Panel1.ResumeLayout(false);
            this.splitItem.Panel2.ResumeLayout(false);
            this.splitItem.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitItem;
        private System.Windows.Forms.ListView lvItems;
        private System.Windows.Forms.ColumnHeader colItemsID;
        private System.Windows.Forms.ColumnHeader colItemsName;
        private System.Windows.Forms.ColumnHeader colItemsDesc;
        private System.Windows.Forms.ColumnHeader colItemsCond;
        private System.Windows.Forms.ColumnHeader colItemsSex;
        private System.Windows.Forms.PropertyGrid pgItemDetail;
        private System.Windows.Forms.ComboBox cbItemFilter;
    }
}