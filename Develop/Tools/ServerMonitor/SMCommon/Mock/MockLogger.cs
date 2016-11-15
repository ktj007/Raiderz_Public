using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SMCommon.Mock
{
    public class MockLogger : FileLog
    {
        public override void Write(string strFormat, params object[] args) {}
    }
}
