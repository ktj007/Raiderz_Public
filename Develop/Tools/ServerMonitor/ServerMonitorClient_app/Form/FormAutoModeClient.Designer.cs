namespace ServerMonitorClient
{
    partial class FormAutoModeClient
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormAutoModeClient));
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.timerMainUpdate = new System.Windows.Forms.Timer(this.components);
            this.timerAutoModeTimeout = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Location = new System.Drawing.Point(0, 0);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(492, 266);
            this.textBox1.TabIndex = 0;
            // 
            // timerMainUpdate
            // 
            this.timerMainUpdate.Enabled = true;
            this.timerMainUpdate.Tick += new System.EventHandler(this.timerMainUpdate_Tick);
            // 
            // timerAutoModeTimeout
            // 
            this.timerAutoModeTimeout.Enabled = true;
            this.timerAutoModeTimeout.Interval = 300000;
            this.timerAutoModeTimeout.Tick += new System.EventHandler(this.timerAutoModeTimeout_Tick);
            // 
            // FormAutoMode
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(492, 266);
            this.Controls.Add(this.textBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormAutoMode";
            this.Text = "AutoModeForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AutoModeForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Timer timerMainUpdate;
        private System.Windows.Forms.Timer timerAutoModeTimeout;
    }
}