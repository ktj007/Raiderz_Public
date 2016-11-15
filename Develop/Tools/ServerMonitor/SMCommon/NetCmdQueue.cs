using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace SMCommon
{
    public class NetCmdQueue
    {
        List<NetCommand> m_listCmdQueue = new List<NetCommand>();

        public void Clear()
        {
            m_listCmdQueue.Clear();
        }

        public void PushCommand(NetCommand command)
        {
            lock (m_listCmdQueue)
            {
                 m_listCmdQueue.Add(command);
            }
        }

        public NetCommand PopCommand()
        {
            NetCommand command = null;

            lock (m_listCmdQueue)
            {
                if (m_listCmdQueue.Count > 0)
                {
                    command = m_listCmdQueue[0];
                    m_listCmdQueue.RemoveAt(0);
                }
            }
            return command;
        }
    }
}
