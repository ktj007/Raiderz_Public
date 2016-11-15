using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitor
{
    public partial class FormSettingsAddServer : Form
    {
        private readonly string TITLE_FIND_SERVER_EXE_FILE_WND = "ServerMonitor 에 등록할 Server의 EXE파일을 선택해 주세요";
        private readonly string FILTER_FIND_SERVER_EXE_FILE = "Server EXE File(*.exe)|*.exe";

        private readonly string ERRSTR_NOT_EXIST_SERVER_EXE_PATH = "관리할 서버의 EXE 파일 경로가 필요합니다.";
        private readonly string ERRSTR_NOT_EXIST_SERVER_NAME = "관리할 서버의 이름이 필요합니다. 설정파일(.ini)을 확인하세요.";
        private readonly string ERRSTR_NOT_EXIST_WORLD_SERVER_ID = "Server 설정 INI 파일에 World ID 혹은 Server ID 속성값이 없거나 잘못 되었습니다. 설정파일(.ini)을 확인하세요.";
        private readonly string ERRSTR_WORLD_SERVER_ID_IS_ZERO = "Server 설정 INI 파일에 World ID 혹은 Server ID 속성값은 0 이 될 수 없습니다. 설정파일(.ini)을 확인하세요.";
        
        private readonly string ERRSTR_ALREADY_EXIST_EXE_PATH = "동일한 EXE 파일 경로를 가진 서버가 이미 등록되어 있습니다.";


        ServerInfoMgr m_refServerInfoMgr = null;

        public FormSettingsAddServer(ServerInfoMgr refServerInfoMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;

            InitializeComponent();

            comboboxServerType.Items.Add(SERVER_TYPE.SERVER_LOGIN);
            comboboxServerType.Items.Add(SERVER_TYPE.SERVER_MASTER);
            comboboxServerType.Items.Add(SERVER_TYPE.SERVER_GAME);
            comboboxServerType.Items.Add(SERVER_TYPE.SERVER_APPLICATION);

            comboboxServerType.SelectedIndex = 0;
        }

        private void btnFindExeFile_Click(object sender, EventArgs e)
        {
            opendlgExeFilePath.Title = TITLE_FIND_SERVER_EXE_FILE_WND;
            opendlgExeFilePath.Filter = FILTER_FIND_SERVER_EXE_FILE;
            opendlgExeFilePath.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            
            if (opendlgExeFilePath.ShowDialog()  == DialogResult.OK)
            {
                System.Environment.CurrentDirectory = Application.StartupPath;

                System.Uri uriAbsolutePath = new Uri(opendlgExeFilePath.FileName);
                System.Uri uriCurrentPath = new Uri(Application.StartupPath + "\\");

                System.Uri uriRelativePath = uriCurrentPath.MakeRelativeUri(uriAbsolutePath);

                textboxExePath.Text = uriRelativePath.ToString();
                textboxProcessName.Text = System.IO.Path.GetFileNameWithoutExtension(textboxExePath.Text);
                
                string strServerName;
                int nWorldID;
                int nServerID;
                Config.ReadServerINI(textboxExePath.Text, out strServerName, out nWorldID, out nServerID);
                if (strServerName.Length == 0)
                {
                    MessageBox.Show(ERRSTR_NOT_EXIST_SERVER_NAME);
                    btnFindExeFile.Focus();
                }

                if (nWorldID == 0 || nServerID == 0)
                {
                    MessageBox.Show(ERRSTR_WORLD_SERVER_ID_IS_ZERO);
                    btnFindExeFile.Focus();
                }

                textboxServerName.Text = strServerName;
                textboxWorldID.Text = nWorldID.ToString();
                textboxServerID.Text = nServerID.ToString();
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (textboxExePath.TextLength == 0)
            {
                MessageBox.Show(ERRSTR_NOT_EXIST_SERVER_EXE_PATH);
                btnFindExeFile.Focus();
                return;
            }

            if (textboxServerName.TextLength == 0)
            {
                MessageBox.Show(ERRSTR_NOT_EXIST_SERVER_NAME);
                btnFindExeFile.Focus();
                return;
            }

            if (textboxWorldID.Text.Length == 0 || textboxServerID.Text.Length == 0)
            {
                MessageBox.Show(ERRSTR_NOT_EXIST_WORLD_SERVER_ID);
                btnFindExeFile.Focus();
                return;
            }

            SERVER_TYPE eServerType = (SERVER_TYPE)comboboxServerType.SelectedIndex;
            string strServerName = textboxServerName.Text;
            string strExePath = textboxExePath.Text;
            int nWorldID = int.Parse(textboxWorldID.Text);
            int nServerID = int.Parse(textboxServerID.Text);

            if (nWorldID == 0 || nServerID == 0)
            {
                MessageBox.Show(ERRSTR_WORLD_SERVER_ID_IS_ZERO);
                btnFindExeFile.Focus();
                return;
            }

            if (m_refServerInfoMgr.IsExistServerInfo(strExePath) == true)
            {
                MessageBox.Show(ERRSTR_ALREADY_EXIST_EXE_PATH);
                btnFindExeFile.Focus();
                return;
            }

            if (m_refServerInfoMgr.AddServerInfo(eServerType, strServerName, strExePath, nWorldID, nServerID) == false)
            {
                MessageBox.Show("서버 정보 추가 실패!");
                return;
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

    }
}
