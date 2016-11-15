using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.ComponentModel;


namespace UnitTestTool
{
    class UnitTestToolData
    {
        private bool _unittest_execute;
        private int _unittest_repeat;
        private bool _unittest_break_on_failure;
        private bool _unittest_log_suite_name;

        private string _dbtest_server;
        private string _dbtest_database;
        private string _dbtest_username;
        private string _dbtest_password;

        private string _filter_stage;
        private string _filter_test1;
        private string _filter_test2;
        private string _filter_test3;
        private string _filter_test4;
        private string _filter_test_suite1;
        private string _filter_test_suite2;
        private string _filter_test_suite3;
        private string _filter_test_suite4;
        private string _filter_disabled_suite1;
        private string _filter_disabled_suite2;
        private string _filter_disabled_suite3;
        private string _filter_disabled_suite4;

        public UnitTestToolData()        
        {
            Reset();
        }

        public void Reset()
        {
            _unittest_execute = false;
            _unittest_repeat = 0;
            _unittest_break_on_failure = false;
            _unittest_log_suite_name = false;

            _dbtest_server = "SH_DB";
            _dbtest_database = "UnitTestDB";
            _dbtest_username = "dbtest";
            _dbtest_password = "dbtest";

            _filter_stage = "all";
            _filter_test1 = "";
            _filter_test2 = "";
            _filter_test3 = "";
            _filter_test4 = "";
            _filter_test_suite1 = "";
            _filter_test_suite2 = "";
            _filter_test_suite3 = "";
            _filter_test_suite4 = "";
            _filter_disabled_suite1 = "";
            _filter_disabled_suite2 = "";
            _filter_disabled_suite3 = "";
            _filter_disabled_suite4 = "";
        }

        [CategoryAttribute("UNITTEST")]
        public bool Execute
        {
            get { return _unittest_execute; }
            set { _unittest_execute = value; }
        }

        [CategoryAttribute("UNITTEST")]        
        public int Repeat
        {
            get { return _unittest_repeat;  }
            set { _unittest_repeat = value; }
        }

        [CategoryAttribute("UNITTEST")]
        public bool BreakOnFailure
        {
            get { return _unittest_break_on_failure; }
            set { _unittest_break_on_failure = value; }
        }

        [CategoryAttribute("UNITTEST")]
        public bool LogSuiteName
        {
            get { return _unittest_log_suite_name; }
            set { _unittest_log_suite_name = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string Server
        {
            get { return _dbtest_server; }
            set { _dbtest_server = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string DataBase
        {
            get { return _dbtest_database; }
            set { _dbtest_database = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string UserName
        {
            get { return _dbtest_username; }
            set { _dbtest_username = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string Password
        {
            get { return _dbtest_password; }
            set { _dbtest_password = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Stage
        {
            get { return _filter_stage; }
            set { _filter_stage = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test1
        {
            get { return _filter_test1; }
            set { _filter_test1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test2
        {
            get { return _filter_test2; }
            set { _filter_test2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test3
        {
            get { return _filter_test3; }
            set { _filter_test3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test4
        {
            get { return _filter_test4; }
            set { _filter_test4 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite1
        {
            get { return _filter_test_suite1; }
            set { _filter_test_suite1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite2
        {
            get { return _filter_test_suite2; }
            set { _filter_test_suite2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite3
        {
            get { return _filter_test_suite3; }
            set { _filter_test_suite3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite4
        {
            get { return _filter_test_suite4; }
            set { _filter_test_suite4 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite1
        {
            get { return _filter_disabled_suite1; }
            set { _filter_disabled_suite1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite2
        {
            get { return _filter_disabled_suite2; }
            set { _filter_disabled_suite2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite3
        {
            get { return _filter_disabled_suite3; }
            set { _filter_disabled_suite3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite4
        {
            get { return _filter_disabled_suite4; }
            set { _filter_disabled_suite4 = value; }
        }
    }
}
