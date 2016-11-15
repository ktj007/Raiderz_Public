using System;
using System.Collections.Generic;
using System.Text;

namespace NUnitTester
{
    class NTestProgressInfo
    {
        private string m_strCurrentTestName = string.Empty;
        private int m_nRunCount = 0;
        private int m_nIgnoreCount = 0;
        private int m_nFailureCount = 0;
        private int m_nExceptionCount = 0;

        public string CurrentTestName
        {
            get { return m_strCurrentTestName; }
            set { m_strCurrentTestName = value; }
        }

        public int RunCount { get { return m_nRunCount; } }
        public void IncreaseRunCount() { m_nRunCount++; }

        public int IgnoreCount { get { return m_nIgnoreCount; } }
        public void IncreaseIgnoreCount() { m_nIgnoreCount++; }

        public int FailureCount { get { return m_nFailureCount; } }
        public void IncreaseFailureCount() { m_nFailureCount++; }

        public int ExceptionCount { get { return m_nExceptionCount; } }
        public void IncreaseExceptionCount() { m_nExceptionCount++; }
    }
}
