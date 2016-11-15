using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using SMCommon;

namespace AsyncDatabase
{
    class DBThread
    {
        private Database m_DB = new Database();
        private DBThreadTaskQ m_refTaskQ = null;
        private DBThreadTaskResultQ m_refTaskResultQ = null;
        private Thread m_threadWorker = null;
        private bool m_bRunWorker = false;
        private bool m_bIsDestroyedWorker = true;

        public bool IsDestroyedWorker { get { return m_bIsDestroyedWorker; } }

        public bool Create(string strDSN, DBThreadTaskQ refTaskQ, DBThreadTaskResultQ refTaskResultQ)
        {
            m_refTaskQ = refTaskQ;
            m_refTaskResultQ = refTaskResultQ;

            if (m_DB.Connect(strDSN) == false)
            {
                FileLog.Instance.Write("Failed! DBThread,Create(), DB 연결 실패, Msg = {0}", m_DB.LastErrorMsg);
                return false;
            }

            try
            {
                m_bRunWorker = true;
                m_bIsDestroyedWorker = false;

                m_threadWorker = new Thread(new ThreadStart(_WorkerThread));
                m_threadWorker.Start();
            }
            catch (System.Exception e)
            {
                m_bRunWorker = false;
                m_bIsDestroyedWorker = true;

                FileLog.Instance.Write("Failed! DBThread,Create(), Msg = {0}", e.Message);
                return false;
            }
            return true;
        }

        public void Release()
	    {
            m_bRunWorker = false;
            while (IsDestroyedWorker == false) {}
            m_DB.Close();
	    }

        public void SetStopWorker()
        {
            m_bRunWorker = false;
        }

        private void _WorkerThread()
        {
            if (null == m_refTaskQ) return;
            if (null == m_refTaskResultQ) return;

            while (m_bRunWorker)
            {
                DBThreadTask task = m_refTaskQ.FetchTask();
                if (null == task) continue;

                task.OnExecute(m_DB);
                m_refTaskResultQ.Push(task);
            }

            m_bIsDestroyedWorker = true;
        }

    }
}
