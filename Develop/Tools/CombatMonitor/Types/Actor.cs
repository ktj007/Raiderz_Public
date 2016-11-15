using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace CombatMonitor.Types
{
    public class Actor
    {
        public string Name = "";
        public int UIID = -1;
        public int Level = 0;

        public virtual ArrayList ExtractReport(ref ArrayList arr)
        {
            arr.Add(new Report("< Basic >"));
            arr.Add(new Report("Name", Name));
            arr.Add(new Report("UIID", UIID.ToString()));
            arr.Add(new Report("Level", Level.ToString()));
            return arr;
        }
    }
}
