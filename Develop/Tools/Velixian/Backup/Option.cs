using System;
using SoulHunt;

namespace Velixian
{
    static class Option
    {
        static private bool _AutoMonitorFile = false;

        static public bool AutoMonitorFile
        {
            get { return _AutoMonitorFile; }
            set { _AutoMonitorFile = value; }
        }
    }
}