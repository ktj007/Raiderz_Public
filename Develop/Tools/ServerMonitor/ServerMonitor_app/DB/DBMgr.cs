using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AsyncDatabase;
using SMCommon;

namespace ServerMonitor.DB
{
    public class DBMgr
    {
        private AsyncDatabase.AsyncDatabase m_asyncDB = new AsyncDatabase.AsyncDatabase();
        private AsyncDatabase.Database m_syncDB = new AsyncDatabase.Database();
        private string m_strLastError = "";

        public virtual bool IsSyncDBOpen { get { return m_syncDB.IsOpen; } }
        public string LastErrorMsg { get { string strTemp = m_strLastError; m_strLastError = ""; return strTemp; } }

        public bool Create(string strDataSource, string strDBName, string strID, string strPwd)
        {
            System.Data.SqlClient.SqlConnectionStringBuilder builder = new System.Data.SqlClient.SqlConnectionStringBuilder();
            builder.DataSource = strDataSource;
            builder.InitialCatalog = strDBName;
            builder.UserID = strID;
            builder.Password = strPwd;
            builder.IntegratedSecurity = false;

            if (m_asyncDB.Create(builder.ConnectionString) == false)
                return false;

            if (m_syncDB.Connect(builder.ConnectionString) == false)
                return false;

            return true;
        }
        public void Release()
        {
            m_asyncDB.Release();
        }
        public void Update()
        {
            m_asyncDB.Update();
        }

        public bool CheckSyncDBReconnect()
        {
            return m_syncDB.CheckDBReconnect();
        }

        public bool InsertLogServerMonitor(int nServerMonitorID, int nWorldID, int nServerID, string strLogMessage)
        {
            //string strSQL_SERVER_MONITOR_INSERT_LOG = "EXEC spServerMonitorInsertLog {0}, {1}, {2}, '{3}'";
            //strSQL_SERVER_MONITOR_INSERT_LOG = string.Format(strSQL_SERVER_MONITOR_INSERT_LOG, nServerMonitorID, nWorldID, nServerID, strLogMessage);

            //DBTaskInsertLogServerMonitoring dbTask = new DBTaskInsertLogServerMonitoring();
            //dbTask.PushSQL(strSQL_SERVER_MONITOR_INSERT_LOG);

            //return m_asyncDB.ExecuteAsync(dbTask);
            return true;
        }
        
        public virtual bool GetServerStatusInfo_Sync(int nWorldID, int nServerID, out bool bIsRun, out bool bIsServable, out short nPlayerCount)
        {
            string strSQL = "EXEC dbo.USP_RZ_SERVER_GET_STATUS_INFO {0}, {1}";
            strSQL = string.Format(strSQL, nWorldID, nServerID);

            bIsRun = false;
            bIsServable = false;
            nPlayerCount = 0;

            DBRecordSet rs = new DBRecordSet(m_syncDB);
            if (rs.Open(strSQL) == false)
            {
                m_strLastError = m_syncDB.LastErrorMsg;
                rs.Close();
                return false;
            }

            if (rs.HasRows == false)
            {
                rs.Close();
                return false;
            }

            int nIsRun;
            if (rs.GetInt32("IS_RUN", out nIsRun) == false) { rs.Close(); return false; }
            if (rs.GetBool("SERVABLE", out bIsServable) == false) { rs.Close(); return false; }

            bIsRun = (nIsRun == 1 ? true : false);

            rs.Close();
            return true;
        }

    }
}
