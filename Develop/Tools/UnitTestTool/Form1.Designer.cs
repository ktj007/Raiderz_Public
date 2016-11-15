namespace UnitTestTool
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.button_Reset = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage_Enable = new System.Windows.Forms.TabPage();
            this.textBox_TEST4 = new System.Windows.Forms.TextBox();
            this.label_TEST4 = new System.Windows.Forms.Label();
            this.textBox_TEST_SUITE4 = new System.Windows.Forms.TextBox();
            this.label_TEST_SUITE4 = new System.Windows.Forms.Label();
            this.textBox_TEST3 = new System.Windows.Forms.TextBox();
            this.label_TEST3 = new System.Windows.Forms.Label();
            this.textBox_TEST_SUITE3 = new System.Windows.Forms.TextBox();
            this.label_TEST_SUITE3 = new System.Windows.Forms.Label();
            this.textBox_TEST2 = new System.Windows.Forms.TextBox();
            this.label_TEST2 = new System.Windows.Forms.Label();
            this.textBox_TEST_SUITE2 = new System.Windows.Forms.TextBox();
            this.label_TEST_SUITE2 = new System.Windows.Forms.Label();
            this.textBox_TEST1 = new System.Windows.Forms.TextBox();
            this.label_TEST1 = new System.Windows.Forms.Label();
            this.textBox_TEST_SUITE1 = new System.Windows.Forms.TextBox();
            this.label_TEST_SUITE1 = new System.Windows.Forms.Label();
            this.tabPage_Disable = new System.Windows.Forms.TabPage();
            this.textBox_DISABLE_TEST_SUITE4 = new System.Windows.Forms.TextBox();
            this.textBox_DISABLE_TEST_SUITE3 = new System.Windows.Forms.TextBox();
            this.textBox_DISABLE_TEST_SUITE2 = new System.Windows.Forms.TextBox();
            this.textBox_DISABLE_TEST_SUITE1 = new System.Windows.Forms.TextBox();
            this.label_DISABLE_TEST_SUITE4 = new System.Windows.Forms.Label();
            this.label_DISABLE_TEST_SUITE3 = new System.Windows.Forms.Label();
            this.label_DISABLE_TEST_SUITE2 = new System.Windows.Forms.Label();
            this.label_DISABLE_TEST_SUITE1 = new System.Windows.Forms.Label();
            this.checkBox_Execute = new System.Windows.Forms.CheckBox();
            this.button_Close = new System.Windows.Forms.Button();
            this.tabPage_Option = new System.Windows.Forms.TabPage();
            this.checkBox_BreakOnFailure = new System.Windows.Forms.CheckBox();
            this.label_Repeat = new System.Windows.Forms.Label();
            this.textBox_Repeat = new System.Windows.Forms.TextBox();
            this.tabControl1.SuspendLayout();
            this.tabPage_Enable.SuspendLayout();
            this.tabPage_Disable.SuspendLayout();
            this.tabPage_Option.SuspendLayout();
            this.SuspendLayout();
            // 
            // button_Reset
            // 
            this.button_Reset.Location = new System.Drawing.Point(12, 290);
            this.button_Reset.Name = "button_Reset";
            this.button_Reset.Size = new System.Drawing.Size(62, 28);
            this.button_Reset.TabIndex = 1;
            this.button_Reset.Text = "Reset";
            this.button_Reset.UseVisualStyleBackColor = true;
            this.button_Reset.Click += new System.EventHandler(this.button_Reset_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage_Enable);
            this.tabControl1.Controls.Add(this.tabPage_Disable);
            this.tabControl1.Controls.Add(this.tabPage_Option);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(355, 272);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage_Enable
            // 
            this.tabPage_Enable.Controls.Add(this.textBox_TEST4);
            this.tabPage_Enable.Controls.Add(this.label_TEST4);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST_SUITE4);
            this.tabPage_Enable.Controls.Add(this.label_TEST_SUITE4);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST3);
            this.tabPage_Enable.Controls.Add(this.label_TEST3);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST_SUITE3);
            this.tabPage_Enable.Controls.Add(this.label_TEST_SUITE3);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST2);
            this.tabPage_Enable.Controls.Add(this.label_TEST2);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST_SUITE2);
            this.tabPage_Enable.Controls.Add(this.label_TEST_SUITE2);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST1);
            this.tabPage_Enable.Controls.Add(this.label_TEST1);
            this.tabPage_Enable.Controls.Add(this.textBox_TEST_SUITE1);
            this.tabPage_Enable.Controls.Add(this.label_TEST_SUITE1);
            this.tabPage_Enable.Location = new System.Drawing.Point(4, 21);
            this.tabPage_Enable.Name = "tabPage_Enable";
            this.tabPage_Enable.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Enable.Size = new System.Drawing.Size(347, 247);
            this.tabPage_Enable.TabIndex = 0;
            this.tabPage_Enable.Text = "Enable";
            this.tabPage_Enable.UseVisualStyleBackColor = true;
            // 
            // textBox_TEST4
            // 
            this.textBox_TEST4.Location = new System.Drawing.Point(109, 99);
            this.textBox_TEST4.Name = "textBox_TEST4";
            this.textBox_TEST4.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST4.TabIndex = 4;
            this.textBox_TEST4.TextChanged += new System.EventHandler(this.textBox_TEST4_TextChanged);
            // 
            // label_TEST4
            // 
            this.label_TEST4.AutoSize = true;
            this.label_TEST4.Location = new System.Drawing.Point(22, 102);
            this.label_TEST4.Name = "label_TEST4";
            this.label_TEST4.Size = new System.Drawing.Size(43, 12);
            this.label_TEST4.TabIndex = 14;
            this.label_TEST4.Text = "TEST4";
            // 
            // textBox_TEST_SUITE4
            // 
            this.textBox_TEST_SUITE4.Location = new System.Drawing.Point(109, 211);
            this.textBox_TEST_SUITE4.Name = "textBox_TEST_SUITE4";
            this.textBox_TEST_SUITE4.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST_SUITE4.TabIndex = 8;
            this.textBox_TEST_SUITE4.TextChanged += new System.EventHandler(this.textBox_TEST_SUITE4_TextChanged);
            // 
            // label_TEST_SUITE4
            // 
            this.label_TEST_SUITE4.AutoSize = true;
            this.label_TEST_SUITE4.Location = new System.Drawing.Point(19, 214);
            this.label_TEST_SUITE4.Name = "label_TEST_SUITE4";
            this.label_TEST_SUITE4.Size = new System.Drawing.Size(84, 12);
            this.label_TEST_SUITE4.TabIndex = 12;
            this.label_TEST_SUITE4.Text = "TEST_SUITE4";
            // 
            // textBox_TEST3
            // 
            this.textBox_TEST3.Location = new System.Drawing.Point(109, 72);
            this.textBox_TEST3.Name = "textBox_TEST3";
            this.textBox_TEST3.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST3.TabIndex = 3;
            this.textBox_TEST3.TextChanged += new System.EventHandler(this.textBox_TEST3_TextChanged);
            // 
            // label_TEST3
            // 
            this.label_TEST3.AutoSize = true;
            this.label_TEST3.Location = new System.Drawing.Point(22, 75);
            this.label_TEST3.Name = "label_TEST3";
            this.label_TEST3.Size = new System.Drawing.Size(43, 12);
            this.label_TEST3.TabIndex = 10;
            this.label_TEST3.Text = "TEST3";
            // 
            // textBox_TEST_SUITE3
            // 
            this.textBox_TEST_SUITE3.Location = new System.Drawing.Point(109, 184);
            this.textBox_TEST_SUITE3.Name = "textBox_TEST_SUITE3";
            this.textBox_TEST_SUITE3.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST_SUITE3.TabIndex = 7;
            this.textBox_TEST_SUITE3.TextChanged += new System.EventHandler(this.textBox_TEST_SUITE3_TextChanged);
            // 
            // label_TEST_SUITE3
            // 
            this.label_TEST_SUITE3.AutoSize = true;
            this.label_TEST_SUITE3.Location = new System.Drawing.Point(19, 187);
            this.label_TEST_SUITE3.Name = "label_TEST_SUITE3";
            this.label_TEST_SUITE3.Size = new System.Drawing.Size(84, 12);
            this.label_TEST_SUITE3.TabIndex = 8;
            this.label_TEST_SUITE3.Text = "TEST_SUITE3";
            // 
            // textBox_TEST2
            // 
            this.textBox_TEST2.Location = new System.Drawing.Point(109, 45);
            this.textBox_TEST2.Name = "textBox_TEST2";
            this.textBox_TEST2.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST2.TabIndex = 2;
            this.textBox_TEST2.TextChanged += new System.EventHandler(this.textBox_TEST2_TextChanged);
            // 
            // label_TEST2
            // 
            this.label_TEST2.AutoSize = true;
            this.label_TEST2.Location = new System.Drawing.Point(22, 48);
            this.label_TEST2.Name = "label_TEST2";
            this.label_TEST2.Size = new System.Drawing.Size(43, 12);
            this.label_TEST2.TabIndex = 6;
            this.label_TEST2.Text = "TEST2";
            // 
            // textBox_TEST_SUITE2
            // 
            this.textBox_TEST_SUITE2.Location = new System.Drawing.Point(109, 157);
            this.textBox_TEST_SUITE2.Name = "textBox_TEST_SUITE2";
            this.textBox_TEST_SUITE2.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST_SUITE2.TabIndex = 6;
            this.textBox_TEST_SUITE2.TextChanged += new System.EventHandler(this.textBox_TEST_SUITE2_TextChanged);
            // 
            // label_TEST_SUITE2
            // 
            this.label_TEST_SUITE2.AutoSize = true;
            this.label_TEST_SUITE2.Location = new System.Drawing.Point(19, 160);
            this.label_TEST_SUITE2.Name = "label_TEST_SUITE2";
            this.label_TEST_SUITE2.Size = new System.Drawing.Size(84, 12);
            this.label_TEST_SUITE2.TabIndex = 4;
            this.label_TEST_SUITE2.Text = "TEST_SUITE2";
            // 
            // textBox_TEST1
            // 
            this.textBox_TEST1.Location = new System.Drawing.Point(109, 18);
            this.textBox_TEST1.Name = "textBox_TEST1";
            this.textBox_TEST1.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST1.TabIndex = 1;
            this.textBox_TEST1.TextChanged += new System.EventHandler(this.textBox_TEST1_TextChanged);
            // 
            // label_TEST1
            // 
            this.label_TEST1.AutoSize = true;
            this.label_TEST1.Location = new System.Drawing.Point(22, 21);
            this.label_TEST1.Name = "label_TEST1";
            this.label_TEST1.Size = new System.Drawing.Size(43, 12);
            this.label_TEST1.TabIndex = 2;
            this.label_TEST1.Text = "TEST1";
            // 
            // textBox_TEST_SUITE1
            // 
            this.textBox_TEST_SUITE1.Location = new System.Drawing.Point(109, 130);
            this.textBox_TEST_SUITE1.Name = "textBox_TEST_SUITE1";
            this.textBox_TEST_SUITE1.Size = new System.Drawing.Size(219, 21);
            this.textBox_TEST_SUITE1.TabIndex = 5;
            this.textBox_TEST_SUITE1.TextChanged += new System.EventHandler(this.textBox_TEST_SUITE1_TextChanged);
            // 
            // label_TEST_SUITE1
            // 
            this.label_TEST_SUITE1.AutoSize = true;
            this.label_TEST_SUITE1.Location = new System.Drawing.Point(19, 133);
            this.label_TEST_SUITE1.Name = "label_TEST_SUITE1";
            this.label_TEST_SUITE1.Size = new System.Drawing.Size(84, 12);
            this.label_TEST_SUITE1.TabIndex = 0;
            this.label_TEST_SUITE1.Text = "TEST_SUITE1";
            // 
            // tabPage_Disable
            // 
            this.tabPage_Disable.Controls.Add(this.textBox_DISABLE_TEST_SUITE4);
            this.tabPage_Disable.Controls.Add(this.textBox_DISABLE_TEST_SUITE3);
            this.tabPage_Disable.Controls.Add(this.textBox_DISABLE_TEST_SUITE2);
            this.tabPage_Disable.Controls.Add(this.textBox_DISABLE_TEST_SUITE1);
            this.tabPage_Disable.Controls.Add(this.label_DISABLE_TEST_SUITE4);
            this.tabPage_Disable.Controls.Add(this.label_DISABLE_TEST_SUITE3);
            this.tabPage_Disable.Controls.Add(this.label_DISABLE_TEST_SUITE2);
            this.tabPage_Disable.Controls.Add(this.label_DISABLE_TEST_SUITE1);
            this.tabPage_Disable.Location = new System.Drawing.Point(4, 21);
            this.tabPage_Disable.Name = "tabPage_Disable";
            this.tabPage_Disable.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Disable.Size = new System.Drawing.Size(347, 247);
            this.tabPage_Disable.TabIndex = 1;
            this.tabPage_Disable.Text = "Disable";
            this.tabPage_Disable.UseVisualStyleBackColor = true;
            // 
            // textBox_DISABLE_TEST_SUITE4
            // 
            this.textBox_DISABLE_TEST_SUITE4.Location = new System.Drawing.Point(110, 103);
            this.textBox_DISABLE_TEST_SUITE4.Name = "textBox_DISABLE_TEST_SUITE4";
            this.textBox_DISABLE_TEST_SUITE4.Size = new System.Drawing.Size(219, 21);
            this.textBox_DISABLE_TEST_SUITE4.TabIndex = 10;
            this.textBox_DISABLE_TEST_SUITE4.TextChanged += new System.EventHandler(this.textBox_DISABLE_TEST_SUITE4_TextChanged);
            // 
            // textBox_DISABLE_TEST_SUITE3
            // 
            this.textBox_DISABLE_TEST_SUITE3.Location = new System.Drawing.Point(110, 75);
            this.textBox_DISABLE_TEST_SUITE3.Name = "textBox_DISABLE_TEST_SUITE3";
            this.textBox_DISABLE_TEST_SUITE3.Size = new System.Drawing.Size(219, 21);
            this.textBox_DISABLE_TEST_SUITE3.TabIndex = 9;
            this.textBox_DISABLE_TEST_SUITE3.TextChanged += new System.EventHandler(this.textBox_DISABLE_TEST_SUITE3_TextChanged);
            // 
            // textBox_DISABLE_TEST_SUITE2
            // 
            this.textBox_DISABLE_TEST_SUITE2.Location = new System.Drawing.Point(110, 47);
            this.textBox_DISABLE_TEST_SUITE2.Name = "textBox_DISABLE_TEST_SUITE2";
            this.textBox_DISABLE_TEST_SUITE2.Size = new System.Drawing.Size(219, 21);
            this.textBox_DISABLE_TEST_SUITE2.TabIndex = 8;
            this.textBox_DISABLE_TEST_SUITE2.TextChanged += new System.EventHandler(this.textBox_DISABLE_TEST_SUITE2_TextChanged);
            // 
            // textBox_DISABLE_TEST_SUITE1
            // 
            this.textBox_DISABLE_TEST_SUITE1.Location = new System.Drawing.Point(110, 18);
            this.textBox_DISABLE_TEST_SUITE1.Name = "textBox_DISABLE_TEST_SUITE1";
            this.textBox_DISABLE_TEST_SUITE1.Size = new System.Drawing.Size(219, 21);
            this.textBox_DISABLE_TEST_SUITE1.TabIndex = 7;
            this.textBox_DISABLE_TEST_SUITE1.TextChanged += new System.EventHandler(this.textBox_DISABLE_TEST_SUITE1_TextChanged);
            // 
            // label_DISABLE_TEST_SUITE4
            // 
            this.label_DISABLE_TEST_SUITE4.AutoSize = true;
            this.label_DISABLE_TEST_SUITE4.Location = new System.Drawing.Point(20, 106);
            this.label_DISABLE_TEST_SUITE4.Name = "label_DISABLE_TEST_SUITE4";
            this.label_DISABLE_TEST_SUITE4.Size = new System.Drawing.Size(84, 12);
            this.label_DISABLE_TEST_SUITE4.TabIndex = 6;
            this.label_DISABLE_TEST_SUITE4.Text = "TEST_SUITE4";
            // 
            // label_DISABLE_TEST_SUITE3
            // 
            this.label_DISABLE_TEST_SUITE3.AutoSize = true;
            this.label_DISABLE_TEST_SUITE3.Location = new System.Drawing.Point(20, 78);
            this.label_DISABLE_TEST_SUITE3.Name = "label_DISABLE_TEST_SUITE3";
            this.label_DISABLE_TEST_SUITE3.Size = new System.Drawing.Size(84, 12);
            this.label_DISABLE_TEST_SUITE3.TabIndex = 5;
            this.label_DISABLE_TEST_SUITE3.Text = "TEST_SUITE3";
            // 
            // label_DISABLE_TEST_SUITE2
            // 
            this.label_DISABLE_TEST_SUITE2.AutoSize = true;
            this.label_DISABLE_TEST_SUITE2.Location = new System.Drawing.Point(20, 50);
            this.label_DISABLE_TEST_SUITE2.Name = "label_DISABLE_TEST_SUITE2";
            this.label_DISABLE_TEST_SUITE2.Size = new System.Drawing.Size(84, 12);
            this.label_DISABLE_TEST_SUITE2.TabIndex = 4;
            this.label_DISABLE_TEST_SUITE2.Text = "TEST_SUITE2";
            // 
            // label_DISABLE_TEST_SUITE1
            // 
            this.label_DISABLE_TEST_SUITE1.AutoSize = true;
            this.label_DISABLE_TEST_SUITE1.Location = new System.Drawing.Point(20, 21);
            this.label_DISABLE_TEST_SUITE1.Name = "label_DISABLE_TEST_SUITE1";
            this.label_DISABLE_TEST_SUITE1.Size = new System.Drawing.Size(84, 12);
            this.label_DISABLE_TEST_SUITE1.TabIndex = 3;
            this.label_DISABLE_TEST_SUITE1.Text = "TEST_SUITE1";
            // 
            // checkBox_Execute
            // 
            this.checkBox_Execute.AutoSize = true;
            this.checkBox_Execute.Location = new System.Drawing.Point(297, 11);
            this.checkBox_Execute.Name = "checkBox_Execute";
            this.checkBox_Execute.Size = new System.Drawing.Size(70, 16);
            this.checkBox_Execute.TabIndex = 5;
            this.checkBox_Execute.Text = "Execute";
            this.checkBox_Execute.UseVisualStyleBackColor = true;
            this.checkBox_Execute.CheckStateChanged += new System.EventHandler(this.checkBox_Execute_CheckStateChanged);
            // 
            // button_Close
            // 
            this.button_Close.Location = new System.Drawing.Point(80, 290);
            this.button_Close.Name = "button_Close";
            this.button_Close.Size = new System.Drawing.Size(61, 28);
            this.button_Close.TabIndex = 6;
            this.button_Close.Text = "Close";
            this.button_Close.UseVisualStyleBackColor = true;
            this.button_Close.Click += new System.EventHandler(this.button_Close_Click);
            // 
            // tabPage_Option
            // 
            this.tabPage_Option.Controls.Add(this.textBox_Repeat);
            this.tabPage_Option.Controls.Add(this.label_Repeat);
            this.tabPage_Option.Controls.Add(this.checkBox_BreakOnFailure);
            this.tabPage_Option.Location = new System.Drawing.Point(4, 21);
            this.tabPage_Option.Name = "tabPage_Option";
            this.tabPage_Option.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Option.Size = new System.Drawing.Size(347, 247);
            this.tabPage_Option.TabIndex = 2;
            this.tabPage_Option.Text = "Option";
            this.tabPage_Option.UseVisualStyleBackColor = true;
            // 
            // checkBox_BreakOnFailure
            // 
            this.checkBox_BreakOnFailure.AutoSize = true;
            this.checkBox_BreakOnFailure.Location = new System.Drawing.Point(197, 18);
            this.checkBox_BreakOnFailure.Name = "checkBox_BreakOnFailure";
            this.checkBox_BreakOnFailure.Size = new System.Drawing.Size(110, 16);
            this.checkBox_BreakOnFailure.TabIndex = 6;
            this.checkBox_BreakOnFailure.Text = "BreakOnFailure";
            this.checkBox_BreakOnFailure.UseVisualStyleBackColor = true;
            this.checkBox_BreakOnFailure.CheckedChanged += new System.EventHandler(this.checkBox_BreakOnFailure_CheckedChanged);
            // 
            // label_Repeat
            // 
            this.label_Repeat.AutoSize = true;
            this.label_Repeat.Location = new System.Drawing.Point(43, 19);
            this.label_Repeat.Name = "label_Repeat";
            this.label_Repeat.Size = new System.Drawing.Size(44, 12);
            this.label_Repeat.TabIndex = 7;
            this.label_Repeat.Text = "Repeat";
            // 
            // textBox_Repeat
            // 
            this.textBox_Repeat.Location = new System.Drawing.Point(93, 16);
            this.textBox_Repeat.Name = "textBox_Repeat";
            this.textBox_Repeat.Size = new System.Drawing.Size(64, 21);
            this.textBox_Repeat.TabIndex = 8;
            this.textBox_Repeat.TextChanged += new System.EventHandler(this.textBox_Repeat_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(377, 328);
            this.Controls.Add(this.button_Close);
            this.Controls.Add(this.checkBox_Execute);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.button_Reset);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.MaximumSize = new System.Drawing.Size(387, 364);
            this.MinimumSize = new System.Drawing.Size(387, 364);
            this.Name = "Form1";
            this.Text = "UnitTestTool";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyUp);
            this.tabControl1.ResumeLayout(false);
            this.tabPage_Enable.ResumeLayout(false);
            this.tabPage_Enable.PerformLayout();
            this.tabPage_Disable.ResumeLayout(false);
            this.tabPage_Disable.PerformLayout();
            this.tabPage_Option.ResumeLayout(false);
            this.tabPage_Option.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_Reset;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage_Enable;
        private System.Windows.Forms.TabPage tabPage_Disable;
        private System.Windows.Forms.TextBox textBox_TEST_SUITE1;
        private System.Windows.Forms.Label label_TEST_SUITE1;
        private System.Windows.Forms.TextBox textBox_TEST1;
        private System.Windows.Forms.Label label_TEST1;
        private System.Windows.Forms.TextBox textBox_TEST4;
        private System.Windows.Forms.Label label_TEST4;
        private System.Windows.Forms.TextBox textBox_TEST_SUITE4;
        private System.Windows.Forms.Label label_TEST_SUITE4;
        private System.Windows.Forms.TextBox textBox_TEST3;
        private System.Windows.Forms.Label label_TEST3;
        private System.Windows.Forms.TextBox textBox_TEST_SUITE3;
        private System.Windows.Forms.Label label_TEST_SUITE3;
        private System.Windows.Forms.TextBox textBox_TEST2;
        private System.Windows.Forms.Label label_TEST2;
        private System.Windows.Forms.TextBox textBox_TEST_SUITE2;
        private System.Windows.Forms.Label label_TEST_SUITE2;
        private System.Windows.Forms.CheckBox checkBox_Execute;
        private System.Windows.Forms.TextBox textBox_DISABLE_TEST_SUITE1;
        private System.Windows.Forms.Label label_DISABLE_TEST_SUITE4;
        private System.Windows.Forms.Label label_DISABLE_TEST_SUITE3;
        private System.Windows.Forms.Label label_DISABLE_TEST_SUITE2;
        private System.Windows.Forms.Label label_DISABLE_TEST_SUITE1;
        private System.Windows.Forms.TextBox textBox_DISABLE_TEST_SUITE4;
        private System.Windows.Forms.TextBox textBox_DISABLE_TEST_SUITE3;
        private System.Windows.Forms.TextBox textBox_DISABLE_TEST_SUITE2;
        private System.Windows.Forms.Button button_Close;
        private System.Windows.Forms.TabPage tabPage_Option;
        private System.Windows.Forms.TextBox textBox_Repeat;
        private System.Windows.Forms.Label label_Repeat;
        private System.Windows.Forms.CheckBox checkBox_BreakOnFailure;
    }
}

