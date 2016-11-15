using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnitTester;

namespace ServerMonitorClient_unittest
{
    class main
    {
        static int Main(string[] args)
        {
            NTestRunner cTestRunner = new NTestRunner("ServerMonitorClient_unittest_result.xml");
            int nRet = cTestRunner.Run();
            return nRet;
        }
    }
}
