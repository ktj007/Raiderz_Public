using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Text.RegularExpressions;

namespace UpdateLang
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;

            UpdateLanguage(textBox1.Text);

            button1.Enabled = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (DialogResult.OK == openFileDialog1.ShowDialog())
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        public bool UpdateLanguage(String filename)
        {
            String sourcePath = Path.GetDirectoryName(filename);
            String targetPath = Path.GetDirectoryName(sourcePath) + "\\en_US";
            String fileName = Path.GetFileName(filename);


            String sourceFilename = sourcePath + "\\" + fileName;
            String targetFilename = targetPath + "\\" + fileName;
            String reportFilename = "UpdateLangReport_" + fileName;  // .xml


            XmlDocument sourceDoc = new XmlDocument();
            sourceDoc.PreserveWhitespace = true;

            XmlDocument targetDoc = new XmlDocument();
            targetDoc.PreserveWhitespace = true;
            
            XmlDocument newDoc = new XmlDocument();
            newDoc.PreserveWhitespace = true;                                    

            XmlDocument report = new XmlDocument();


            sourceDoc.Load(sourceFilename);
            targetDoc.Load(targetFilename);

            XmlNode sourceRoot = sourceDoc.DocumentElement;
            XmlNode targetRoot = targetDoc.DocumentElement;

            newDoc.AppendChild(newDoc.CreateXmlDeclaration("1.0", "utf-8", null));
            newDoc.AppendChild(newDoc.CreateWhitespace("\r\n"));
            newDoc.AppendChild(newDoc.CreateElement("maiet"));
            
            XmlNode newRoot = newDoc.DocumentElement;            

            report.AppendChild(report.CreateXmlDeclaration("1.0", "utf-8", null));
            report.AppendChild(report.CreateElement("maiet"));
            XmlNode reportRoot = report.DocumentElement;

            XmlNode reportAddNode = report.CreateElement("add");
            XmlNode reportChangeNode = report.CreateElement("change");
            XmlNode reportDebugNode = report.CreateElement("debug");
            XmlNode reportUpdateLangNode = report.CreateElement("updatelang");

            reportRoot.AppendChild(reportUpdateLangNode);
            reportUpdateLangNode.AppendChild(reportAddNode);
            reportUpdateLangNode.AppendChild(reportChangeNode);
            reportUpdateLangNode.AppendChild(reportDebugNode);


            foreach (XmlNode node in sourceRoot.ChildNodes)
            {
                if (node.NodeType != XmlNodeType.Element)
                {
                    newRoot.AppendChild(newDoc.ImportNode(node, false));
                    continue;
                }

                String value = GetAttributeValue(node, "string");

                // 개발용표시
                if (0 == value.IndexOf("#") ||
                    1 == value.IndexOf("#") ||
                        value.Contains("}#"))
                {
                    newRoot.AppendChild(newDoc.ImportNode(node, false));
                    reportDebugNode.AppendChild(report.ImportNode(node, false));
                    continue;
                }

                String xpath = "//maiet/*[@key='" + GetAttributeValue(node, "key") + "']";
                XmlNode findNode = targetRoot.SelectSingleNode(xpath);

                if (findNode == null)
                {
                    // 새 문자열 추가
                    newRoot.AppendChild(newDoc.ImportNode(node, false));
                    reportAddNode.AppendChild(report.ImportNode(node, false));
                }
                else
                {
                    try
                    {
                        if (GetAttributeValue(findNode, "crc32") == GetAttributeValue(node, "crc32"))
                        {
                            // 변경사항 없음
                            newRoot.AppendChild(newDoc.ImportNode(findNode, false));

                            // 미번역 문자열의 경우 새 문자열 보고서에 추가
                            if (value != "" &&
                                GetAttributeValue(findNode, "string") == value &&
                                Regex.IsMatch(value, @"[가-힣]+"))
                            {
                                reportAddNode.AppendChild(report.ImportNode(node, false));                             
                            }
                        }
                        else if ("" == GetAttributeValue(findNode, "string"))
                        {
                            // 새 문자열 추가
                            newRoot.AppendChild(newDoc.ImportNode(node, false));
                            reportAddNode.AppendChild(report.ImportNode(node, false));
                        }
                        else
                        {
                            if (false)                          
                            {
                                // interface.xml에 CRC 없을때 처리용
                                XmlNode newNode = newDoc.ImportNode(findNode, false);
                                XmlNode newAttr = newDoc.ImportNode(node.Attributes["crc32"], false);

                                newNode.Attributes.InsertBefore((XmlAttribute)newAttr, newNode.Attributes[0]);
                                newRoot.AppendChild(newNode);
                            }
                            else
                            {
                                if (true)
                                {
                                    // 문자열이 변경되었습니다 - 적용하진 않습니다.
                                    newRoot.AppendChild(newDoc.ImportNode(findNode, false));
                                }
                                else
                                {
                                    // 문자열 변경 덥어씀
                                    newRoot.AppendChild(newDoc.ImportNode(node, false));
                                }
                            }                            

                            reportChangeNode.AppendChild(report.ImportNode(node, false));
                        }
                    }
                    catch (System.Exception)
                    {
                        // crc32 속성이 없는 경우 - 초기작업을 위해 준비했습니다 - 변경사항없음 취급
                        XmlNode newNode = newDoc.ImportNode(findNode, false);
                        XmlNode newAttr = newDoc.ImportNode(node.Attributes["crc32"], false);

                        newNode.Attributes.InsertBefore((XmlAttribute)newAttr, newNode.Attributes[0]);
                        newRoot.AppendChild(newNode);

                        // 미번역 문자열의 경우 새 문자열 보고서에 추가
                        if (value != "" && GetAttributeValue(findNode, "string") == value)
                        {
                            reportAddNode.AppendChild(report.ImportNode(newNode, false));
                        }
                    }
                }
            }

            newDoc.Save(targetFilename);
            report.Save(reportFilename);

            return true;
        }

        private string GetAttributeValue(XmlNode node, string attributeName)
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
    }
}
