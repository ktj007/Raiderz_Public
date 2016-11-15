using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitorClient
{
    public partial class FormAutoModeClient : Form
    {
        AutoModeClient m_refModel;

        public FormAutoModeClient(AutoModeClient refModel)
        {
            m_refModel = refModel;

            InitializeComponent();
        }


        private void AutoModeForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_refModel.Destroy();
        }

        private void timerMainUpdate_Tick(object sender, EventArgs e)
        {
            m_refModel.Update();
        }

        private void timerAutoModeTimeout_Tick(object sender, EventArgs e)
        {
            Timer refSender = (Timer)sender;

            System.Windows.Forms.Application.Exit();
            FileLog.Instance.Write("타임아웃! {0} 초간 정상처리 되지 않아 종료합니다!", (float)(refSender.Interval*0.001));
        }

    }
}
