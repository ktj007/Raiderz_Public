
using System;
using System.Collections;
using System.ComponentModel;
using System.Xml;
using System.IO;
using System.Windows.Forms;


namespace Configuration
{
    public static class Config
    {
        public static Hashtable Options = new Hashtable();


        private static string GetFileName()
        {
            string fileName = Application.ProductName + ".config.xml";
            return fileName;
        }

        public static bool LoadConfig()
        {
            string fileName = GetFileName();
            if (File.Exists(fileName) == false)
                return false;

            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.Load(fileName);

            XmlElement elementRoot = xmlDocument.DocumentElement;
            XmlNodeList nodeLists = elementRoot.ChildNodes;
            for (int i = 0; i < nodeLists.Count; i++)
            {
                XmlNode nodeList = nodeLists[i];
                if (nodeList.Name != "CONFIG")
                    continue;

                string name = "";
                string value = "";
                foreach (XmlAttribute attr in nodeList.Attributes)
                {
                    if (attr.Name == "name") name = attr.Value;
                    else if (attr.Name == "value") value = attr.Value;
                }

                Options.Add( name, value);
            }

            return true;
        }
        public static bool SaveConfig()
        {
            XmlDocument xmlDocument = new XmlDocument();
            XmlNode docDeclaration = xmlDocument.CreateXmlDeclaration("1.0", "UTF-8", null);
            xmlDocument.AppendChild(docDeclaration);

            XmlElement elementRoot = xmlDocument.CreateElement("DATA");
            xmlDocument.AppendChild(elementRoot);

            foreach (DictionaryEntry dic in Options)
            {
                XmlElement element = xmlDocument.CreateElement("CONFIG");
                element.SetAttribute("name", dic.Key.ToString());
                element.SetAttribute("value", dic.Value.ToString());
                elementRoot.AppendChild(element);
            }

            string fileName = GetFileName();
            xmlDocument.Save(fileName);

            return true;
        }

        public static string GetOption(string key)
        {
            if (Options.ContainsKey(key) == false)
                return "";

            return Options[key].ToString();
        }
        public static string GetOption(string key, string defaultval)
        {
            if (Options.ContainsKey(key) == false)
                return defaultval;

            return Options[key].ToString();
        }
        public static void SetOption(string key, string value)
        {
            if (Options.ContainsKey(key) == false)
                Options.Add(key, value);
            else
                Options[key] = value;
        }
    }
}