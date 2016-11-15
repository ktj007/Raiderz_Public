using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace CombatMonitor.Types
{
    public class Report
    {
        public Report()
        {
        }

        public Report(string name)
        {
            Name = name;
        }

        public Report(string name, string param)
        {
            Name = name;
            Param1 = param;
        }

        public Report(string name, string param, string param2)
        {
            Name = name;
            Param1 = param;
            Param2 = param2;
        }
        public string Name = "";
        public string Param1 = "";
        public string Param2 = "";
    }
}
