using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using NUnitTester;
using ServerMonitor;
using SMCommon;
using SMCommon.Mock;

namespace ServerMonitor_unittest
{
    public class UnitTest
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        public static int Main(string[] args)
        {
            FileLog.Instance.ChangeInstance(new MockLogger());

            NTestRunner cTestRunner = new NTestRunner("ServerMonitor_unittest_result.xml");
            int nRet = cTestRunner.Run();
            return nRet;
        }
    }
}
