namespace Velixian
{
    partial class EditForm_NPC
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
            this.splitNPC = new System.Windows.Forms.SplitContainer();
            this.cbNPCFilter = new System.Windows.Forms.ComboBox();
            this.lvNPCs = new System.Windows.Forms.ListView();
            this.colNPCID = new System.Windows.Forms.ColumnHeader();
            this.colNPCName = new System.Windows.Forms.ColumnHeader();
            this.colNPCClan = new System.Windows.Forms.ColumnHeader();
            this.colNPCICount = new System.Windows.Forms.ColumnHeader();
            this.colNPCLoot = new System.Windows.Forms.ColumnHeader();
            this.wbNPCDetail = new System.Windows.Forms.WebBrowser();
            this.splitNPC.Panel1.SuspendLayout();
            this.splitNPC.Panel2.SuspendLayout();
            this.splitNPC.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitNPC
            // 
            this.splitNPC.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitNPC.Location = new System.Drawing.Point(0, 0);
            this.splitNPC.Name = "splitNPC";
            // 
            // splitNPC.Panel1
            // 
            this.splitNPC.Panel1.Controls.Add(this.cbNPCFilter);
            this.splitNPC.Panel1.Controls.Add(this.lvNPCs);
            // 
            // splitNPC.Panel2
            // 
            this.splitNPC.Panel2.Controls.Add(this.wbNPCDetail);
            this.splitNPC.Size = new System.Drawing.Size(780, 535);
            this.splitNPC.SplitterDistance = 505;
            this.splitNPC.TabIndex = 1;
            // 
            // cbNPCFilter
            // 
            this.cbNPCFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbNPCFilter.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cbNPCFilter.FormattingEnabled = true;
            this.cbNPCFilter.ItemHeight = 21;
            this.cbNPCFilter.Items.AddRange(new object[] {
            ""});
            this.cbNPCFilter.Location = new System.Drawing.Point(12, 9);
            this.cbNPCFilter.Name = "cbNPCFilter";
            this.cbNPCFilter.Size = new System.Drawing.Size(478, 29);
            this.cbNPCFilter.TabIndex = 7;
            this.cbNPCFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbNPCFilter_KeyDown);
            // 
            // lvNPCs
            // 
            this.lvNPCs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvNPCs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colNPCID,
            this.colNPCName,
            this.colNPCClan,
            this.colNPCICount,
            this.colNPCLoot});
            this.lvNPCs.FullRowSelect = true;
            this.lvNPCs.HideSelection = false;
            this.lvNPCs.Location = new System.Drawing.Point(0, 47);
            this.lvNPCs.MultiSelect = false;
            this.lvNPCs.Name = "lvNPCs";
            this.lvNPCs.Size = new System.Drawing.Size(505, 488);
            this.lvNPCs.TabIndex = 1;
            this.lvNPCs.UseCompatibleStateImageBehavior = false;
            this.lvNPCs.View = System.Windows.Forms.View.Details;
            this.lvNPCs.SelectedIndexChanged += new System.EventHandler(this.lvNPCs_SelectedIndexChanged);
            this.lvNPCs.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvNPCs_KeyDown);
            // 
            // colNPCID
            // 
            this.colNPCID.Text = "ID";
            this.colNPCID.Width = 55;
            // 
            // colNPCName
            // 
            this.colNPCName.Text = "Name";
            this.colNPCName.Width = 110;
            // 
            // colNPCClan
            // 
            this.colNPCClan.Text = "Clan";
            this.colNPCClan.Width = 137;
            // 
            // colNPCICount
            // 
            this.colNPCICount.Text = "Count";
            this.colNPCICount.Width = 103;
            // 
            // colNPCLoot
            // 
            this.colNPCLoot.Text = "Loot";
            this.colNPCLoot.Width = 112;
            // 
            // wbNPCDetail
            // 
            this.wbNPCDetail.AllowWebBrowserDrop = false;
            this.wbNPCDetail.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbNPCDetail.IsWebBrowserContextMenuEnabled = false;
            this.wbNPCDetail.Location = new System.Drawing.Point(0, 0);
            this.wbNPCDetail.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbNPCDetail.Name = "wbNPCDetail";
            this.wbNPCDetail.Size = new System.Drawing.Size(271, 535);
            this.wbNPCDetail.TabIndex = 0;
            this.wbNPCDetail.WebBrowserShortcutsEnabled = false;
            // 
            // EditForm_NPC
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(780, 535);
            this.Controls.Add(this.splitNPC);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_NPC";
            this.Text = "NPC";
            this.Load += new System.EventHandler(this.EditForm_NPC_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_NPC_KeyDown);
            this.splitNPC.Panel1.ResumeLayout(false);
            this.splitNPC.Panel2.ResumeLayout(false);
            this.splitNPC.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitNPC;
        private System.Windows.Forms.ListView lvNPCs;
        private System.Windows.Forms.ColumnHeader colNPCID;
        private System.Windows.Forms.ColumnHeader colNPCName;
        private System.Windows.Forms.ColumnHeader colNPCClan;
        private System.Windows.Forms.ColumnHeader colNPCICount;
        private System.Windows.Forms.ColumnHeader colNPCLoot;
        private System.Windows.Forms.WebBrowser wbNPCDetail;
        private System.Windows.Forms.ComboBox cbNPCFilter;
    }
}