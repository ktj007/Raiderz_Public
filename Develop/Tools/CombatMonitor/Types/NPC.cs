using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.Collections;

namespace CombatMonitor.Types
{
    [XmlRootAttribute(ElementName = "NPC", IsNullable = false)]
    public class NPC : Actor
    {
        public int ID = 0;

        public override ArrayList ExtractReport(ref ArrayList arr)
        {
            arr = base.ExtractReport(ref arr);
            arr.Add(new Report("< NPC 속성값 >"));
            arr.Add(new Report("ID", ID.ToString()));
            return arr;
        }
    }
}
