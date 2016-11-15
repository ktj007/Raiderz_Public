namespace Velixian
{
    partial class SplashForm
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
            this.tbSplash = new System.Windows.Forms.TextBox();
            this.pgBar = new System.Windows.Forms.ProgressBar();
            this.bgWorker = new System.ComponentModel.BackgroundWorker();
            this.SuspendLayout();
            // 
            // tbSplash
            // 
            this.tbSplash.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(209)))), ((int)(((byte)(209)))), ((int)(((byte)(209)))));
            this.tbSplash.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbSplash.Cursor = System.Windows.Forms.Cursors.Default;
            this.tbSplash.Font = new System.Drawing.Font("굴림", 9F);
            this.tbSplash.Location = new System.Drawing.Point(320, 112);
            this.tbSplash.Multiline = true;
            this.tbSplash.Name = "tbSplash";
            this.tbSplash.ReadOnly = true;
            this.tbSplash.Size = new System.Drawing.Size(203, 155);
            this.tbSplash.TabIndex = 0;
            this.tbSplash.TabStop = false;
            this.tbSplash.WordWrap = false;
            // 
            // pgBar
            // 
            this.pgBar.Location = new System.Drawing.Point(320, 285);
            this.pgBar.Name = "pgBar";
            this.pgBar.Size = new System.Drawing.Size(203, 23);
            this.pgBar.TabIndex = 1;
            // 
            // bgWorker
            // 
            this.bgWorker.WorkerReportsProgress = true;
            this.bgWorker.WorkerSupportsCancellation = true;
            this.bgWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgWorker_DoWork);
            this.bgWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgWorker_RunWorkerCompleted);
            this.bgWorker.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgWorker_ProgressChanged);
            // 
            // SplashForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(209)))), ((int)(((byte)(209)))), ((int)(((byte)(209)))));
            this.BackgroundImage = global::Velixian.Properties.Resources.splash;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ClientSize = new System.Drawing.Size(546, 320);
            this.ControlBox = false;
            this.Controls.Add(this.pgBar);
            this.Controls.Add(this.tbSplash);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SplashForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Velixian";
            this.Load += new System.EventHandler(this.SplashForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbSplash;
        private System.Windows.Forms.ProgressBar pgBar;
        private System.ComponentModel.BackgroundWorker bgWorker;


    }
}