using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsyncDatabase
{
    public class AsyncDatabase
    {
        private readonly uint DB_THREAD_QSIZE_FREE	= 0;
        private readonly uint DB_THREAD_COUNT = 2;
        private readonly int DB_TASK_Q_DEFAULT_WAIT = 1000; // 1초.

        private DBThreadPool m_ThreadPool = new DBThreadPool();
        private DBThreadTaskQ m_TaskQ = new DBThreadTaskQ();
        private DBThreadTaskResultQ m_TaskResultQ = new DBThreadTaskResultQ();
        private uint m_nMaxTaskQSize = 0;
        private uint m_nAsyncExecCount = 0;
        private uint m_dwAsyncCompltedCount = 0;

        public uint ExecuteCount { get { return m_nAsyncExecCount; } }
        public uint CompleteCount { get { return m_dwAsyncCompltedCount; } }

        public bool Create(string strDSN)
        {
            return Create(strDSN, DB_THREAD_COUNT, DB_THREAD_QSIZE_FREE, DB_TASK_Q_DEFAULT_WAIT);
        }

        public bool Create(string strDSN, uint nThreadCount, uint nMaxTaskQSize, int nWaitEventTime)
        {
            m_nMaxTaskQSize = nMaxTaskQSize;

            if (!m_TaskQ.Init(nWaitEventTime))
            {
                return false;
            }

            if (m_ThreadPool.Create(nThreadCount, strDSN, m_TaskQ, m_TaskResultQ) == false)
            {
                return false;
            }
            return true;
        }

        public void Release()
	    {
		    // 여기선 큐에 남은 작업에 대해선 책임지지 않는다.
		    WaitForThreadExitCompleted();

		    m_ThreadPool.Release();
		    m_TaskQ.Release();
		    m_TaskResultQ.Release();
	    }

        public bool ExecuteAsync(DBThreadTask pTask)
        {
		    if ((DB_THREAD_QSIZE_FREE != m_nMaxTaskQSize) 
			    && (m_nMaxTaskQSize < m_TaskQ.Size))
		    {
			    return false;
		    }

            m_nAsyncExecCount++;

		    m_TaskQ.Push(pTask);
            return true;
        }

        public void Update()
	    {
		    DBThreadTask refTask = null;

		    while (true)
		    {
			    if (m_TaskResultQ.IsEmpty())
				    break;

			    refTask = m_TaskResultQ.PopFront();
                if (null == refTask) break;
                refTask.OnCompleted();

                m_dwAsyncCompltedCount++;
		    }
	    }

        private void WaitForThreadExitCompleted()
	    {
		    m_ThreadPool.SetStopWorker();
		    while (false == m_ThreadPool.IsDestroyedWorker())
		    {
			    m_TaskQ.SignalPushEvent();
		    }
	    }

    }
}
