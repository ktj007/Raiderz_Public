using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsyncDatabase
{
    class DBThreadPool
    {
        private List<DBThread> m_listThread = new List<DBThread>();

        public bool Create(uint nThreadCount, string strDSN, DBThreadTaskQ refTaskQ, DBThreadTaskResultQ refTaskResultQ)
        {
            for (int i = 0; i < nThreadCount; ++i)
		    {
			    DBThread dbThread = new DBThread();
    			
			    if (dbThread.Create(strDSN, refTaskQ, refTaskResultQ) == false)
			    {
				    return false;
			    }

                m_listThread.Add(dbThread);
		    }
            return true;
        }

        public void Release()
        {
            foreach (DBThread refThread in m_listThread)
            {
                refThread.Release();
            }
            m_listThread.Clear();
        }

        public void SetStopWorker()
        {
            foreach (DBThread refThread in m_listThread)
            {
                refThread.SetStopWorker();
            }
        }

        public bool IsDestroyedWorker()
        {
            foreach (DBThread refThread in m_listThread)
            {
                if (refThread.IsDestroyedWorker == false)
                    return false;
            }
            return true;
        }

    }
}
