using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using NUnit.Framework;
using ServerMonitor;
using SMCommon;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestConfig
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;
        private readonly string TSET_SERVER_EXE_PATH = "C:\\TestServer.exe";
        private readonly string TSET_SERVER_PROCESS_NAME = "TestServer";
        
        [Test]
        public void TestLoadConfig()
        {
            MockConfigFileController mockConfigFileLoader = new MockConfigFileController();
            mockConfigFileLoader.CreateMockXmlConfigDocument();
            mockConfigFileLoader.AddServerInfoToMockXmlConfigDocument((int)TEST_SERVER_TYPE, TSET_SERVER_EXE_PATH, TSET_SERVER_PROCESS_NAME);
            Config config = new Config(mockConfigFileLoader);

            Assert.AreEqual(true, config.LoadConfig());
            Assert.AreEqual(1, config.refListServerInfo.Count);
        }

        [Test]
        public void TestAddServerInfo()
        {
            MockConfigFileController mockConfigFileLoader = new MockConfigFileController();
            mockConfigFileLoader.CreateMockXmlConfigDocument();
            Config config = new Config(mockConfigFileLoader);

            Assert.AreEqual(false, mockConfigFileLoader.IsCallSaveConfig);
            Assert.AreEqual(0, mockConfigFileLoader.GetServerInfoCountInMockXmlConfigDocument());

            config.AddServerInfo(TEST_SERVER_TYPE, TSET_SERVER_EXE_PATH, TSET_SERVER_PROCESS_NAME);

            Assert.AreEqual(true, mockConfigFileLoader.IsCallSaveConfig);
            Assert.AreEqual(1, mockConfigFileLoader.GetServerInfoCountInMockXmlConfigDocument());
        }

        [Test]
        public void TestDeleteServerInfo()
        {
            MockConfigFileController mockConfigFileLoader = new MockConfigFileController();
            mockConfigFileLoader.CreateMockXmlConfigDocument();
            mockConfigFileLoader.AddServerInfoToMockXmlConfigDocument((int)TEST_SERVER_TYPE, TSET_SERVER_EXE_PATH, TSET_SERVER_PROCESS_NAME);
            Config config = new Config(mockConfigFileLoader);

            Assert.AreEqual(false, mockConfigFileLoader.IsCallSaveConfig);
            Assert.AreEqual(1, mockConfigFileLoader.GetServerInfoCountInMockXmlConfigDocument());

            config.DeleteServerInfo(TSET_SERVER_EXE_PATH);

            Assert.AreEqual(true, mockConfigFileLoader.IsCallSaveConfig);
            Assert.AreEqual(0, mockConfigFileLoader.GetServerInfoCountInMockXmlConfigDocument());
        }
    }

    public class MockConfigFileController : ConfigFileController
    {
        private bool m_bIsCallSaveConfig = false;
        private XmlDocument m_mockXmlConfigDoc = null;

        public bool IsCallSaveConfig { get { return m_bIsCallSaveConfig; } }

        public void CreateMockXmlConfigDocument()
        {
            m_mockXmlConfigDoc = new XmlDocument();
            m_mockXmlConfigDoc.AppendChild(m_mockXmlConfigDoc.CreateXmlDeclaration("1.0", "utf-8", "no"));
            m_mockXmlConfigDoc.AppendChild(m_mockXmlConfigDoc.CreateElement(string.Empty, ConfigConst.ELEMENT_NAME_ROOT, string.Empty));
        }

        public void AddServerInfoToMockXmlConfigDocument(int nServerType, string strExePath, string strProcessName)
        {
            XmlElement nodeServerInfo = m_mockXmlConfigDoc.CreateElement(ConfigConst.ELEMENT_NAME_SERVERINFO);
            XmlAttribute att = null;

            att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_TYPE);
            att.Value = nServerType.ToString();
            nodeServerInfo.Attributes.Append(att);

            att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_EXEPATH);
            att.Value = strExePath;
            nodeServerInfo.Attributes.Append(att);

            att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_SERVERINFO_PROCESS_NAME);
            att.Value = strProcessName;
            nodeServerInfo.Attributes.Append(att);

            XmlElement elementRoot = m_mockXmlConfigDoc.DocumentElement;
            elementRoot.AppendChild(nodeServerInfo);
        }

        public int GetServerInfoCountInMockXmlConfigDocument()
        {
            int nCount = 0;

            XmlElement elementRoot = m_mockXmlConfigDoc.DocumentElement;
            XmlNode nodeConfig = elementRoot.FirstChild;

            while (nodeConfig != null)
            {
                if (nodeConfig.Name.Equals(ConfigConst.ELEMENT_NAME_SERVERINFO) == true)
                {
                    nCount++;
                }

                nodeConfig = nodeConfig.NextSibling;
            }

            return nCount;
        }

        public override bool LoadConfigFile(string strConfigFileName, out XmlDocument xmlConfigDoc)
        {
            xmlConfigDoc = m_mockXmlConfigDoc;
            return true;
        }

        public override bool SaveConfig(string strConfigFileName, XmlDocument xmlConfig)
        {
            m_bIsCallSaveConfig = true;
            return true;
        }

    }
}
