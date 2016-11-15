namespace ServerMonitor
{
    partial class FormServerDetailInfo
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
            this.listboxDetailInfo = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // listboxDetailInfo
            // 
            this.listboxDetailInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listboxDetailInfo.FormattingEnabled = true;
            this.listboxDetailInfo.HorizontalScrollbar = true;
            this.listboxDetailInfo.ItemHeight = 12;
            this.listboxDetailInfo.Location = new System.Drawing.Point(0, 0);
            this.listboxDetailInfo.Name = "listboxDetailInfo";
            this.listboxDetailInfo.Size = new System.Drawing.Size(341, 136);
            this.listboxDetailInfo.TabIndex = 0;
            // 
            // FormServerDetailInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(341, 136);
            this.Controls.Add(this.listboxDetailInfo);
            this.Name = "FormServerDetailInfo";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Server 상세 정보";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox listboxDetailInfo;

    }
}