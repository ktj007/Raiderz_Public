namespace ChannelListViewer
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.listviewChannel = new System.Windows.Forms.ListView();
            this.colServerID = new System.Windows.Forms.ColumnHeader();
            this.colChannel = new System.Windows.Forms.ColumnHeader();
            this.colType = new System.Windows.Forms.ColumnHeader();
            this.btnServerListXML = new System.Windows.Forms.Button();
            this.textboxServerListXML = new System.Windows.Forms.TextBox();
            this.btnFieldListXML = new System.Windows.Forms.Button();
            this.textboxFieldListXML = new System.Windows.Forms.TextBox();
            this.openFileDialog_serverlistXML = new System.Windows.Forms.OpenFileDialog();
            this.openFileDialog_fieldlistXML = new System.Windows.Forms.OpenFileDialog();
            this.listviewFieldList = new System.Windows.Forms.ListView();
            this.colFieldID = new System.Windows.Forms.ColumnHeader();
            this.colFieldName = new System.Windows.Forms.ColumnHeader();
            this.colChannelCnt = new System.Windows.Forms.ColumnHeader();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.checkboxSortByServer = new System.Windows.Forms.CheckBox();
            this.btnRefreshFieldList = new System.Windows.Forms.Button();
            this.btnRefreshServerList = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label1.Location = new System.Drawing.Point(12, 106);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "FieldList";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label2.Location = new System.Drawing.Point(312, 106);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(62, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "ChannelList";
            // 
            // listviewChannel
            // 
            this.listviewChannel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listviewChannel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colServerID,
            this.colChannel,
            this.colType});
            this.listviewChannel.Location = new System.Drawing.Point(315, 123);
            this.listviewChannel.Name = "listviewChannel";
            this.listviewChannel.Size = new System.Drawing.Size(244, 542);
            this.listviewChannel.TabIndex = 2;
            this.listviewChannel.UseCompatibleStateImageBehavior = false;
            this.listviewChannel.View = System.Windows.Forms.View.Details;
            // 
            // colServerID
            // 
            this.colServerID.Text = "ServerID";
            this.colServerID.Width = 59;
            // 
            // colChannel
            // 
            this.colChannel.Text = "Channel";
            this.colChannel.Width = 61;
            // 
            // colType
            // 
            this.colType.Text = "Type";
            this.colType.Width = 100;
            // 
            // btnServerListXML
            // 
            this.btnServerListXML.Location = new System.Drawing.Point(12, 41);
            this.btnServerListXML.Name = "btnServerListXML";
            this.btnServerListXML.Size = new System.Drawing.Size(97, 23);
            this.btnServerListXML.TabIndex = 3;
            this.btnServerListXML.Text = "serverlist.xml";
            this.btnServerListXML.UseVisualStyleBackColor = true;
            this.btnServerListXML.Click += new System.EventHandler(this.btnServerListXML_Click);
            // 
            // textboxServerListXML
            // 
            this.textboxServerListXML.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textboxServerListXML.Location = new System.Drawing.Point(115, 43);
            this.textboxServerListXML.Name = "textboxServerListXML";
            this.textboxServerListXML.ReadOnly = true;
            this.textboxServerListXML.Size = new System.Drawing.Size(363, 21);
            this.textboxServerListXML.TabIndex = 4;
            // 
            // btnFieldListXML
            // 
            this.btnFieldListXML.Location = new System.Drawing.Point(12, 12);
            this.btnFieldListXML.Name = "btnFieldListXML";
            this.btnFieldListXML.Size = new System.Drawing.Size(97, 23);
            this.btnFieldListXML.TabIndex = 3;
            this.btnFieldListXML.Text = "fieldlist.xml";
            this.btnFieldListXML.UseVisualStyleBackColor = true;
            this.btnFieldListXML.Click += new System.EventHandler(this.btnFieldListXML_Click);
            // 
            // textboxFieldListXML
            // 
            this.textboxFieldListXML.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textboxFieldListXML.Location = new System.Drawing.Point(115, 14);
            this.textboxFieldListXML.Name = "textboxFieldListXML";
            this.textboxFieldListXML.ReadOnly = true;
            this.textboxFieldListXML.Size = new System.Drawing.Size(363, 21);
            this.textboxFieldListXML.TabIndex = 4;
            // 
            // openFileDialog_serverlistXML
            // 
            this.openFileDialog_serverlistXML.FileName = "serverlist.xml";
            // 
            // openFileDialog_fieldlistXML
            // 
            this.openFileDialog_fieldlistXML.FileName = "fieldlist.xml";
            // 
            // listviewFieldList
            // 
            this.listviewFieldList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.listviewFieldList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colFieldID,
            this.colFieldName,
            this.colChannelCnt});
            this.listviewFieldList.FullRowSelect = true;
            this.listviewFieldList.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listviewFieldList.Location = new System.Drawing.Point(12, 123);
            this.listviewFieldList.MultiSelect = false;
            this.listviewFieldList.Name = "listviewFieldList";
            this.listviewFieldList.Size = new System.Drawing.Size(297, 542);
            this.listviewFieldList.TabIndex = 5;
            this.listviewFieldList.UseCompatibleStateImageBehavior = false;
            this.listviewFieldList.View = System.Windows.Forms.View.Details;
            this.listviewFieldList.SelectedIndexChanged += new System.EventHandler(this.listviewFieldList_SelectedIndexChanged);
            // 
            // colFieldID
            // 
            this.colFieldID.Text = "FieldID";
            this.colFieldID.Width = 51;
            // 
            // colFieldName
            // 
            this.colFieldName.Text = "FieldName";
            this.colFieldName.Width = 133;
            // 
            // colChannelCnt
            // 
            this.colChannelCnt.Text = "Channel(D/S/R)";
            this.colChannelCnt.Width = 105;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label3.Location = new System.Drawing.Point(197, 80);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(58, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "D : Default";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label4.Location = new System.Drawing.Point(197, 93);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(50, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "S : Static";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label5.Location = new System.Drawing.Point(197, 106);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(70, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "R : Reserved";
            // 
            // checkboxSortByServer
            // 
            this.checkboxSortByServer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.checkboxSortByServer.AutoSize = true;
            this.checkboxSortByServer.Location = new System.Drawing.Point(462, 106);
            this.checkboxSortByServer.Name = "checkboxSortByServer";
            this.checkboxSortByServer.Size = new System.Drawing.Size(97, 16);
            this.checkboxSortByServer.TabIndex = 6;
            this.checkboxSortByServer.Text = "SortByServer";
            this.checkboxSortByServer.UseVisualStyleBackColor = true;
            this.checkboxSortByServer.CheckedChanged += new System.EventHandler(this.checkboxSortByServer_CheckedChanged);
            // 
            // btnRefreshFieldList
            // 
            this.btnRefreshFieldList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRefreshFieldList.Location = new System.Drawing.Point(484, 12);
            this.btnRefreshFieldList.Name = "btnRefreshFieldList";
            this.btnRefreshFieldList.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshFieldList.TabIndex = 7;
            this.btnRefreshFieldList.Text = "Refresh";
            this.btnRefreshFieldList.UseVisualStyleBackColor = true;
            this.btnRefreshFieldList.Click += new System.EventHandler(this.btnRefreshFieldList_Click);
            // 
            // btnRefreshServerList
            // 
            this.btnRefreshServerList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRefreshServerList.Location = new System.Drawing.Point(484, 41);
            this.btnRefreshServerList.Name = "btnRefreshServerList";
            this.btnRefreshServerList.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshServerList.TabIndex = 7;
            this.btnRefreshServerList.Text = "Refresh";
            this.btnRefreshServerList.UseVisualStyleBackColor = true;
            this.btnRefreshServerList.Click += new System.EventHandler(this.btnRefreshServerList_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(571, 677);
            this.Controls.Add(this.btnRefreshServerList);
            this.Controls.Add(this.btnRefreshFieldList);
            this.Controls.Add(this.checkboxSortByServer);
            this.Controls.Add(this.listviewFieldList);
            this.Controls.Add(this.textboxFieldListXML);
            this.Controls.Add(this.textboxServerListXML);
            this.Controls.Add(this.btnFieldListXML);
            this.Controls.Add(this.btnServerListXML);
            this.Controls.Add(this.listviewChannel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "ChannelListViewer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ListView listviewChannel;
        private System.Windows.Forms.ColumnHeader colChannel;
        private System.Windows.Forms.ColumnHeader colServerID;
        private System.Windows.Forms.Button btnServerListXML;
        private System.Windows.Forms.TextBox textboxServerListXML;
        private System.Windows.Forms.Button btnFieldListXML;
        private System.Windows.Forms.TextBox textboxFieldListXML;
        private System.Windows.Forms.OpenFileDialog openFileDialog_serverlistXML;
        private System.Windows.Forms.OpenFileDialog openFileDialog_fieldlistXML;
        private System.Windows.Forms.ListView listviewFieldList;
        private System.Windows.Forms.ColumnHeader colFieldID;
        private System.Windows.Forms.ColumnHeader colChannelCnt;
        private System.Windows.Forms.ColumnHeader colFieldName;
        private System.Windows.Forms.ColumnHeader colType;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox checkboxSortByServer;
        private System.Windows.Forms.Button btnRefreshFieldList;
        private System.Windows.Forms.Button btnRefreshServerList;


    }
}

