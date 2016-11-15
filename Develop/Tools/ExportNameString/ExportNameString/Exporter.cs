using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Security.Cryptography;

namespace ExportNameString
{
    class Exporter
    {
        static public bool Export(string file, string path)
        {
            XmlDocument doc = new XmlDocument();            
            try
            {
                doc.Load(file);
            }
            catch (System.Exception)
            {
                return false;
            }


            XmlNode rootNode = doc.DocumentElement;            

            if (rootNode.Name == "maiet")
            {
                string strFileName = Path.GetFileNameWithoutExtension(file);
                strFileName = path + "\\name_" + strFileName + ".xml";

                XmlTextWriter writer = new XmlTextWriter(strFileName, Encoding.UTF8);

                writer.Formatting = Formatting.Indented;
                writer.Indentation = 4;

                writer.WriteStartDocument();
                writer.WriteStartElement("maiet");

                

                XmlNodeList nodeList = doc.SelectNodes(".//FIELD");

                if (0 == nodeList.Count)
                {
                    nodeList = rootNode.ChildNodes;                
                }


                foreach (XmlNode node in nodeList)
                {
                    if (node.NodeType != XmlNodeType.Element) continue;
                    
                    // name
                    if (node.Name != "FACTION")
                    {
                        writer.WriteStartElement("STR");
                        writer.WriteAttributeString("crc32", GetCrc32(GetAttributeValue(node, "name")));
                        writer.WriteAttributeString("key", node.Name + "_NAME_" + GetAttributeValue(node, "id") + GetMode(node));
                        writer.WriteAttributeString("string", GetAttributeValue(node, "name"));
                        writer.WriteEndElement();
                    }

                    // title
                    if (node.Name == "FACTION")
                    {
                        writer.WriteStartElement("STR");
                        writer.WriteAttributeString("crc32", GetCrc32(GetAttributeValue(node, "title")));
                        writer.WriteAttributeString("key", node.Name + "_TITLE_" + GetAttributeValue(node, "id") + GetMode(node));
                        writer.WriteAttributeString("string", GetAttributeValue(node, "title"));
                        writer.WriteEndElement();
                    }

                    // desc
                    if (node.Name != "NPC" && node.Name != "FIELD")
                    {
                        writer.WriteStartElement("STR");
                        writer.WriteAttributeString("crc32", GetCrc32(GetAttributeValue(node, "desc")));
                        writer.WriteAttributeString("key", node.Name + "_DESC_" + GetAttributeValue(node, "id") + GetMode(node));
                        writer.WriteAttributeString("string", GetAttributeValue(node, "desc"));
                        writer.WriteEndElement();
                    }

                    // Clan
                    if (node.Name == "NPC")
                    {
                        writer.WriteStartElement("STR");
                        writer.WriteAttributeString("crc32", GetCrc32(GetAttributeValue(node, "Clan")));
                        writer.WriteAttributeString("key", node.Name + "_CLAN_" + GetAttributeValue(node, "id") + GetMode(node));
                        writer.WriteAttributeString("string", GetAttributeValue(node, "Clan"));
                        writer.WriteEndElement();
                    }
                }

                writer.WriteEndElement();
                writer.WriteEndDocument();

                writer.Close();
            }

            return true;
        }

        static private string GetAttributeValue(XmlNode node, string attributeName)
        {
            try
            {
                // 어트리뷰트 이름을 대소문자 구별없이 접근합니다.
                String value = ((XmlAttribute)node.SelectSingleNode("@*[translate(name(),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')=translate('" + attributeName + "','abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')]")).Value;
                return value.Trim(" ".ToCharArray());
            }
            catch (System.Exception)
            {
                return "";
            }
        }

        static private String GetCrc32(String value)
        {
            Crc32 crc = new Crc32();

            byte[] input = System.Text.Encoding.UTF8.GetBytes(value);
            crc.ComputeHash(input);

            return String.Format("{0:x8}", crc.CrcValue);
        }

        static private String GetMode(XmlNode node)
        {
            if (node.Name == "TALENT")
            {
                String strMode = GetAttributeValue(node, "Mode");

                if (strMode != "" && strMode != "none")
                {
                    return "_" + strMode;
                }
            }

            if (node.Name == "NPC")
            {
                String strMode = GetAttributeValue(node, "Mode");

                if (strMode != "" && strMode != "0")
                {
                    return "_" + strMode;
                }
            }

            return "";
        }
    }
}
