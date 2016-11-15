namespace SoulHunt
{
    partial class CompareFileForm
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.lblFileName = new System.Windows.Forms.Label();
            this.btnCopyServer = new System.Windows.Forms.Button();
            this.btnCopyClient = new System.Windows.Forms.Button();
            this.btnExit = new System.Windows.Forms.Button();
            this.btnRestart = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.SystemColors.Control;
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Location = new System.Drawing.Point(10, 41);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(463, 104);
            this.textBox1.TabIndex = 0;
            this.textBox1.TabStop = false;
            this.textBox1.Text = "Client/Server File mismatch";
            // 
            // lblFileName
            // 
            this.lblFileName.AutoSize = true;
            this.lblFileName.Font = new System.Drawing.Font("Gulim", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lblFileName.Location = new System.Drawing.Point(10, 10);
            this.lblFileName.Name = "lblFileName";
            this.lblFileName.Size = new System.Drawing.Size(66, 14);
            this.lblFileName.TabIndex = 0;
            this.lblFileName.Text = "npc.xml";
            // 
            // btnCopyServer
            // 
            this.btnCopyServer.DialogResult = System.Windows.Forms.DialogResult.Yes;
            this.btnCopyServer.Location = new System.Drawing.Point(10, 152);
            this.btnCopyServer.Name = "btnCopyServer";
            this.btnCopyServer.Size = new System.Drawing.Size(117, 35);
            this.btnCopyServer.TabIndex = 0;
            this.btnCopyServer.Text = "Copy from Server (&S)";
            this.btnCopyServer.UseVisualStyleBackColor = true;
            // 
            // btnCopyClient
            // 
            this.btnCopyClient.DialogResult = System.Windows.Forms.DialogResult.No;
            this.btnCopyClient.Location = new System.Drawing.Point(133, 152);
            this.btnCopyClient.Name = "btnCopyClient";
            this.btnCopyClient.Size = new System.Drawing.Size(147, 35);
            this.btnCopyClient.TabIndex = 1;
            this.btnCopyClient.Text = "Copy from Client (&C)";
            this.btnCopyClient.UseVisualStyleBackColor = true;
            // 
            // btnExit
            // 
            this.btnExit.DialogResult = System.Windows.Forms.DialogResult.Abort;
            this.btnExit.Location = new System.Drawing.Point(398, 152);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(85, 35);
            this.btnExit.TabIndex = 2;
            this.btnExit.Text = "Exit (&X)";
            this.btnExit.UseVisualStyleBackColor = true;
            // 
            // btnRestart
            // 
            this.btnRestart.DialogResult = System.Windows.Forms.DialogResult.Retry;
            this.btnRestart.Location = new System.Drawing.Point(295, 152);
            this.btnRestart.Name = "btnRestart";
            this.btnRestart.Size = new System.Drawing.Size(98, 35);
            this.btnRestart.TabIndex = 3;
            this.btnRestart.Text = "Retry (&R)";
            this.btnRestart.UseVisualStyleBackColor = true;
            // 
            // CompareFileForm
            // 
            this.AcceptButton = this.btnCopyServer;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnExit;
            this.ClientSize = new System.Drawing.Size(493, 202);
            this.ControlBox = false;
            this.Controls.Add(this.btnRestart);
            this.Controls.Add(this.btnExit);
            this.Controls.Add(this.btnCopyClient);
            this.Controls.Add(this.btnCopyServer);
            this.Controls.Add(this.lblFileName);
            this.Controls.Add(this.textBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CompareFileForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "File Compare";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnExit;
        public System.Windows.Forms.Label lblFileName;
        public System.Windows.Forms.Button btnCopyServer;
        public System.Windows.Forms.Button btnCopyClient;
        private System.Windows.Forms.Button btnRestart;

    }
}