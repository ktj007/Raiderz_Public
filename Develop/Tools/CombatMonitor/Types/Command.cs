using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;

namespace CombatMonitor.Types
{
    [XmlRootAttribute(ElementName = "Command", IsNullable = false)]
    public class Command
    {
        public string Type;
    }
}
