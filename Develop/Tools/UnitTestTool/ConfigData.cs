using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.ComponentModel;


namespace UnitTestTool
{
    class ConfigData
    {
        private bool unittest_execute;
        private int unittest_repeat;
        private bool unittest_break_on_failure;
        private bool unittest_log_suite_name;

        private string dbtest_server;
        private string dbtest_database;
        private string dbtest_username;
        private string dbtest_password;

        private string filter_stage;
        private string filter_test1;
        private string filter_test2;
        private string filter_test3;
        private string filter_test4;
        private string filter_test_suite1;
        private string filter_test_suite2;
        private string filter_test_suite3;
        private string filter_test_suite4;
        private string filter_disabled_suite1;
        private string filter_disabled_suite2;
        private string filter_disabled_suite3;
        private string filter_disabled_suite4;

        public ConfigData()        
        {
            Reset();
        }

        public void Reset()
        {
            unittest_execute = false;
            unittest_repeat = 0;
            unittest_break_on_failure = false;
            unittest_log_suite_name = false;

            dbtest_server = "SH_DB";
            dbtest_database = "UnitTestDB";
            dbtest_username = "dbtest";
            dbtest_password = "dbtest";

            filter_stage = "all";
            filter_test1 = "";
            filter_test2 = "";
            filter_test3 = "";
            filter_test4 = "";
            filter_test_suite1 = "";
            filter_test_suite2 = "";
            filter_test_suite3 = "";
            filter_test_suite4 = "";
            filter_disabled_suite1 = "";
            filter_disabled_suite2 = "";
            filter_disabled_suite3 = "";
            filter_disabled_suite4 = "";
        }

        [CategoryAttribute("UNITTEST")]
        public bool Execute
        {
            get { return unittest_execute; }
            set { unittest_execute = value; }
        }

        [CategoryAttribute("UNITTEST")]        
        public int Repeat
        {
            get { return unittest_repeat;  }
            set { unittest_repeat = value; }
        }

        [CategoryAttribute("UNITTEST")]
        public bool BreakOnFailure
        {
            get { return unittest_break_on_failure; }
            set { unittest_break_on_failure = value; }
        }

        [CategoryAttribute("UNITTEST")]
        public bool LogSuiteName
        {
            get { return unittest_log_suite_name; }
            set { unittest_log_suite_name = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string Server
        {
            get { return dbtest_server; }
            set { dbtest_server = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string DataBase
        {
            get { return dbtest_database; }
            set { dbtest_database = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string UserName
        {
            get { return dbtest_username; }
            set { dbtest_username = value; }
        }

        [CategoryAttribute("DB_TEST")]
        public string Password
        {
            get { return dbtest_password; }
            set { dbtest_password = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Stage
        {
            get { return filter_stage; }
            set { filter_stage = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test1
        {
            get { return filter_test1; }
            set { filter_test1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test2
        {
            get { return filter_test2; }
            set { filter_test2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test3
        {
            get { return filter_test3; }
            set { filter_test3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string Test4
        {
            get { return filter_test4; }
            set { filter_test4 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite1
        {
            get { return filter_test_suite1; }
            set { filter_test_suite1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite2
        {
            get { return filter_test_suite2; }
            set { filter_test_suite2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite3
        {
            get { return filter_test_suite3; }
            set { filter_test_suite3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string TestSuite4
        {
            get { return filter_test_suite4; }
            set { filter_test_suite4 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite1
        {
            get { return filter_disabled_suite1; }
            set { filter_disabled_suite1 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite2
        {
            get { return filter_disabled_suite2; }
            set { filter_disabled_suite2 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite3
        {
            get { return filter_disabled_suite3; }
            set { filter_disabled_suite3 = value; }
        }

        [CategoryAttribute("FILTER")]
        public string DisabledSuite4
        {
            get { return filter_disabled_suite4; }
            set { filter_disabled_suite4 = value; }
        }

        public void SetValueByPropertyLabel(string label, string value)
        {
            if ("Server" == label)
            {
                this.Server = value;
            }
            else if ("DataBase" == label)
            {
                this.DataBase = value;
            }
            else if ("UserName" == label)
            {
                this.UserName = value;
            }
            else if ("PassWord" == label)
            {
                this.Password = value;
            }
            else if ("Stage" == label)
            {
                this.Stage = value;
            }
            else if ("Test1" == label)
            {
                this.Test1 = value;
            }
            else if ("Test2" == label)
            {
                this.Test2 = value;
            }
            else if ("Test3" == label)
            {
                this.Test3 = value;
            }
            else if ("Test4" == label)
            {
                this.Test4 = value;
            }
            else if ("TestSuite1" == label)
            {
                this.TestSuite1 = value;
            }
            else if ("TestSuite2" == label)
            {
                this.TestSuite2 = value;
            }
            else if ("TestSuite3" == label)
            {
                this.TestSuite3 = value;
            }
            else if ("TestSuite4" == label)
            {
                this.TestSuite1 = value;
            }
            else if ("DisabledSuite1" == label)
            {
                this.DisabledSuite1 = value;
            }
            else if ("DisabledSuite2" == label)
            {
                this.DisabledSuite2 = value;
            }
            else if ("DisabledSuite3" == label)
            {
                this.DisabledSuite3 = value;
            }
            else if ("DisabledSuite4" == label)
            {
                this.DisabledSuite4 = value;
            }
        }

        public void ResetTest()
        {
            filter_test1 = "";
            filter_test2 = "";
            filter_test3 = "";
            filter_test4 = "";
        }

        public void ResetSuite()
        {
            filter_test_suite1 = "";
            filter_test_suite2 = "";
            filter_test_suite3 = "";
            filter_test_suite4 = "";
        }
    }
}
