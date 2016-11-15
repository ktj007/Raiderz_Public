using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;

namespace ServerMonitor_unittest.Mock
{
    class MockLogStringMgr : LogStringMgr
    {
        public static readonly ushort TEST_STRING_ID1 = 1;
        public static readonly ushort TEST_STRING_ID2 = 2;
        public static readonly ushort TEST_STRING_ID3 = 3;

        public static readonly string TEST_STRING1 = "TestString1";
        public static readonly string TEST_STRING2 = "TestString2";
        public static readonly string TEST_STRING3 = "TestString3";

        public override bool LoadStringTable()
        {
            LOGSTRING logString;
            logString = new LOGSTRING();
            logString.strString = TEST_STRING1;
            m_dicStringTable.Add(TEST_STRING_ID1, logString);

            logString = new LOGSTRING();
            logString.strString = TEST_STRING2;
            m_dicStringTable.Add(TEST_STRING_ID2, logString);

            logString = new LOGSTRING();
            logString.strString = TEST_STRING3;
            m_dicStringTable.Add(TEST_STRING_ID3, logString);
            return true;
        }

    }
}
