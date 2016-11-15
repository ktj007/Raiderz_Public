using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security.Cryptography;
using System.Xml;
using cmlNET;

namespace AddCrc32ToStringTable
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (DialogResult.OK == openFileDialog1.ShowDialog())
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            button2.Enabled = false;

            AddCrc32(textBox1.Text);

            button1.Enabled = true;
            button2.Enabled = true;
        }


        private String GetCrc32(String value)
        {
            MCrc32 crc = new MCrc32();

            byte[] input = System.Text.Encoding.UTF8.GetBytes(value);            
            crc.ComputeHash(input);
            
            return String.Format("{0:x8}", crc.CrcValue);
        }


        public bool AddCrc32(String filename)
        {
            // xml 읽기            
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.PreserveWhitespace = true;
                doc.Load(filename);


                XmlNode rootNode = doc.DocumentElement;

                foreach (XmlNode node in rootNode.ChildNodes)
                {
                    if (node.NodeType != XmlNodeType.Element) continue;

                    String value = node.Attributes["string"].Value.Trim(" ".ToCharArray());

                    
                    XmlAttribute attr = doc.CreateAttribute("crc32");  
                    attr.Value = GetCrc32(value);

                    node.Attributes.InsertBefore(attr, node.Attributes[0]);
                }


                doc.Save(filename);
            }
            catch (System.Exception e)
            {
                return false;
            }

            return true;
        }
    }
}
