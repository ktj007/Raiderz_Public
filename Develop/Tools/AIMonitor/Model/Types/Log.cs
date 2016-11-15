using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.ComponentModel;

namespace AIMonitor.Model.Types
{
    public class Log
    {
        [XmlRootAttribute(ElementName = "Log", IsNullable = false)]
        public struct Item
        {
            [XmlAttribute("Type")]
            public string Type;
            [XmlAttribute("Text")]
            public string Text;
            public string Date;
        }

        public List<Item> Items { get; set; }
    }
}
