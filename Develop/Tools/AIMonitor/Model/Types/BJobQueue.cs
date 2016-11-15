using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.ComponentModel;

namespace ColtViewer.Model.Types
{
    [XmlRootAttribute(ElementName = "BJobQueue", IsNullable = false)]
    public class BJobQueue
    {
        public struct Item
        {
            [XmlAttribute("JobName")]
            public string JobName;
            [XmlAttribute("TaskName")]
            public string TaskName;
            [XmlAttribute("Param")]
            public string Param;
        }

        public BJobQueue()
        {
        }

        [XmlElement("Item")]
        public Item[] arrayItem;
    }
}
