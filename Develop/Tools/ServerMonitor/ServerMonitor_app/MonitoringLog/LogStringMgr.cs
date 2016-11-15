using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using SMCommon;

namespace ServerMonitor
{
    public struct LOGSTRING 
    {
        public string strName;
        public string strString;
    }

    public class LogStringMgr
    {
        private readonly string LOG_STRING_TABLE_FILE_NAME = "ServerMonitorStringTable.xml";
        private readonly string ELEMENT_NAME_LOGSTRING = "LogString";
        private readonly string ATT_NAME_ID = "id";
        private readonly string ATT_NAME_NAME = "name";
        private readonly string ATT_NAME_STRING = "string";

        protected Dictionary<ushort, LOGSTRING> m_dicStringTable = new Dictionary<ushort, LOGSTRING>();
        ConfigFileController m_configFileController = new ConfigFileController();

        public virtual bool LoadStringTable()
        {
            XmlDocument oDoc;
            if (LoadStringTableFile(out oDoc) == false)
                return false;

            XmlElement elementRoot = oDoc.DocumentElement;
            XmlNode nodeLogString = elementRoot.FirstChild;

            while (nodeLogString != null)
            {
                if (nodeLogString.Name.Equals(ELEMENT_NAME_LOGSTRING) == true)
                {
                    ushort nID;
                    LOGSTRING logString;
                    if (ReadNode_LogString(nodeLogString, out nID, out logString) == false)
                    {
                        FileLog.Instance.Write("실패! {0} Element의 속성을 읽지 못했습니다. (id={1})", ELEMENT_NAME_LOGSTRING, nID);
                        return false;
                    }

                    m_dicStringTable.Add(nID, logString);
                }

                nodeLogString = nodeLogString.NextSibling;
            }

            return true;
        }
        public bool GetString(ushort nLogStringID, out string strLogMessage)
        {
            try
            {
                LOGSTRING logString = m_dicStringTable[nLogStringID];
                strLogMessage = logString.strString;
            }
            catch
            {
                strLogMessage = "";
                return false;
            }
            return true;
        }

        private bool LoadStringTableFile(out XmlDocument oDoc)
        {
            if (m_configFileController.IsExistFile(LOG_STRING_TABLE_FILE_NAME) == false)
            {
                oDoc = null;
                FileLog.Instance.Write("실패! 파일({0})이 존재하지 않습니다.", LOG_STRING_TABLE_FILE_NAME);
                return false;
            }

            if (m_configFileController.LoadConfigFile(LOG_STRING_TABLE_FILE_NAME, out oDoc) == false)
            {
                FileLog.Instance.Write("실패! 파일({0})을 로드하지 못했습니다.", LOG_STRING_TABLE_FILE_NAME);
                return false;
            }
            return true;
        }
        private bool ReadNode_LogString(XmlNode nodeServerInfo, out ushort nID, out LOGSTRING logString)
        {
            nID = 0;
            logString = new LOGSTRING();

            if (nodeServerInfo.Attributes[ATT_NAME_ID] == null ||
                nodeServerInfo.Attributes[ATT_NAME_NAME] == null ||
                nodeServerInfo.Attributes[ATT_NAME_STRING] == null)
            {
                return false;
            }

            string strID = nodeServerInfo.Attributes[ATT_NAME_ID].Value;
            string strName = nodeServerInfo.Attributes[ATT_NAME_NAME].Value;
            string strString = nodeServerInfo.Attributes[ATT_NAME_STRING].Value;

            if (strID.Length == 0 ||
                strName.Length == 0 ||
                strString.Length == 0)
            {
                return false;
            }

            if (ushort.TryParse(strID, out nID) == false) return false;
            logString.strName = strName;
            logString.strString = strString;
            return true;
        }

    }
}
