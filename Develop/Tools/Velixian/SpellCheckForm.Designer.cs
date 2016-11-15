namespace Velixian
{
    partial class SpellCheckForm
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
            this.wbSpellCheck = new System.Windows.Forms.WebBrowser();
            this.SuspendLayout();
            // 
            // wbSpellCheck
            // 
            this.wbSpellCheck.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbSpellCheck.Location = new System.Drawing.Point(0, 0);
            this.wbSpellCheck.MinimumSize = new System.Drawing.Size(17, 22);
            this.wbSpellCheck.Name = "wbSpellCheck";
            this.wbSpellCheck.ScrollBarsEnabled = false;
            this.wbSpellCheck.Size = new System.Drawing.Size(360, 711);
            this.wbSpellCheck.TabIndex = 0;
            this.wbSpellCheck.Url = new System.Uri("http://urimal.cs.pusan.ac.kr/system/speller", System.UriKind.Absolute);
            // 
            // SpellCheckForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(360, 711);
            this.Controls.Add(this.wbSpellCheck);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "SpellCheckForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Spell Check";
            this.Load += new System.EventHandler(this.SpellCheckForm_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.WebBrowser wbSpellCheck;
    }
}