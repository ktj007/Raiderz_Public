using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SMCommon
{
    public class FileLog
    {
        private readonly string DEFAULT_LOGFILE_NAME = "smlog.txt";

        private static FileLog m_sLogger = null;
        private string m_strLogFileName;

        public static FileLog Instance
        {
            get
            {
                if (m_sLogger == null)
                    m_sLogger = new FileLog();
                return m_sLogger;
            }
        }

        public FileLog ChangeInstance(FileLog newInstance)
        {
            FileLog originalInstance = m_sLogger;
            m_sLogger = newInstance;
            return originalInstance;
        }

        public void Init(string strLogFileName)
        {
            m_strLogFileName = strLogFileName;

            if (m_strLogFileName.Length == 0)
                m_strLogFileName = DEFAULT_LOGFILE_NAME;

            try
            {
                if (System.IO.File.Exists(m_strLogFileName) == true)
                    System.IO.File.Delete(m_strLogFileName);
            }
            catch {}

            OutputFile("===========================================================================");
            OutputFile("  " + DateTime.Now);
            OutputFile("===========================================================================");
        }

        public virtual void Write(string strFormat, params object[] args)
        {
            string strTime = DateTime.Now.ToString("yyyy/MM/dd-HH:mm:ss");
            string strMessage = String.Format(strFormat, args);
            string strLogMsg = strTime + " - " + strMessage;

            OutputFile(strLogMsg);
        }

        private void OutputFile(string strLog)
        {
            try
            {
                StreamWriter writer = new StreamWriter(m_strLogFileName, true);
                writer.WriteLine(strLog);
                writer.Flush();
                writer.Close();
            }
            catch
            {
                return;
            }
        }

        protected FileLog()
        {
            m_strLogFileName = DEFAULT_LOGFILE_NAME;
        }

    }
}
