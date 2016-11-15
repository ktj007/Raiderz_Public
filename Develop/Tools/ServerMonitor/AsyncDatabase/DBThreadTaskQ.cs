using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace AsyncDatabase
{
    public class DBThreadTaskQ
    {
        private LinkedList<DBThreadTask> m_listTask = new LinkedList<DBThreadTask>();
        private AutoResetEvent m_eventPushTast = new AutoResetEvent(false);
        private int m_nWaitEventTime = 0;

        public int Size { get { return m_listTask.Count; } }

        public bool Init(int nWaitEventTime)
        {
            m_nWaitEventTime = nWaitEventTime;
            return true;
        }

        public void Release()
        {
            m_listTask.Clear();
        }

        public void Push(DBThreadTask pTask)
        {
            lock (m_listTask)
            {
                m_listTask.AddLast(pTask);
            }
            SignalPushEvent();
        }

        public void SignalPushEvent()
        {
            m_eventPushTast.Set();
        }

        public DBThreadTask FetchTask()
        {
            m_eventPushTast.WaitOne(m_nWaitEventTime);

            if (0 == m_listTask.Count)
                return null;

            lock(m_listTask)
            {
                DBThreadTask pTask = _FetchTaskUnsafe();
		        return pTask;
            }
        }

        private DBThreadTask _FetchTaskUnsafe()
	    {
		    if (0 == m_listTask.Count)
			    return null;

		    DBThreadTask refFetchedTask = m_listTask.First.Value;
            m_listTask.RemoveFirst();

            return refFetchedTask;
	    }

    }
}
