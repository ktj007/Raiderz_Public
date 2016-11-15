using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace CombatMonitor.Types
{
    [XmlRootAttribute(ElementName = "Stats", IsNullable = false)]
    public class Stats
    {
        public string Type;
        public int Owner;
        public int Attacker = -1;
        public int Damage = 0;
        public string SourceType = "none";
        public int SourceID = -1;
    }
}
