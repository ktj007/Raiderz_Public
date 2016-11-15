using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsyncDatabase
{
    public class DBThreadTaskResultQ
    {
        private LinkedList<DBThreadTask> m_listTaskResult = new LinkedList<DBThreadTask>();

        public void Release()
        {
            m_listTaskResult.Clear();
        }

        public bool IsEmpty()
        {
            return (m_listTaskResult.Count == 0);
        }

        public DBThreadTask PopFront()
        {
            DBThreadTask refTask = null;
            lock (m_listTaskResult)
            {
                if (IsEmpty()) return null;
                refTask = m_listTaskResult.First.Value;
                m_listTaskResult.RemoveFirst();
            }
            return refTask;
        }

        public void Push(DBThreadTask refTask)
        {
            lock (m_listTaskResult)
            {
                m_listTaskResult.AddLast(refTask);
            }
        }

    }
}
