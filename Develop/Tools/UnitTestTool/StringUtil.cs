using System;
using System.Collections.Generic;
using System.Text;

namespace UnitTestTool
{

    namespace StringUtil
    {
        class File
        {
            private static readonly string unittest = "unittest.ini";
            private static readonly string unittest_rel = "unittest_rel.ini";
            private static readonly string stage1_result = "UnitTestStage1_Results.xml";
            private static readonly string stage2_result = "UnitTestStage2_Results.xml";
            private static readonly string stage3_result = "UnitTestStage3_Results.xml";

            private static readonly string unitetesttool_AC = "UnitTestTool_AC.xml";

            public static string UnitTest
            {
                get { return unittest; }
            }

            public static string UnitTestRel
            {
                get { return unittest_rel; }
            }

            public static string Stage1Result
            {
                get { return stage1_result; }
            }

            public static string Stage2Result
            {
                get { return stage2_result; }
            }

            public static string Stage3Result
            {
                get { return stage3_result; }
            }

            public static string UniteTestToolAC
            {
                get { return unitetesttool_AC; }
            }
        }


        class INI
        {
            private static readonly string unittest = "UNITTEST";
            private static readonly string execute = "EXECUTE";
            private static readonly string repeat = "REPEAT";
            private static readonly string break_on_failure = "BREAK_ON_FAILURE";
            private static readonly string log_suite_name = "LOG_SUITE_NAME";

            public static string UnitTest
            {
                get { return unittest; }
            }

            public static string Execute
            {
                get { return execute; }
            }

            public static string Repeat
            {
                get { return repeat; }
            }

            public static string BreakOnFailure
            {
                get { return break_on_failure; }
            }

            public static string LogSuiteName
            {
                get { return log_suite_name; }
            }


            private static readonly string _db_test = "DB_TEST";
            private static readonly string _server = "SERVER";
            private static readonly string _database = "DATABASE";
            private static readonly string _username = "USERNAME";
            private static readonly string _password = "PASSWORD";

            public static string DBTest
            {
                get { return _db_test; }
            }

            public static string Server
            {
                get { return _server; }
            }

            public static string DataBase
            {
                get { return _database; }
            }

            public static string UserName
            {
                get { return _username; }
            }

            public static string PassWord
            {
                get { return _password; }
            }


            private static readonly string _filter = "FILTER";
            private static readonly string _stage = "STAGE";
            private static readonly string _test1 = "TEST1";
            private static readonly string _test2 = "TEST2";
            private static readonly string _test3 = "TEST3";
            private static readonly string _test4 = "TEST4";
            private static readonly string _test_suite_1 = "TEST_SUITE1";
            private static readonly string _test_suite_2 = "TEST_SUITE2";
            private static readonly string _test_suite_3 = "TEST_SUITE3";
            private static readonly string _test_suite_4 = "TEST_SUITE4";
            private static readonly string _disabled_suite_1 = "DISABLED_SUITE1";
            private static readonly string _disabled_suite_2 = "DISABLED_SUITE2";
            private static readonly string _disabled_suite_3 = "DISABLED_SUITE3";
            private static readonly string _disabled_suite_4 = "DISABLED_SUITE4";

            public static string Filter
            {
                get { return _filter; }
            }

            public static string Stage
            {
                get { return _stage; }
            }

            public static string Test1
            {
                get { return _test1; }
            }

            public static string Test2
            {
                get { return _test2; }
            }

            public static string Test3
            {
                get { return _test3; }
            }

            public static string Test4
            {
                get { return _test4; }
            }

            public static string TestSuite1
            {
                get { return _test_suite_1; }
            }

            public static string TestSuite2
            {
                get { return _test_suite_2; }
            }

            public static string TestSuite3
            {
                get { return _test_suite_3; }
            }

            public static string TestSuite4
            {
                get { return _test_suite_4; }
            }

            public static string DisabledSuite1
            {
                get { return _disabled_suite_1; }
            }

            public static string DisabledSuite2
            {
                get { return _disabled_suite_2; }
            }

            public static string DisabledSuite3
            {
                get { return _disabled_suite_3; }
            }

            public static string DisabledSuite4
            {
                get { return _disabled_suite_4; }
            }
        }

        class XML
        {
            private static readonly string autoComplete = "AutoComplete";
            private static readonly string test = "test";
            private static readonly string suite = "suite";
            private static readonly string name = "name";

            private static readonly string stage1 = "stage1";
            private static readonly string stage2 = "stage2";
            private static readonly string stage3 = "stage3";
            private static readonly string db = "DB";

            public static string AutoComplete
            {
                get { return autoComplete; }
            }

            public static string Test
            {
                get { return test; }
            }

            public static string Suite
            {
                get { return suite; }
            }

            public static string Name
            {
                get { return name; }
            }

            public static string Stage1
            {
                get { return stage1; }
            }

            public static string Stage2
            {
                get { return stage2; }
            }

            public static string Stage3
            {
                get { return stage3; }
            }

            public static string DB
            {
                get { return db; }
            }
        }
    }
}
