using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;
using System.IO;
using System.Collections;


namespace DumpAnalyzer
{

    public class XFileInfo
    {
        private string m_FileName;
        private DateTime m_CreateDate;

        public XFileInfo(string fileName, DateTime createDate)
        {
            this.m_FileName = fileName;
            this.m_CreateDate = createDate;
        }

        public string FileName
        {
            get
            {
                return this.m_FileName;
            }
            set
            {
                this.m_FileName = value;
            }
        }

        public DateTime CreateDate
        {
            get
            {
                return this.m_CreateDate;
            }
            set
            {
                this.m_CreateDate = value;
            }
        }
    }

    public class MyComparer : IComparer
    {
        bool _isSort;
        int result;

        #region IComparer 멤버

        public int Compare(object x, object y)
        {
            if ((x is XFileInfo) && (y is XFileInfo)) // XFileInfo 형변환 여부 체크
            {
                XFileInfo file1 = (XFileInfo)x;
                XFileInfo file2 = (XFileInfo)y;

                if (_isSort)
                {
                    result = String.Compare(file1.CreateDate.ToString(), file2.CreateDate.ToString());
                }
                else
                {
                    result = (-String.Compare(file1.CreateDate.ToString(), file2.CreateDate.ToString()));
                }
            }

            return result;
        }

        public bool isSort
        {
            set { this._isSort = value; }
        }

        #endregion

    }

}