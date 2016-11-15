using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using SMCommon;

namespace ServerMonitorClient
{
    public class ConfigConst
    {
        public static readonly string CONFIG_FILE_NAME = "./ServerMonitorClient.config";
        public static readonly string CONFIG_FILE_NAME_REL = "./ServerMonitorClient_rel.config";

        public static readonly string ELEMENT_NAME_ROOT = "ServerMonitorClient";
        public static readonly string ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT = "automode_local_port";

        public static readonly string ELEMENT_NAME_MACHINE = "Machine";
        public static readonly string ATT_NAME_MACHINE_NAME = "name";
        public static readonly string ATT_NAME_MACHINE_IP = "ip";
        public static readonly string ATT_NAME_MACHINE_PORT = "port";
    }

    public class Config
    {
        ConfigFileController m_refConfigFileController;

        public Config(ConfigFileController refConfigFileController)
        {
            m_refConfigFileController = refConfigFileController;
        }

        public int GetAutoModeLocalPort()
        {
            XmlDocument oDoc;
            LoadConfigFile(out oDoc);

            XmlElement elementRoot = oDoc.DocumentElement;

            if (elementRoot.Attributes[ConfigConst.ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT] == null)
                return 0;

            int nPort = int.Parse(elementRoot.Attributes[ConfigConst.ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT].Value);

            return nPort;
        }

        public bool LoadConfig(out MachineInfo[] arrMachineInfo)
        {
            arrMachineInfo = null;

            XmlDocument oDoc;
            LoadConfigFile(out oDoc);

            XmlElement elementRoot = oDoc.DocumentElement;
            XmlNode nodeMachine = elementRoot.FirstChild;

            LinkedList<MachineInfo> listMachineInfo = new LinkedList<MachineInfo>();

            while (nodeMachine != null)
            {
                if (nodeMachine.Name.Equals(ConfigConst.ELEMENT_NAME_MACHINE) == true)
                {
                    MachineInfo machineInfo;
                    if (ReadNode_MachineInfo(nodeMachine, out machineInfo) == false)
                    {
                        FileLog.Instance.Write("실패! {0} Element의 속성을 읽지 못했습니다.", ConfigConst.ELEMENT_NAME_MACHINE);
                        return false;
                    }

                    listMachineInfo.AddLast(machineInfo);

                    XmlNode nodeServerInfo = nodeMachine.FirstChild;
                }

                nodeMachine = nodeMachine.NextSibling;
            }

            arrMachineInfo = new MachineInfo[listMachineInfo.Count];
            LinkedListNode<MachineInfo> nodeMachineInfoList = listMachineInfo.First;
            for (int i = 0; i < listMachineInfo.Count; ++i)
            {
                arrMachineInfo[i] = nodeMachineInfoList.Value;
                nodeMachineInfoList = nodeMachineInfoList.Next;
            }
            return true;
        }

        private bool LoadConfigFile(out XmlDocument oDoc)
        {
            string strConfigFileName = ConfigConst.CONFIG_FILE_NAME;
            if (System.IO.File.Exists(strConfigFileName) == false)
                strConfigFileName = ConfigConst.CONFIG_FILE_NAME_REL;

            if (m_refConfigFileController.IsExistFile(strConfigFileName) == false)
            {
                m_refConfigFileController.CreateConfigFile(strConfigFileName, ConfigConst.ELEMENT_NAME_ROOT, out oDoc);
                m_refConfigFileController.AddAttributeToRootNode(strConfigFileName, ConfigConst.ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT, "0");
            }
            else
            {
                if (m_refConfigFileController.LoadConfigFile(strConfigFileName, out oDoc) == false)
                {
                    FileLog.Instance.Write("실패! 컨피그 파일을 로드하지 못했습니다.");
                    return false;
                }
            }
            return true;
        }

        private bool ReadNode_MachineInfo(XmlNode nodeMachine, out MachineInfo machineInfo)
        {
            machineInfo = new MachineInfo();

            if (nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_NAME] == null ||
                nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_IP] == null ||
                nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_PORT] == null)
            {
                return false;
            }

            string strName = nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_NAME].Value;
            string strIP = nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_IP].Value;
            string strPort = nodeMachine.Attributes[ConfigConst.ATT_NAME_MACHINE_PORT].Value;

            if (strName.Length == 0 || strPort.Length == 0)
            {
                FileLog.Instance.Write("실패! Config.xml 에서 {0} 과 {0} 는 공백일 수 없습니다.", ConfigConst.ATT_NAME_MACHINE_NAME, ConfigConst.ATT_NAME_MACHINE_PORT);
                return false;
            }

            machineInfo.MachineName = strName;
            machineInfo.IP = strIP;
            machineInfo.Port = int.Parse(strPort);
            return true;
        }

    }
}
