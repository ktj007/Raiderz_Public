namespace SoulHunt
{
    partial class SHCE_TextEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SHCE_TextEditor));
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.lvList = new System.Windows.Forms.ListView();
            this.colKey = new System.Windows.Forms.ColumnHeader();
            this.colValue = new System.Windows.Forms.ColumnHeader();
            this.cbSearchText = new System.Windows.Forms.ComboBox();
            this.cbIDOnly = new System.Windows.Forms.CheckBox();
            this.wbViewText = new System.Windows.Forms.WebBrowser();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.tsbtnReplaceCharacterTag = new System.Windows.Forms.ToolStripButton();
            this.tsbtnReplaceItemTag = new System.Windows.Forms.ToolStripButton();
            this.tsbtnReplacePlaceTag = new System.Windows.Forms.ToolStripButton();
            this.tsbtnAniTag = new System.Windows.Forms.ToolStripButton();
            this.lbTextLen = new System.Windows.Forms.Label();
            this.tbEditor = new System.Windows.Forms.TextBox();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Font = new System.Drawing.Font("Gulim", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnOK.Location = new System.Drawing.Point(499, 522);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(357, 76);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "Check (&O)";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Font = new System.Drawing.Font("Gulim", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnCancel.Location = new System.Drawing.Point(861, 522);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(163, 76);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel (&C)";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.Location = new System.Drawing.Point(10, 584);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(410, 15);
            this.label1.TabIndex = 3;
            this.label1.Text = "[br] line breaks are automatically convereted, aswell as becoming invisible.";
            // 
            // lvList
            // 
            this.lvList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.lvList.AutoArrange = false;
            this.lvList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colKey,
            this.colValue});
            this.lvList.Font = new System.Drawing.Font("Gulim", 9F);
            this.lvList.FullRowSelect = true;
            this.lvList.HideSelection = false;
            this.lvList.Location = new System.Drawing.Point(721, 62);
            this.lvList.MultiSelect = false;
            this.lvList.Name = "lvList";
            this.lvList.Size = new System.Drawing.Size(303, 443);
            this.lvList.TabIndex = 6;
            this.lvList.UseCompatibleStateImageBehavior = false;
            this.lvList.View = System.Windows.Forms.View.Details;
            this.lvList.DoubleClick += new System.EventHandler(this.lvList_DoubleClick);
            // 
            // colKey
            // 
            this.colKey.Text = "Username";
            this.colKey.Width = 100;
            // 
            // colValue
            // 
            this.colValue.Text = "Reference";
            this.colValue.Width = 300;
            // 
            // cbSearchText
            // 
            this.cbSearchText.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.cbSearchText.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.cbSearchText.FormattingEnabled = true;
            this.cbSearchText.Location = new System.Drawing.Point(721, 34);
            this.cbSearchText.Name = "cbSearchText";
            this.cbSearchText.Size = new System.Drawing.Size(212, 21);
            this.cbSearchText.TabIndex = 7;
            this.cbSearchText.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbSearchText_KeyDown);
            // 
            // cbIDOnly
            // 
            this.cbIDOnly.AutoSize = true;
            this.cbIDOnly.Checked = true;
            this.cbIDOnly.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbIDOnly.Location = new System.Drawing.Point(938, 34);
            this.cbIDOnly.Name = "cbIDOnly";
            this.cbIDOnly.Size = new System.Drawing.Size(98, 17);
            this.cbIDOnly.TabIndex = 8;
            this.cbIDOnly.Text = "ID Search Only";
            this.cbIDOnly.UseVisualStyleBackColor = true;
            // 
            // wbViewText
            // 
            this.wbViewText.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.wbViewText.Location = new System.Drawing.Point(310, 34);
            this.wbViewText.MinimumSize = new System.Drawing.Size(17, 22);
            this.wbViewText.Name = "wbViewText";
            this.wbViewText.Size = new System.Drawing.Size(405, 471);
            this.wbViewText.TabIndex = 9;
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbtnReplaceCharacterTag,
            this.tsbtnReplaceItemTag,
            this.tsbtnReplacePlaceTag,
            this.tsbtnAniTag});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1043, 28);
            this.toolStrip1.TabIndex = 10;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // tsbtnReplaceCharacterTag
            // 
            this.tsbtnReplaceCharacterTag.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.tsbtnReplaceCharacterTag.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tsbtnReplaceCharacterTag.Image = ((System.Drawing.Image)(resources.GetObject("tsbtnReplaceCharacterTag.Image")));
            this.tsbtnReplaceCharacterTag.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnReplaceCharacterTag.Name = "tsbtnReplaceCharacterTag";
            this.tsbtnReplaceCharacterTag.Size = new System.Drawing.Size(92, 25);
            this.tsbtnReplaceCharacterTag.Text = "{character}";
            this.tsbtnReplaceCharacterTag.ToolTipText = "{character} 변환 - F1";
            this.tsbtnReplaceCharacterTag.Click += new System.EventHandler(this.tsbtnReplaceCharacterTag_Click);
            // 
            // tsbtnReplaceItemTag
            // 
            this.tsbtnReplaceItemTag.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.tsbtnReplaceItemTag.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tsbtnReplaceItemTag.Image = ((System.Drawing.Image)(resources.GetObject("tsbtnReplaceItemTag.Image")));
            this.tsbtnReplaceItemTag.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnReplaceItemTag.Name = "tsbtnReplaceItemTag";
            this.tsbtnReplaceItemTag.Size = new System.Drawing.Size(57, 25);
            this.tsbtnReplaceItemTag.Text = "{item}";
            this.tsbtnReplaceItemTag.ToolTipText = "{item} 변환 - F2";
            this.tsbtnReplaceItemTag.Click += new System.EventHandler(this.tsbtnReplaceItemTag_Click);
            // 
            // tsbtnReplacePlaceTag
            // 
            this.tsbtnReplacePlaceTag.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.tsbtnReplacePlaceTag.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tsbtnReplacePlaceTag.Image = ((System.Drawing.Image)(resources.GetObject("tsbtnReplacePlaceTag.Image")));
            this.tsbtnReplacePlaceTag.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnReplacePlaceTag.Name = "tsbtnReplacePlaceTag";
            this.tsbtnReplacePlaceTag.Size = new System.Drawing.Size(63, 25);
            this.tsbtnReplacePlaceTag.Text = "{place}";
            this.tsbtnReplacePlaceTag.ToolTipText = "{place} 변환 - F3";
            this.tsbtnReplacePlaceTag.Click += new System.EventHandler(this.tsbtnReplacePlaceTag_Click);
            // 
            // tsbtnAniTag
            // 
            this.tsbtnAniTag.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.tsbtnAniTag.Font = new System.Drawing.Font("Malgun Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tsbtnAniTag.Image = ((System.Drawing.Image)(resources.GetObject("tsbtnAniTag.Image")));
            this.tsbtnAniTag.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnAniTag.Name = "tsbtnAniTag";
            this.tsbtnAniTag.Size = new System.Drawing.Size(45, 25);
            this.tsbtnAniTag.Text = "{ani}";
            this.tsbtnAniTag.ToolTipText = "{ani} 추가 - F4";
            this.tsbtnAniTag.Click += new System.EventHandler(this.tsbtnAniTag_Click);
            // 
            // lbTextLen
            // 
            this.lbTextLen.AutoSize = true;
            this.lbTextLen.Location = new System.Drawing.Point(14, 524);
            this.lbTextLen.Name = "lbTextLen";
            this.lbTextLen.Size = new System.Drawing.Size(101, 13);
            this.lbTextLen.TabIndex = 11;
            this.lbTextLen.Text = "Number of Chars : 0";
            // 
            // tbEditor
            // 
            this.tbEditor.AcceptsReturn = true;
            this.tbEditor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.tbEditor.Font = new System.Drawing.Font("Malgun Gothic", 9.75F);
            this.tbEditor.Location = new System.Drawing.Point(10, 34);
            this.tbEditor.Multiline = true;
            this.tbEditor.Name = "tbEditor";
            this.tbEditor.Size = new System.Drawing.Size(295, 471);
            this.tbEditor.TabIndex = 13;
            this.tbEditor.TextChanged += new System.EventHandler(this.tbEditor_TextChanged);
            // 
            // SHCE_TextEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(1043, 611);
            this.Controls.Add(this.tbEditor);
            this.Controls.Add(this.lbTextLen);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.wbViewText);
            this.Controls.Add(this.cbIDOnly);
            this.Controls.Add(this.cbSearchText);
            this.Controls.Add(this.lvList);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SHCE_TextEditor";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Text Editor";
            this.TopMost = true;
            this.Load += new System.EventHandler(this.VelixianTextEditor_Load);
            this.Shown += new System.EventHandler(this.SHCE_TextEditor_Shown);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.SHCE_TextEditor_KeyDown);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListView lvList;
        private System.Windows.Forms.ColumnHeader colKey;
        private System.Windows.Forms.ColumnHeader colValue;
        private System.Windows.Forms.ComboBox cbSearchText;
        private System.Windows.Forms.CheckBox cbIDOnly;
        private System.Windows.Forms.WebBrowser wbViewText;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton tsbtnReplaceCharacterTag;
        private System.Windows.Forms.ToolStripButton tsbtnReplaceItemTag;
        private System.Windows.Forms.ToolStripButton tsbtnReplacePlaceTag;
        private System.Windows.Forms.ToolStripButton tsbtnAniTag;
        private System.Windows.Forms.Label lbTextLen;
        private System.Windows.Forms.TextBox tbEditor;
    }
}