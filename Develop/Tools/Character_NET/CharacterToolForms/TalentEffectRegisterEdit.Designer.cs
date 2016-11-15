namespace CharacterToolForms
{
    partial class TalentEffectRegisterEdit
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
            this.tCancelButton = new System.Windows.Forms.Button();
            this.tOkButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.TalentModecomboBox = new System.Windows.Forms.ComboBox();
            this.TalentIDtextBox = new CharacterToolForms.NumbericTextBox();
            this.SuspendLayout();
            // 
            // TalentIDtextBox
            // 
            this.TalentIDtextBox.Location = new System.Drawing.Point(102, 20);
            this.TalentIDtextBox.MaxLength = 9;
            this.TalentIDtextBox.Name = "TalentIDtextBox";
            this.TalentIDtextBox.Size = new System.Drawing.Size(100, 21);
            this.TalentIDtextBox.TabIndex = 1;
            this.TalentIDtextBox.Text = "";
            // 
            // tCancelButton
            // 
            this.tCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.tCancelButton.Location = new System.Drawing.Point(119, 85);
            this.tCancelButton.Name = "tCancelButton";
            this.tCancelButton.Size = new System.Drawing.Size(75, 23);
            this.tCancelButton.TabIndex = 4;
            this.tCancelButton.Text = "Cancel";
            this.tCancelButton.UseVisualStyleBackColor = true;
            this.tCancelButton.Click += new System.EventHandler(this.CancleButton_Click);
            // 
            // tOkButton
            // 
            this.tOkButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.tOkButton.Location = new System.Drawing.Point(38, 85);
            this.tOkButton.Name = "tOkButton";
            this.tOkButton.Size = new System.Drawing.Size(75, 23);
            this.tOkButton.TabIndex = 3;
            this.tOkButton.Text = "OK";
            this.tOkButton.UseVisualStyleBackColor = true;
            this.tOkButton.Click += new System.EventHandler(this.OkButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 12);
            this.label1.TabIndex = 4;
            this.label1.Text = "Talent ID";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "TalentMode";
            // 
            // TalentModecomboBox
            // 
            this.TalentModecomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.TalentModecomboBox.FormattingEnabled = true;
            this.TalentModecomboBox.Location = new System.Drawing.Point(102, 47);
            this.TalentModecomboBox.Name = "TalentModecomboBox";
            this.TalentModecomboBox.Size = new System.Drawing.Size(100, 20);
            this.TalentModecomboBox.TabIndex = 2;
            // 
            // TalentEffectRegisterEdit
            // 
            this.AcceptButton = this.tOkButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.tCancelButton;
            this.ClientSize = new System.Drawing.Size(233, 125);
            this.Controls.Add(this.TalentModecomboBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.TalentIDtextBox);
            this.Controls.Add(this.tCancelButton);
            this.Controls.Add(this.tOkButton);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.KeyPreview = true;
            this.Name = "TalentEffectRegisterEdit";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "레퍼런스 탤런트 선택";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button tCancelButton;
        private System.Windows.Forms.Button tOkButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private NumbericTextBox TalentIDtextBox;

        public int m_nTalentID;
        public int m_nTalentMode;
        public System.Windows.Forms.ComboBox TalentModecomboBox;
    }
}