using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AsyncDatabase;
using SMCommon;

namespace ServerMonitor.DB
{
    class DBTaskInsertLogServerMonitoring : DBThreadTask
    {
        enum SQL_INDEX
        {
            INSERT_LOG_SERVER_MONITORING = 0,
        }

        public DBTaskInsertLogServerMonitoring()
            : base("InsertLogServerMonitoring") { }

        public override void OnExecute(Database refDB)
        {
            DBRecordSet dbRecordSet = new DBRecordSet(refDB);
            if (!ExecuteRepeat(ref dbRecordSet, GetSQL((int)SQL_INDEX.INSERT_LOG_SERVER_MONITORING)))
            {
                FileLog.Instance.Write("Failed! DBTaskInsertLogServerMonitoring, {0}", refDB.LastErrorMsg);
                return;
            }

            SetTaskSuccess();
            dbRecordSet.Close();
        }

        public override void OnCompleted()
        {
            if (CheckResult() == false) return;
        }

    }
}
