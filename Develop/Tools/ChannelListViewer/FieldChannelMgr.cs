using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Windows.Forms;

namespace ChannelListViewer
{
    public class ChannelInfo
    {
        public bool bReserved = false;
        public int nChannelID = 0;
        public int nGameServerID = 0;
    }

    public class FieldInfo
    {
        public int nFieldID = 0;
        public int nDefaultChannel = 0;
        public string strName = "";
        public SortedList<int, ChannelInfo> listChannelInfo = new SortedList<int, ChannelInfo>();
        //public LinkedList<ChannelInfo> listChannelInfo = new LinkedList<ChannelInfo>();

        public int GetStaticChannelCount()
        {
            int nCount = 0;
            for (int i = 0; i < listChannelInfo.Count; ++i)
            {
                ChannelInfo refChannelInfo = listChannelInfo.Values[i];
                if (!refChannelInfo.bReserved)
                    nCount++;
            }
            return nCount;
        }
        public int GetReservedChannelCount()
        {
            int nCount = 0;
            for (int i = 0; i < listChannelInfo.Count; ++i)
            {
                ChannelInfo refChannelInfo = listChannelInfo.Values[i];
                if (refChannelInfo.bReserved)
                    nCount++;
            }
            return nCount;
        }
    }

    class FieldChannelMgr
    {
        private bool m_bIsLoadedFieldList = false;
        private bool m_bIsLoadedServerList = false;

        public bool IsLoadedFieldList { get { return m_bIsLoadedFieldList; } }
        public bool IsLoadedServerList { get { return m_bIsLoadedServerList; } }


        public LinkedList<FieldInfo> m_listFieldInfo = new LinkedList<FieldInfo>();


        public bool LoadFieldList(string strFilePath)
        {
            ClearFieldInfo();
            m_bIsLoadedFieldList = true;
            m_bIsLoadedServerList = false;

            if (System.IO.File.Exists(strFilePath) == false)
                return false;

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(strFilePath);

            XmlElement elementRoot = xmlDoc.DocumentElement;
            XmlNode nodeFIELDLIST = elementRoot.FirstChild;
            if (nodeFIELDLIST.Name.Equals("FIELDLIST") == false)
                return false;

            XmlNode nodeFIELD = nodeFIELDLIST.FirstChild;

            while (nodeFIELD != null)
            {
                if (nodeFIELD.Name.Equals("FIELD") == true)
                {
                    ParseFieldInfo(nodeFIELD);
                }

                nodeFIELD = nodeFIELD.NextSibling;
            }

            return true;
        }

        private bool ParseFieldInfo(XmlNode nodeFIELD)
        {
            if (nodeFIELD.Attributes["id"] == null ||
                 nodeFIELD.Attributes["name"] == null ||
                 nodeFIELD.Attributes["default_channel"] == null)
            {
                return false;
            }

            string strID = nodeFIELD.Attributes["id"].Value;
            string strName = nodeFIELD.Attributes["name"].Value;
            string strDafaultChannel = nodeFIELD.Attributes["default_channel"].Value;

            int nID = int.Parse(strID);
            int nDefaultChannel = int.Parse(strDafaultChannel);

            FieldInfo fieldInfo = new FieldInfo();
            fieldInfo.nFieldID = nID;
            fieldInfo.strName = strName;
            fieldInfo.nDefaultChannel = nDefaultChannel;

            m_listFieldInfo.AddLast(fieldInfo);

            return true;
        }

        public void ClearFieldInfo()
        {
            m_listFieldInfo.Clear();
        }
        public void ClearChannelInfo()
        {
            LinkedListNode<FieldInfo> node = m_listFieldInfo.First;
            for (; node != null; node = node.Next)
            {
                FieldInfo refFieldInfo = node.Value;

                refFieldInfo.listChannelInfo.Clear();
            }
        }

        public bool LoadServerList(string strFilePath)
        {
            ClearChannelInfo();
            m_bIsLoadedServerList = true;

            if (System.IO.File.Exists(strFilePath) == false)
                return false;

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(strFilePath);

            XmlElement elementRoot = xmlDoc.DocumentElement;
            XmlNode nodeServerInfo = elementRoot.FirstChild;

            while (nodeServerInfo != null)
            {
                if (nodeServerInfo.Name.Equals("GameServer") == true)
                {
                    ParseGameServerInfo(nodeServerInfo);
                }

                nodeServerInfo = nodeServerInfo.NextSibling;
            }

            return true;
        }

        private bool ParseGameServerInfo(XmlNode nodeGameServer)
        {
            if (nodeGameServer.Attributes["id"] == null)
                return false;

            string strGameServerID = nodeGameServer.Attributes["id"].Value;
            int nGameServerID = int.Parse(strGameServerID);

            XmlNode nodeChild = nodeGameServer.FirstChild;
            while (nodeChild != null)
            {
                if (nodeChild.Name.Equals("SharedField") == true)
                {
                    ParseSharedField(nodeChild, nGameServerID);
                }
                else if (nodeChild.Name.Equals("ReservedField") == true)
                {
                    ParseReservedField(nodeChild, nGameServerID);
                }

                nodeChild = nodeChild.NextSibling;
            }



            return true;
        }
        private bool ParseSharedField(XmlNode nodeSharedField, int nGameServerID)
        {
            if (nodeSharedField.Attributes["id"] == null ||
                 nodeSharedField.Attributes["channel"] == null)
            {
                return false;
            }

            int nFieldID = int.Parse(nodeSharedField.Attributes["id"].Value);
            string strChannel = nodeSharedField.Attributes["channel"].Value;

            FieldInfo refFieldInfo = FindFIeldInfo(nFieldID);
            if (refFieldInfo == null)
                return false;

            string[] arrChannel = strChannel.Split(',');
            for (int i = 0; i < arrChannel.Length; ++i)
            {
                ChannelInfo newChannelInfo = new ChannelInfo();
                newChannelInfo.bReserved = false;
                newChannelInfo.nGameServerID = nGameServerID;
                newChannelInfo.nChannelID = int.Parse(arrChannel[i].Trim());

                refFieldInfo.listChannelInfo.Add(newChannelInfo.nChannelID, newChannelInfo);
                //refFieldInfo.listChannelInfo.AddLast(newChannelInfo);
            }

            return true;
        }

        private bool ParseReservedField(XmlNode nodeReservedField, int nGameServerID)
        {
            if (nodeReservedField.Attributes["id"] == null ||
                 nodeReservedField.Attributes["channel"] == null)
            {
                return false;
            }

            int nFieldID = int.Parse(nodeReservedField.Attributes["id"].Value);
            string strChannel = nodeReservedField.Attributes["channel"].Value;

            FieldInfo refFieldInfo = FindFIeldInfo(nFieldID);
            if (refFieldInfo == null)
                return false;

            string[] arrChannel = strChannel.Split(',');
            for (int i = 0; i < arrChannel.Length; ++i)
            {
                ChannelInfo newChannelInfo = new ChannelInfo();
                newChannelInfo.bReserved = true;
                newChannelInfo.nGameServerID = nGameServerID;
                newChannelInfo.nChannelID = int.Parse(arrChannel[i].Trim());

                refFieldInfo.listChannelInfo.Add(newChannelInfo.nChannelID, newChannelInfo);
                //refFieldInfo.listChannelInfo.AddLast(newChannelInfo);
            }

            return true;
        }



        //////////////////////////////////////////////////////////////////////////

        private FieldInfo FindFIeldInfo(int nFieldID)
        {
            LinkedListNode<FieldInfo> node = m_listFieldInfo.First;
            for (; node != null; node = node.Next)
            {
                FieldInfo refFieldInfo = node.Value;
                if (refFieldInfo.nFieldID == nFieldID)
                    return refFieldInfo;
            }
            return null;
        }
    }
}
