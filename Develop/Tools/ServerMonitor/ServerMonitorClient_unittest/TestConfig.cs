using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using NUnit.Framework;
using ServerMonitorClient;
using SMCommon;

namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestConfig
    {
        private readonly int TSET_AUTO_MODE_LOCAL_PORT = 7500;

        private readonly string TEST_MACHINE_NAME = "TestMachine";
        private readonly string TSET_MACHINE_IP = "127.0.0.1";
        private readonly int TSET_MACHINE_PORT = 7500;

        [Test]
        public void TestLoadConfig()
        {
            MockConfigFileController mockConfigFileLoader = new MockConfigFileController();
            mockConfigFileLoader.CreateMockXmlConfigDocument(TSET_AUTO_MODE_LOCAL_PORT);
            mockConfigFileLoader.AddMachineInfo(TEST_MACHINE_NAME, TSET_MACHINE_IP, TSET_MACHINE_PORT);
            Config config = new Config(mockConfigFileLoader);

            MachineInfo[] arrMachineInfo;
            Assert.AreEqual(true, config.LoadConfig(out arrMachineInfo));
            Assert.AreEqual(1, arrMachineInfo.Length);
        }

        [Test]
        public void TestLoadConfig_Failure()
        {
            InvalidMockConfigFileController invalidMockConfigFileLoader = new InvalidMockConfigFileController();
            invalidMockConfigFileLoader.CreateMockXmlConfigDocument();
            invalidMockConfigFileLoader.AddMachineInfo(TEST_MACHINE_NAME);
            Config config = new Config(invalidMockConfigFileLoader);

            MachineInfo[] arrMachineInfo;
            Assert.AreEqual(false, config.LoadConfig(out arrMachineInfo));
            Assert.AreEqual(null, arrMachineInfo);
        }

        [Test]
        public void TestGetAutoModeLocalPort()
        {
            MockConfigFileController mockConfigFileLoader = new MockConfigFileController();
            mockConfigFileLoader.CreateMockXmlConfigDocument(TSET_AUTO_MODE_LOCAL_PORT);
            Config config = new Config(mockConfigFileLoader);

            Assert.AreEqual(TSET_AUTO_MODE_LOCAL_PORT, config.GetAutoModeLocalPort());
        }

        [Test]
        public void TestGetAutoModeLocalPort_Failure()
        {
            InvalidMockConfigFileController invalidMockConfigFileLoader = new InvalidMockConfigFileController();
            invalidMockConfigFileLoader.CreateMockXmlConfigDocument();
            Config config = new Config(invalidMockConfigFileLoader);

            Assert.AreEqual(0, config.GetAutoModeLocalPort());
        }


        public class MockConfigFileController : ConfigFileController
        {
            private bool m_bIsCallSaveConfig = false;
            private XmlDocument m_mockXmlConfigDoc = null;

            public bool IsCallSaveConfig { get { return m_bIsCallSaveConfig; } }

            public void CreateMockXmlConfigDocument(int nAutoModeLocalPort)
            {
                m_mockXmlConfigDoc = new XmlDocument();
                m_mockXmlConfigDoc.AppendChild(m_mockXmlConfigDoc.CreateXmlDeclaration("1.0", "utf-8", "no"));
                m_mockXmlConfigDoc.AppendChild(m_mockXmlConfigDoc.CreateElement(string.Empty, ConfigConst.ELEMENT_NAME_ROOT, string.Empty));

                XmlAttribute att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_ROOT_AUTOMODE_LOCAL_PORT);
                att.Value = nAutoModeLocalPort.ToString();
                m_mockXmlConfigDoc.DocumentElement.Attributes.Append(att);
            }

            public void AddMachineInfo(string strMachineName, string strIP, int nPort)
            {
                XmlElement nodeServerInfo = m_mockXmlConfigDoc.CreateElement(ConfigConst.ELEMENT_NAME_MACHINE);
                XmlAttribute att = null;

                att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_MACHINE_NAME);
                att.Value = strMachineName;
                nodeServerInfo.Attributes.Append(att);

                att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_MACHINE_IP);
                att.Value = strIP;
                nodeServerInfo.Attributes.Append(att);

                att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_MACHINE_PORT);
                att.Value = nPort.ToString();
                nodeServerInfo.Attributes.Append(att);

                XmlElement elementRoot = m_mockXmlConfigDoc.DocumentElement;
                elementRoot.AppendChild(nodeServerInfo);
            }

            public override bool IsExistFile(string strConfiFileName)
            {
                return true;
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

        public class InvalidMockConfigFileController : ConfigFileController
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

            public void AddMachineInfo(string strMachineName)
            {
                XmlElement nodeServerInfo = m_mockXmlConfigDoc.CreateElement(ConfigConst.ELEMENT_NAME_MACHINE);
                XmlAttribute att = null;

                att = m_mockXmlConfigDoc.CreateAttribute(ConfigConst.ATT_NAME_MACHINE_NAME);
                att.Value = strMachineName;
                nodeServerInfo.Attributes.Append(att);

                XmlElement elementRoot = m_mockXmlConfigDoc.DocumentElement;
                elementRoot.AppendChild(nodeServerInfo);
            }

            public override bool IsExistFile(string strConfiFileName)
            {
                return true;
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
}
