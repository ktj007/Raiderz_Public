using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public enum AUTO_MODE_TYPE
    {
        NONE,
        JUST_START,
        JUST_STOP,
    }

    public class AutoModeClient
    {
        private readonly string ADDR_LOCAL = "127.0.0.1";

        FormAutoModeClient m_refView;

        private AUTO_MODE_TYPE m_eMode;
        private UInt64 m_nSessionID = 0;
        private NetClient m_netClient = null;
        private CommandSender m_commandSender = null;
        private string m_strTargetServerName;

        public AUTO_MODE_TYPE Mode { get { return m_eMode; } }
        public CommandSender refCommandSender { get { return m_commandSender; } }
        public string TargetServerName { get { return m_strTargetServerName; } }

        public bool Init(FormAutoModeClient refView, string[] args)
        {
            m_refView = refView;

            string strErrMessage;
            if (CheckArgs(args, out strErrMessage) == false)
            {
                FileLog.Instance.Write(strErrMessage);
                return false;
            }

            m_netClient = new NetClient();
            m_commandSender = new CommandSender(m_netClient);
            m_netClient.SetCommandHandler(new CommandHandler_AutoMode(this, m_commandSender));

            m_eMode = GetAutoModeByString(args[0]);
            if (m_eMode == AUTO_MODE_TYPE.JUST_START)
            {
                m_strTargetServerName = args[1];
            }
            else if (m_eMode == AUTO_MODE_TYPE.JUST_STOP)
            {
                m_strTargetServerName = args[1];
            }

            return true;
        }

        public void Destroy()
        {
            m_netClient.Destroy();
        }

        public void Update()
        {
            m_netClient.Update();
        }

        public bool StartAutoMode()
        {
            Config config = new Config(new ConfigFileController());

            int nPort = config.GetAutoModeLocalPort();
            if (nPort == 0)
            {
                FileLog.Instance.Write("실패! 컨피그 파일의 루트 엘러먼트 속성 \"{0}\" 의 값이 0입니다!", ConfigConst.ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT);
                return false;
            }

            if (m_netClient.NewClientSession("", ADDR_LOCAL, nPort, out m_nSessionID) == false)
                return false;

            return true;
        }

        public bool OnConnected()
        {
            m_commandSender.SendCheckCommandVersionReq(m_nSessionID, CMD.COMMAND_VERSION);
            return true;
        }


        private bool CheckArgs(string[] args, out string strErrMessage)
        {
            strErrMessage = "";

            if (args.Length < 1)
            {
                strErrMessage = "실행 인자가 올바르지 않습니다.\nServerMonitorClient.exe [Mode]";
                return false;
            }

            string strMode = args[0];

            if (strMode.Equals("jstart") == true)
            {
                if (args.Length != 2)
                {
                    strErrMessage = "실행 인자가 올바르지 않습니다.\nServerMonitorClient.exe jstart [ServerName]";
                    return false;
                }
            }
            else if (strMode.Equals("jstop") == true)
            {
                if (args.Length != 2)
                {
                    strErrMessage = "실행 인자가 올바르지 않습니다.\nServerMonitorClient.exe jstop [ServerName]";
                    return false;
                }
            }
            else
            {
                strErrMessage = "실행 인자가 올바르지 않습니다.\nServerMonitorClient.exe [Mode](jstart or jstop)";
                return false;
            }

            return true;
        }

        private AUTO_MODE_TYPE GetAutoModeByString(string strMode)
        {
            if (strMode.Equals("jstart") == true)
            {
                return AUTO_MODE_TYPE.JUST_START;
            }
            else if (strMode.Equals("jstop") == true)
            {
                return AUTO_MODE_TYPE.JUST_STOP;
            }

            return AUTO_MODE_TYPE.NONE;
        }

    }
}
