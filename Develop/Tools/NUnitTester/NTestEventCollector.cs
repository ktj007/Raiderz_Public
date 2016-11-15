/*
 * @brief   NUnit의 이벤트 리스너
 * @author  aibeast
 * @date    2009/03/20
 * @desc    NUnit에서 테스트 진행 상황에 따른 이벤트 핸들러가 호출됩니다.
 *          이곳에 XML 리포트 기능을 추가할 수 있습니다.
 */
using System;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Collections;
using System.Collections.Specialized;
using NUnit.Core;
using NUnit.Util;

namespace NUnitTester
{
	/// <summary>
	/// Summary description for EventCollector.
	/// </summary>
	class NTestEventCollector : MarshalByRefObject, EventListener
	{
        NTestProgressInfo m_testProgressInfo = new NTestProgressInfo();

		public NTestEventCollector()
		{
			AppDomain.CurrentDomain.UnhandledException += 
				new UnhandledExceptionEventHandler(OnUnhandledException);
		}

        public bool IsFailed { get { return ((m_testProgressInfo.FailureCount > 0) || (m_testProgressInfo.ExceptionCount > 0)); } }

        public void RunStarted(string name, int testCount)      {}
        public void RunFinished(TestResult result)              { ReportResult(result); }
        public void RunFinished(Exception exception)            {}

        public void TestStarted(TestName testCaseName)          { m_testProgressInfo.CurrentTestName = testCaseName.FullName; }
        public void TestFinished(TestCaseResult testCaseResult)
        {
            UpdateTestInfo(testCaseResult);
            if (testCaseResult.Executed && testCaseResult.IsFailure)
                ReportTestFailureInfo(testCaseResult);

            m_testProgressInfo.CurrentTestName = string.Empty;
        }

		public void SuiteStarted(TestName testName)	{}
		public void SuiteFinished(TestSuiteResult suiteResult) {}

        public void TestOutput(TestOutput output) {}
        
        public override object InitializeLifetimeService() { return null; }

        private void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (e.ExceptionObject.GetType() != typeof(System.Threading.ThreadAbortException))
            {
                this.UnhandledException((Exception)e.ExceptionObject);
            }
        }

		public void UnhandledException( Exception exception )
		{
            m_testProgressInfo.IncreaseExceptionCount();

            string msg = string.Format("##### Unhandled Exception while running {0}", m_testProgressInfo.CurrentTestName);
			Console.WriteLine(msg);
			Console.WriteLine(exception.ToString());
		}

        private void UpdateTestInfo(TestCaseResult testResult)
        {
            if (testResult.Executed)
            {
                m_testProgressInfo.IncreaseRunCount();

                if (testResult.IsFailure)
                    m_testProgressInfo.IncreaseFailureCount();
            }
            else
            {
                m_testProgressInfo.IncreaseIgnoreCount();
            }
        }
        private void ReportTestFailureInfo(TestCaseResult testResult)
        {
            Trace.WriteLine(string.Format("###TEST FAILURE! {0}) {1} :", m_testProgressInfo.FailureCount, testResult.Test.TestName.FullName));
            Trace.WriteLine(testResult.Message.Trim(Environment.NewLine.ToCharArray()));

            string strStackTrace = StackTraceFilter.Filter(testResult.StackTrace);
            if (strStackTrace != null && strStackTrace != string.Empty)
            {
                string[] trace = strStackTrace.Split(System.Environment.NewLine.ToCharArray());
                foreach (string s in trace)
                {
                    if (s != string.Empty)
                    {
                        string link = Regex.Replace(s.Trim(), @".* in (.*):line (.*)", "$1($2)");
                        Trace.WriteLine(string.Format("at\n{0}", link));
                    }
                }
            }
        }
        private void ReportResult(TestResult result)
        {
            Trace.WriteLine("############################################################################");

            if (m_testProgressInfo.FailureCount == 0)
            {
                Trace.WriteLine("##############                 S U C C E S S               #################");
            }
            else
            {
                Trace.WriteLine("##############                F A I L U R E S              #################");
            }

            Trace.WriteLine("############################################################################");
            Trace.WriteLine("Executed tests       : " + m_testProgressInfo.RunCount);
            Trace.WriteLine("Ignored tests        : " + m_testProgressInfo.IgnoreCount);
            Trace.WriteLine("Failed tests         : " + m_testProgressInfo.FailureCount);
            Trace.WriteLine("Unhandled exceptions : " + m_testProgressInfo.ExceptionCount);
            Trace.WriteLine("Total time           : " + result.Time + " seconds");
            Trace.WriteLine("############################################################################");
        }

	}
}
