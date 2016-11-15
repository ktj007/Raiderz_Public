namespace Velixian
{
    partial class EditForm_String
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
            this.splitStrings = new System.Windows.Forms.SplitContainer();
            this.lvStrings = new System.Windows.Forms.ListView();
            this.colStringsKey = new System.Windows.Forms.ColumnHeader();
            this.colStringsStr = new System.Windows.Forms.ColumnHeader();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.tbStringKey = new System.Windows.Forms.TextBox();
            this.tbStringValue = new System.Windows.Forms.TextBox();
            this.splitStrings.Panel1.SuspendLayout();
            this.splitStrings.Panel2.SuspendLayout();
            this.splitStrings.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitStrings
            // 
            this.splitStrings.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitStrings.Location = new System.Drawing.Point(0, 0);
            this.splitStrings.Name = "splitStrings";
            // 
            // splitStrings.Panel1
            // 
            this.splitStrings.Panel1.Controls.Add(this.lvStrings);
            // 
            // splitStrings.Panel2
            // 
            this.splitStrings.Panel2.Controls.Add(this.tableLayoutPanel1);
            this.splitStrings.Size = new System.Drawing.Size(809, 669);
            this.splitStrings.SplitterDistance = 450;
            this.splitStrings.TabIndex = 6;
            // 
            // lvStrings
            // 
            this.lvStrings.AutoArrange = false;
            this.lvStrings.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colStringsKey,
            this.colStringsStr});
            this.lvStrings.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvStrings.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvStrings.FullRowSelect = true;
            this.lvStrings.HideSelection = false;
            this.lvStrings.Location = new System.Drawing.Point(0, 0);
            this.lvStrings.MultiSelect = false;
            this.lvStrings.Name = "lvStrings";
            this.lvStrings.Size = new System.Drawing.Size(450, 669);
            this.lvStrings.TabIndex = 6;
            this.lvStrings.UseCompatibleStateImageBehavior = false;
            this.lvStrings.View = System.Windows.Forms.View.Details;
            this.lvStrings.SelectedIndexChanged += new System.EventHandler(this.lvStrings_SelectedIndexChanged);
            // 
            // colStringsKey
            // 
            this.colStringsKey.Text = "Index";
            this.colStringsKey.Width = 120;
            // 
            // colStringsStr
            // 
            this.colStringsStr.Text = "String";
            this.colStringsStr.Width = 384;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.tbStringKey, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.tbStringValue, 0, 1);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(355, 669);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // tbStringKey
            // 
            this.tbStringKey.BackColor = System.Drawing.SystemColors.Window;
            this.tbStringKey.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbStringKey.Location = new System.Drawing.Point(3, 3);
            this.tbStringKey.Name = "tbStringKey";
            this.tbStringKey.Size = new System.Drawing.Size(349, 21);
            this.tbStringKey.TabIndex = 3;
            // 
            // tbStringValue
            // 
            this.tbStringValue.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbStringValue.Location = new System.Drawing.Point(3, 29);
            this.tbStringValue.Multiline = true;
            this.tbStringValue.Name = "tbStringValue";
            this.tbStringValue.Size = new System.Drawing.Size(349, 637);
            this.tbStringValue.TabIndex = 4;
            // 
            // EditForm_String
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(809, 669);
            this.Controls.Add(this.splitStrings);
            this.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.HideOnClose = true;
            this.KeyPreview = true;
            this.Name = "EditForm_String";
            this.Text = "Strings";
            this.Load += new System.EventHandler(this.EditForm_String_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditForm_String_KeyDown);
            this.splitStrings.Panel1.ResumeLayout(false);
            this.splitStrings.Panel2.ResumeLayout(false);
            this.splitStrings.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitStrings;
        private System.Windows.Forms.ListView lvStrings;
        private System.Windows.Forms.ColumnHeader colStringsKey;
        private System.Windows.Forms.ColumnHeader colStringsStr;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TextBox tbStringKey;
        private System.Windows.Forms.TextBox tbStringValue;

    }
}