using System;
using System.Collections.Generic;
using System.Threading;

namespace OverlappedExecuteChecker
{
    class Checker
    {
        private static readonly string MUTEX_NAME = "UnitTestTool";
        private static Mutex m_mutex = null;

        public static bool Check()
        {
            bool bCreatedNew;
            try
            {
                m_mutex = new Mutex(true, MUTEX_NAME, out bCreatedNew);
            }
            catch
            {
                return true;
            }

            if (bCreatedNew == false) return true;

            return false;
        }
    }
}