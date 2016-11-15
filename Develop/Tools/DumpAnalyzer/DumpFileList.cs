using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;
using System.IO;
using System.Collections;

namespace DumpAnalyzer
{
    class DumpFileNameList
    {
        public ArrayList m_FileNameArrayList;

        public DumpFileNameList()
        {
            m_FileNameArrayList = new ArrayList();
        }
        public void Check(string path, bool bCheckFilter)
        {
            m_FileNameArrayList.Clear();

            XFileInfo Files;
            MyComparer comparer;

            DirectoryInfo dir = new DirectoryInfo(path);

            foreach (FileInfo file in dir.GetFiles())
            {
                if (XHelper.IsDumpFile(file.Name) == false) continue;
                if (file.Length <= 0) continue;

                if (bCheckFilter)
                {
                    if (CheckFilter(file.CreationTime) == false) continue;
                }

                Files = new XFileInfo(file.Name, file.CreationTime);
                m_FileNameArrayList.Add(Files);
            }

            comparer = new MyComparer();

            m_FileNameArrayList.Sort(comparer);
        }

        public void OutputDebug()
        {
            for (int i = 0; i < m_FileNameArrayList.Count; i++)
            {
                Console.WriteLine("파일명 : {0} , 날짜 : {1}",
                    ((XFileInfo)m_FileNameArrayList[i]).FileName,
                    ((XFileInfo)m_FileNameArrayList[i]).CreateDate);
            }
        }

        public string GetText()
        {
            string text = "";
            for (int i = 0; i < m_FileNameArrayList.Count; i++)
            {
                text += Convert.ToString(i + 1) + " : " + ((XFileInfo)m_FileNameArrayList[i]).FileName + "\n";
            }

            return text;
        }

        private bool CheckFilter(DateTime creationTime)
        {
            DateTime today = DateTime.Now;
            TimeSpan timeSpan = today - creationTime;

            if (timeSpan.TotalHours <= 24)
            {
                return true;
            }

            return false;
        }

        public int GetCount()
        {
            return m_FileNameArrayList.Count;
        }
    }


}