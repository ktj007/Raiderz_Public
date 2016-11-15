using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace AsyncDatabase
{
    public enum DB_THREAD_TASK_RESULT
    {
        SUCCESS,
        FAILURE
    }

    public abstract class DBThreadTask
    {
        private static readonly int DB_MAX_REPEAT_QUERY_COUNT = 3;

        private List<string> m_arrSQLString = new List<string>();
        private string m_strTaskName;
        private string m_strErrorMsg;
        private DB_THREAD_TASK_RESULT m_eResult = DB_THREAD_TASK_RESULT.FAILURE;

        public DB_THREAD_TASK_RESULT Result { get { return m_eResult; } }

        public DBThreadTask(string strTaskName)
        {
            m_strTaskName = strTaskName;
        }

        public void PushSQL(string strSQL)
        {
            m_arrSQLString.Add(strSQL);
        }

        public string GetSQL(int nIndex)
	    {
            if (nIndex >= m_arrSQLString.Count) return "";
            return m_arrSQLString[nIndex];
	    }

	    public int GetQuerySize()
	    {
		    return m_arrSQLString.Count;
	    }

        public abstract void OnExecute(Database refDB);
        public abstract void OnCompleted();

        protected bool ExecuteRepeat(ref DBRecordSet refRecordSet, string strSQL)
        {
            if (strSQL.Length == 0)
                return false;

            if (refRecordSet.refDB.CheckDBReconnect() == false)
            {
                m_strErrorMsg = "Fail to Reconnect DB. " + refRecordSet.refDB.LastErrorMsg;
                return false;
            }

            for (int i = 0; i < DB_MAX_REPEAT_QUERY_COUNT; ++i)
            {
                if (refRecordSet.Open(strSQL) == true)
                    return true;

                m_strErrorMsg = refRecordSet.refDB.LastErrorMsg;
            }

            return false;
        }
        protected void SetTaskSuccess()
        {
            m_eResult = DB_THREAD_TASK_RESULT.SUCCESS;
        }
        protected bool CheckResult()
        {
	        if (DB_THREAD_TASK_RESULT.FAILURE == Result)
	        {
                SMCommon.FileLog.Instance.Write("DB Error! Task Result is Failure. TastName:{0}, Message:{1}", m_strTaskName, m_strErrorMsg);
		        return false;
	        }

	        return true;
        }

    }
}
