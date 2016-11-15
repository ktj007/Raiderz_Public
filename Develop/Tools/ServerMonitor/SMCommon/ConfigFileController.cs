using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using SMCommon;

namespace SMCommon
{
    public class ConfigFileController
    {
        public virtual bool IsExistFile(string strConfiFileName)
        {
            if (System.IO.File.Exists(strConfiFileName) == false)
            {
                return false;
            }
            return true;
        }

        public virtual bool LoadConfigFile(string strConfiFileName, out XmlDocument xmlConfig)
        {
            xmlConfig = new XmlDocument();

            if (System.IO.File.Exists(strConfiFileName) == false)
            {
                FileLog.Instance.Write("오류! 설정 파일({0})이 존재하지 않습니다.", strConfiFileName);
                return false;
            }
            try
            {
                System.Environment.CurrentDirectory = Application.StartupPath;
                xmlConfig.Load(strConfiFileName);
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("오류! 설정 파일({0})을 읽을 수 없습니다. 예외 메시지 : {1}", strConfiFileName, e.Message);
                return false;
            }
            return true;
        }

        public virtual bool SaveConfig(string strConfiFileName, XmlDocument xmlConfig)
        {
            try
            {
                xmlConfig.Save(strConfiFileName);
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("오류! 설정 파일 저장 실패. 예외 메시지 : {0}", e.Message);
                return false;
            }
            return true;
        }

        public bool CreateConfigFile(string strConfiFileName, string strRootElementName, out XmlDocument xmlConfig)
        {
            xmlConfig = new XmlDocument();
            xmlConfig.AppendChild(xmlConfig.CreateXmlDeclaration("1.0", "utf-8", "no"));
            xmlConfig.AppendChild(xmlConfig.CreateElement(string.Empty, strRootElementName, string.Empty));
            
            try
            {
                xmlConfig.Save(strConfiFileName);
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("오류! 설정 파일({0})을 생성할 수 없습니다. 예외 메시지 : {1}\n", strConfiFileName, e.Message);
                return false;
            }
            return true;
        }

        public bool AddAttributeToRootNode(string strConfiFileName, string strAttName, string strValue)
        {
            XmlDocument xmlConfig = new XmlDocument();

            if (System.IO.File.Exists(strConfiFileName) == false)
            {
                FileLog.Instance.Write("오류! 설정 파일({0})이 존재하지 않습니다.", strConfiFileName);
                return false;
            }
            try
            {
                System.Environment.CurrentDirectory = Application.StartupPath;
                xmlConfig.Load(strConfiFileName);

                XmlAttribute att = xmlConfig.CreateAttribute(strAttName);
                att.Value = strValue;
                xmlConfig.FirstChild.Attributes.Append(att);
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("오류! 설정 파일({0})을 읽을 수 없습니다. 예외 메시지 : {1}", strConfiFileName, e.Message);
                return false;
            }
            return true;
        }

    }
}
