using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace Config
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            ConfigData configData = new ConfigData();
            this.propertyGrid1.SelectedObject = configData;

            String filename = "config.xml";
            if (!System.IO.File.Exists(filename)) filename = "config_rel.xml";
            if (System.IO.File.Exists(filename)) _LoadFromFile(filename);
        }

        private void _SaveToFile(string strFileName)
        {
            ConfigData configData = (ConfigData)this.propertyGrid1.SelectedObject;

            try
            {
                Encoding encoding = Encoding.ASCII;

                XmlTextWriter tw = new XmlTextWriter(strFileName, encoding);
                tw.Formatting = Formatting.Indented;
                tw.WriteStartDocument();
                tw.WriteStartElement("Config");

                // Video ---
                tw.WriteStartElement("Video");
                tw.WriteAttributeString("width", Convert.ToString(configData.Width));
                tw.WriteAttributeString("height", Convert.ToString(configData.Height));
                tw.WriteAttributeString("fullscreen", Convert.ToString(configData.FullScreen));
                tw.WriteAttributeString("colorbits", Convert.ToString(configData.ColorBits));
                tw.WriteAttributeString("depthbits", Convert.ToString(configData.DepthBits));
                tw.WriteAttributeString("shader", Convert.ToString(configData.Shader));
                tw.WriteEndElement();		// Video

                // Audio ---
                tw.WriteStartElement("Audio");
                tw.WriteAttributeString("mute", Convert.ToString(configData.Mute));
                tw.WriteAttributeString("bgm_volume", Convert.ToString(configData.BgmVolume));
                tw.WriteAttributeString("effect_volume", Convert.ToString(configData.EffectVolume));
                tw.WriteEndElement();		// Video

                // Network ---
                tw.WriteStartElement("Network");
                tw.WriteAttributeString("port", Convert.ToString(configData.Port));
                tw.WriteEndElement();		// Video


                tw.WriteEndElement(); // Config
                tw.Flush();
                tw.Close();
            }
            catch (Exception e)
            {
                MessageBox.Show("저장하지 못했습니다.");
            }
        }

        private void _LoadFromFile(string strFileName)
        {
            ConfigData configData = (ConfigData)this.propertyGrid1.SelectedObject;

            try
            {
                string sKey;
                XmlTextReader xtr = new XmlTextReader(strFileName);
                while (xtr.Read())
                {
                    if(xtr.NodeType == XmlNodeType.Element)
                    {
                        if (xtr.LocalName.ToLower() == "video")
                        {
                            for (int i = 0; i < xtr.AttributeCount; i++)
                            {
                                xtr.MoveToAttribute(i);

                                string key;
                                string value;

                                if (xtr.Name.ToLower() == "width")
                                {
                                    value = xtr.Value;
                                    configData.Width = Convert.ToInt32(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "height")
                                {
                                    value = xtr.Value;
                                    configData.Height = Convert.ToInt32(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "colorbits")
                                {
                                    value = xtr.Value;
                                    configData.ColorBits = Convert.ToInt32(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "fullscreen")
                                {
                                    value = xtr.Value;
                                    configData.FullScreen = Convert.ToBoolean(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "depthbits")
                                {
                                    value = xtr.Value;
                                    configData.DepthBits = Convert.ToInt32(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "shader")
                                {
                                    value = xtr.Value;
                                    configData.Shader = Convert.ToBoolean(xtr.Value);
                                }
                            }
                        }

                        // audio
                        if (xtr.LocalName.ToLower() == "audio")
                        {
                            for (int i = 0; i < xtr.AttributeCount; i++)
                            {
                                xtr.MoveToAttribute(i);

                                string key;
                                string value;

                                if (xtr.Name.ToLower() == "bgm_volume")
                                {
                                    value = xtr.Value;
                                    configData.BgmVolume = (float)Convert.ToDouble(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "effect_volume")
                                {
                                    value = xtr.Value;
                                    configData.EffectVolume = (float)Convert.ToDouble(xtr.Value);
                                }
                                else if (xtr.Name.ToLower() == "mute")
                                {
                                    value = xtr.Value;
                                    configData.Mute = Convert.ToBoolean(xtr.Value);
                                }
                            }
                        }

                        // network
                        if (xtr.LocalName.ToLower() == "network")
                        {
                            for (int i = 0; i < xtr.AttributeCount; i++)
                            {
                                xtr.MoveToAttribute(i);

                                string key;
                                string value;

                                if (xtr.Name.ToLower() == "port")
                                {
                                    value = xtr.Value;
                                    configData.Port = Convert.ToInt32(xtr.Value);
                                }
                            }
                        }

                        xtr.Read();

                    }
                }
                xtr.Close();
            }
            catch (Exception e)
            {

            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            _SaveToFile("config.xml");
            this.Close();
        }

        private void btnInit_Click(object sender, EventArgs e)
        {
            _LoadFromFile("config_rel.xml");
            propertyGrid1.Refresh();
        }
    }
}