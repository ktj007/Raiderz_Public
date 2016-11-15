using System;
using System.ComponentModel;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Xml;
using System.Xml.Schema;
using SoulHunt;

namespace Velixian
{
    public delegate void EventLoadAfter(bool bFirst);

    public partial class SplashForm : Form
    {
        private static bool _bFirst = true;
        internal bool RetestRequired;
        private EventLoadAfter _LoadAfter;

        #region 생성자
        public SplashForm()
        {
            InitializeComponent();
            RetestRequired = false;
            _LoadAfter = null;
        }
        #endregion

        #region 윈도우 이벤트 핸들러

        private void SplashForm_Load(object sender, EventArgs e)
        {
            bgWorker.RunWorkerAsync();
        }

        #endregion

        public delegate void progressUpdateDelegate(string strMessage);

        #region 로딩스레드

        private void bgWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                SHGlobal.CheckFiles(bgWorker);
                RetestRequired = false;
            }
            catch (Exception ex)
            {
                progressUpdateDelegate td = new progressUpdateDelegate(OutputError);
                this.Invoke(td, new object[] { ex.Message });

                //DialogResult result = MessageBox.Show(ex.Message + "\n\n이 버그를 수정한 뒤에 \"재시도\"를 누르시면 처음부터 다시 시작됩니다.", Application.ProductName, MessageBoxButtons.AbortRetryIgnore);

                //if (result == DialogResult.Retry)
                //{
                //    bgWorker.CancelAsync();
                //    RetestRequired = true;
                //}
                //else if (result == DialogResult.Abort)
                //{
                //    Environment.Exit(-1);
                //}
            }
            bgWorker.ReportProgress(100, Environment.NewLine + "초기화를 하고 있습니다.");
        }
        private void OutputError(string strMessage)
        {
            Global.OutputError(strMessage);
        }

        private void bgWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            pgBar.Value = e.ProgressPercentage;

            tbSplash.SelectedText += e.UserState.ToString() + "\r\n";
            tbSplash.ScrollToCaret();
        }

        private void bgWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (RetestRequired)
            {
                bgWorker.RunWorkerAsync();
            }
            else
            {
                if (_LoadAfter != null) _LoadAfter(_bFirst);

                _bFirst = false;

                this.Close();
            }
        }
        
        #endregion

        public void SetLoadAfter(EventLoadAfter work)
        {
            _LoadAfter = work;
        }
    }
}
