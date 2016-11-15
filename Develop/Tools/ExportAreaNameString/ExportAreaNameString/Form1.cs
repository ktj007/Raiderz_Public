using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace ExportAreaNameString
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (DialogResult.OK == folderBrowserDialog1.ShowDialog())
            {
                textBox1.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Export(textBox1.Text);
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

        public bool Export(string path)
        {
            try
            {
                XmlDocument newDoc = new XmlDocument();
                newDoc.AppendChild(newDoc.CreateXmlDeclaration("1.0", "utf-8", null));
                newDoc.AppendChild(newDoc.CreateElement("maiet"));

                XmlNode descRoot = newDoc.DocumentElement;


                String pathSrc = path;
                DirectoryInfo dirSrc = new DirectoryInfo(pathSrc);
                FileInfo[] fieldFiles = dirSrc.GetFiles("*.field.xml");

                foreach (FileInfo file in fieldFiles)
                {
                    XmlDocument fieldDoc = new XmlDocument();
                    fieldDoc.Load(file.FullName);


                    String fileID = Path.GetFileNameWithoutExtension(Path.GetFileNameWithoutExtension(file.Name));

                    XmlNode fieldRoot = fieldDoc.DocumentElement.SelectSingleNode("FIELDINFO");

                    foreach (XmlNode node in fieldRoot.ChildNodes)
                    {
                        if (node.NodeType != XmlNodeType.Element) continue;
                        if (node.Name != "AREA") continue;
                        if ("" == GetAttributeValue(node, "id")) continue;


                        XmlNode descNode = newDoc.CreateElement("STR");

                        XmlAttribute attrKey = newDoc.CreateAttribute("key");
                        attrKey.Value = "AREA_NAME_" + fileID + "_" + GetAttributeValue(node, "id");

                        XmlAttribute attrString = newDoc.CreateAttribute("string");
                        attrString.Value = GetAttributeValue(node, "name");


                        descNode.Attributes.Append(attrKey);
                        descNode.Attributes.Append(attrString);

                        descRoot.AppendChild(descNode);
                    }
                }


                String pathDesc = path + "\\..\\system\\lang\\ko_KR\\name_area.xml";
                newDoc.Save(pathDesc);
            }
            catch (System.Exception ex)
            {
                return false;
            }

            return true;
        }
    }
}
