using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using SMCommon;

namespace ServerMonitor
{
    public class ConfigConst
    {
        public static readonly string INI_FILE_NAME = "./servermonitor.ini";
        public static readonly string INI_FILE_NAME_REL = "./servermonitor_rel.ini";
        public static readonly string INI_APPNAME_DEFAULT_CONFIG = "DEFAULT_CONFIG";
        public static readonly string INI_DEFAULT_KEYNAME_ID = "ID";
        public static readonly string INI_DEFAULT_KEYNAME_PORT = "PORT";
        public static readonly string INI_APPNAME_DB_CONFIG = "DB_CONFIG";
        public static readonly string INI_DB_KEYNAME_DATA_SOURCE = "DATA_SOURCE";
        public static readonly string INI_DB_KEYNAME_INITIAL_CATALOG = "INITIAL_CATALOG";
        public static readonly string INI_DB_KEYNAME_USER_ID = "USER_ID";
        public static readonly string INI_DB_KEYNAME_PWD = "PWD";

        public static readonly string CONFIG_FILE_NAME = "./ServerMonitor.config";
        public static readonly string CONFIG_FILE_NAME_REL = "./ServerMonitor_rel.config";
        public static readonly string ELEMENT_NAME_ROOT = "ServerMonitor";
        public static readonly string ELEMENT_NAME_SERVERINFO = "ServerInfo";
        public static readonly string ATT_NAME_SERVERINFO_TYPE = "type";
        public static readonly string ATT_NAME_SERVERINFO_EXEPATH = "exe_path";
        public static readonly string ATT_NAME_SERVERINFO_PROCESS_NAME = "process_name";

        public static readonly string SERVER_INI_FILE_NAME = "server.ini";
        public static readonly string SERVER_INI_FILE_NAME_REL = "server_rel.ini";
        public static readonly string SERVER_INI_APPNAME_SERVER_INFO = "SERVER_INFO";
        public static readonly string SERVER_INI_KEYNAME_NAME = "NAME";
        public static readonly string SERVER_INI_KEYNAME_WORLD_ID = "WORLD_ID";
        public static readonly string SERVER_INI_KEYNAME_SERVER_ID = "SERVER_ID";
    }

    public class Config
    {
        [DllImport("kernel32.dll")]
        private static extern int GetPrivateProfileInt(String strAppName, String strKeyName, int nDefault, String strFilePath);
        [DllImport("kernel32.dll")]
        private static extern int GetPrivateProfileString(String strAppName, String strKeyName, String strDefault, StringBuilder retVal, int size, String strFilePath);

        private ConfigFileController m_refConfigFileController;
        private int m_nID = 0;
        private int m_nPort = 0;
        private List<ServerInfo> m_listServerInfo = new List<ServerInfo>();

        private string m_strDataSource;
        private string m_strInitialCatalog;
        private string m_strUserID;
        private string m_strPassword;

        public int ServerMonitorID { get { return m_nID; } }
        public int PortNumber { get { return m_nPort; } }
        public List<ServerInfo> refListServerInfo { get { return m_listServerInfo; } }

        public string DB_DataSource { get { return m_strDataSource; } }
        public string DB_InitialCatalog { get { return m_strInitialCatalog; } }
        public string DB_UserID { get { return m_strUserID; } }
        public string DB_Password { get { return m_strPassword; } }

        public Config(ConfigFileController refConfigFileController)
        {
            m_refConfigFileController = refConfigFileController;
        }

        public bool ReadConfigINI()
        {
            string strPath = ConfigConst.INI_FILE_NAME;
            if (System.IO.File.Exists(strPath) == false)
                strPath = ConfigConst.INI_FILE_NAME_REL;

            // Read Default Setting
            m_nID = GetPrivateProfileInt(ConfigConst.INI_APPNAME_DEFAULT_CONFIG, ConfigConst.INI_DEFAULT_KEYNAME_ID, 0, strPath);
            m_nPort = GetPrivateProfileInt(ConfigConst.INI_APPNAME_DEFAULT_CONFIG, ConfigConst.INI_DEFAULT_KEYNAME_PORT, 0, strPath);

            if (m_nID == 0 || m_nPort == 0)
                return false;

            // Read DB Connection Setting
            StringBuilder temp = new StringBuilder(128);
            GetPrivateProfileString(ConfigConst.INI_APPNAME_DB_CONFIG, ConfigConst.INI_DB_KEYNAME_DATA_SOURCE, "", temp, temp.Capacity, strPath);
            m_strDataSource = temp.ToString();

            GetPrivateProfileString(ConfigConst.INI_APPNAME_DB_CONFIG, ConfigConst.INI_DB_KEYNAME_INITIAL_CATALOG, "", temp, temp.Capacity, strPath);
            m_strInitialCatalog = temp.ToString();

            GetPrivateProfileString(ConfigConst.INI_APPNAME_DB_CONFIG, ConfigConst.INI_DB_KEYNAME_USER_ID, "", temp, temp.Capacity, strPath);
            m_strUserID = temp.ToString();

            GetPrivateProfileString(ConfigConst.INI_APPNAME_DB_CONFIG, ConfigConst.INI_DB_KEYNAME_PWD, "", temp, temp.Capacity, strPath);
            m_strPassword = temp.ToString();

            return true;
        }

        public bool LoadConfig()
        {
            XmlDocument oDoc;
            LoadConfigFile(out oDoc);

            XmlElement elementRoot = oDoc.DocumentElement;
            XmlNode nodeConfig = elementRoot.FirstChild;

            while (nodeConfig != null)
            {
                if (nodeConfig.Name.Equals(ConfigConst.ELEMENT_NAME_SERVERINFO) == true)
                {
                    ServerInfo refServerInfo = Parse_ServerInfo(nodeConfig);
                    if (null == refServerInfo)
                    {
                        FileLog.Instance.Write("실패! {0} Element의 속성을 읽지 못했습니다.", ConfigConst.ELEMENT_NAME_SERVERINFO);
                        return false;
                    }

                    m_listServerInfo.Add(refServerInfo);
                }

                nodeConfig = nodeConfig.NextSibling;
            }
            return true;
        }

        public bool AddServerInfo(SERVER_TYPE eServerType, string strExePath, string strProcessName)
        {
            if (strExePath.Length == 0 || strProcessName.Length == 0)
                return false;

            XmlDocument xmlConfigDoc;
            LoadConfigFile(out xmlConfigDoc);

            XmlElement nodeServerInfo = xmlConfigDoc.CreateElement(ConfigConst.ELEMENT_NAME_SERVERINFO);
            XmlAttribute att = null;

            int nServerType = (int)eServerType;
            att = xmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_TYPE);
            att.Value = nServerType.ToString();
            nodeServerInfo.Attributes.Append(att);

            att = xmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_EXEPATH);
            att.Value = strExePath;
            nodeServerInfo.Attributes.Append(att);

            att = xmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_PROCESS_NAME);
            att.Value = strProcessName;
            nodeServerInfo.Attributes.Append(att);

            XmlElement elementRoot = xmlConfigDoc.DocumentElement;
            elementRoot.AppendChild(nodeServerInfo);

            m_refConfigFileController.SaveConfig(ConfigConst.CONFIG_FILE_NAME, xmlConfigDoc);
            return true;
        }
        public bool DeleteServerInfo(string strExePath)
        {
            if (strExePath.Length == 0) return false;

            XmlDocument xmlConfigDoc;
            LoadConfigFile(out xmlConfigDoc);

            XmlElement elementRoot = xmlConfigDoc.DocumentElement;
            XmlNode nodeServerInfo = elementRoot.FirstChild;

            while (nodeServerInfo != null)
            {
                if (nodeServerInfo.Name.Equals(ConfigConst.ELEMENT_NAME_SERVERINFO) == false)
                {
                    nodeServerInfo = nodeServerInfo.NextSibling;
                    continue;
                }

                if (nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_EXEPATH] == null)
                    return false;

                if (nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_EXEPATH].Value.Equals(strExePath) == true)
                {
                    elementRoot.RemoveChild(nodeServerInfo);
                    break;
                }
                else
                {
                    nodeServerInfo = nodeServerInfo.NextSibling;
                }
            }
            m_refConfigFileController.SaveConfig(ConfigConst.CONFIG_FILE_NAME, xmlConfigDoc);
            return true;
        }

        public static void ReadServerINI(string strExePath, out string strServerName, out int nWorldID, out int nServerID)
        {
            string strPath = System.IO.Path.GetDirectoryName(strExePath);
            strPath += "\\";
            strPath += ConfigConst.SERVER_INI_FILE_NAME;

            if (System.IO.File.Exists(strPath) == false)
            {
                strPath = System.IO.Path.GetDirectoryName(strExePath);
                strPath += "\\";
                strPath += ConfigConst.SERVER_INI_FILE_NAME_REL;
            }

            StringBuilder temp = new StringBuilder(256);
            GetPrivateProfileString(ConfigConst.SERVER_INI_APPNAME_SERVER_INFO, ConfigConst.SERVER_INI_KEYNAME_NAME, "", temp, 256, strPath);
            strServerName = temp.ToString();

            nWorldID = GetPrivateProfileInt(ConfigConst.SERVER_INI_APPNAME_SERVER_INFO, ConfigConst.SERVER_INI_KEYNAME_WORLD_ID, 0, strPath);
            nServerID = GetPrivateProfileInt(ConfigConst.SERVER_INI_APPNAME_SERVER_INFO, ConfigConst.SERVER_INI_KEYNAME_SERVER_ID, 0, strPath);
        }
        public static void ReadServerINI_ServerID(string strExePath, out int nWorldID, out int nServerID)
        {
            string strNotUse;
            ReadServerINI(strExePath, out strNotUse, out nWorldID, out nServerID);
        }

        private bool LoadConfigFile(out XmlDocument oDoc)
        {
            string strConfigFileName = ConfigConst.CONFIG_FILE_NAME;
            if (System.IO.File.Exists(strConfigFileName) == false)
                strConfigFileName = ConfigConst.CONFIG_FILE_NAME_REL;

            if (m_refConfigFileController.IsExistFile(strConfigFileName) == false)
            {
                m_refConfigFileController.CreateConfigFile(strConfigFileName, ConfigConst.ELEMENT_NAME_ROOT, out oDoc);
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

        private ServerInfo Parse_ServerInfo(XmlNode nodeServerInfo)
        {
            if (nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_TYPE] == null || 
                nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_EXEPATH] == null ||
                nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_PROCESS_NAME] == null)
            {
                return null;
            }

            string strServerType = nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_TYPE].Value;
            string strExePath = nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_EXEPATH].Value;
            string strProcessName = nodeServerInfo.Attributes[ConfigConst.ATT_NAME_SERVERINFO_PROCESS_NAME].Value;

            if (strServerType.Length == 0 ||
                strExePath.Length == 0 ||
                strProcessName.Length == 0)
            {
                return null;
            }

            int nServerType;
            if (int.TryParse(strServerType, out nServerType) == false)
                return null;

            string strServerName;
            int nWorldID;
            int nServerID;
            ReadServerINI(strExePath, out strServerName, out nWorldID, out nServerID);

            ServerInfo serverInfo = new ServerInfo((SERVER_TYPE)nServerType, strExePath, strProcessName, strServerName, nWorldID, nServerID);
            return serverInfo;
        }

    }
}
