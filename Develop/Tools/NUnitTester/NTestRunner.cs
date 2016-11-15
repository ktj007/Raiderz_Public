/*
 * @brief   Test를 수행하는 주체
 * @author  aibeast
 * @date    2009/03/20
 * @desc    실행 가능한 형태의 테스트 프로젝트에 인스턴스를 생성하여 사용합니다.
 */
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using NUnit.Util;
using NUnit.Core;
using NUnit.Core.Filters;

namespace NUnitTester
{
    public class NTestRunner
    {
        private readonly string DEFAULT_RESULT_FILE_NAME = "UnitTest_Results.xml";

        private string m_strResultXmlFileName = "";

        public NTestRunner() {}
        public NTestRunner(string strResultXmlFileName)
        {
            m_strResultXmlFileName = strResultXmlFileName;
        }

        public int Run()
        {
            if (Init() == false)
                return -1;

            if (RunTest() == false)
                return -1;

            return 0;
        }

        private bool Init()
        {
            ServiceManager.Services.AddService(new DomainManager());
            ServiceManager.Services.InitializeServices();

            if (NConfig.Init_INI() == false)
                return false;

            return true;
        }
        private bool RunTest()
        {
            TestRunner testRunner = MakeTestRunner(System.Windows.Forms.Application.ExecutablePath);

            NTestEventCollector testEventCollector = MakeTestEventCollector();
            TestFilter testFilter = MakeFilter();

            TestResult testResult = testRunner.Run(testEventCollector, testFilter);

            OutputResultXml(testResult);

            if (testResult.IsFailure) return false;
            return true;
        }
        private TestRunner MakeTestRunner(string strTestModuleName)
        {
            TestPackage package = new TestPackage(strTestModuleName);
            TestRunner testRunner = new TestDomain();
            testRunner.Load(package);
            return testRunner;
        }
        private NTestEventCollector MakeTestEventCollector()
        {
            return new NTestEventCollector();
        }
        private TestFilter MakeFilter()
        {
            // TODO : 테스트 필터링 정책에 맞는 필터 기능 구현
            return TestFilter.Empty;
        }
        private void OutputResultXml(TestResult testResult)
        {
            string xmlOutput = CreateXmlString(testResult);
            Console.WriteLine(xmlOutput);

            string strFileName;
            if (m_strResultXmlFileName.Length == 0)
                strFileName = DEFAULT_RESULT_FILE_NAME;
            else
                strFileName = m_strResultXmlFileName;

            using (StreamWriter writer = new StreamWriter(strFileName))
            {
                writer.Write(xmlOutput);
            }
        }
        private string CreateXmlString(TestResult testResult)
        {
            StringBuilder builder = new StringBuilder();
            NTestXMLReporter xmlReporter = new NTestXMLReporter(new StringWriter(builder), testResult);
            testResult.Accept(xmlReporter);
            xmlReporter.Write();

            return builder.ToString();
        }
    }
}
