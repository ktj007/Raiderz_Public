using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;
using System.IO;
using System.Collections;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace DumpAnalyzer
{
    class XRenameProcess
    {
        private string m_strPath;
        private ArrayList m_DumpFileNameList;
        private List<string> m_Postfixes;

        public XRenameProcess(string strPath, ArrayList dumpFileNameList)
        {
            m_strPath = strPath;
            m_DumpFileNameList = dumpFileNameList;

            m_Postfixes = new List<string>();
            m_Postfixes.Add(".dmp");
            m_Postfixes.Add(".dmp_analysis.txt");
            m_Postfixes.Add("_mlog.txt");
        }

        public XRenameProcess(string strPath, ArrayList dumpFileNameList, IEnumerable<String> additionalPostfixes) : this(strPath, dumpFileNameList)
        {
            m_Postfixes.AddRange(additionalPostfixes);
        }

        private bool IsRenamedAlready(string strDumpFileName)
        {
            string strPure = Path.GetFileNameWithoutExtension(strDumpFileName);

            int n = strPure.IndexOf("0x");
            if (n > 0) return true;
            return false;
        }
        private void DoRename(string strSrc, string strDest)
        {
            FileInfo fileRename = new FileInfo(strSrc);
            if (fileRename.Exists)
            {
                fileRename.MoveTo(strDest);
            }
        }
        private void Rename(string strSrcPureName, string strFuncName)
        {
            if (IsRenamedAlready(strSrcPureName)) return;

            foreach (string postfix in m_Postfixes)
            {
                string strFullName = String.Format("{0}{1}", strSrcPureName, postfix);

                string strSrcFullPath = Path.Combine(m_strPath, strFullName);
                string strDestFullPath = Path.Combine(m_strPath, String.Format("{0}_{1}", strFuncName, strFullName));

                DoRename(strSrcFullPath, strDestFullPath);
            }
        }
        public void RenameAll()
        {
            for (int i = 0; i < m_DumpFileNameList.Count; i++)
            {
                string strDumpFileName = ((XFileInfo)m_DumpFileNameList[i]).FileName;

                if (XHelper.IsExistAnalyzeFile(m_strPath, strDumpFileName) == false) continue;

                string strFuncName = MakeFuncName(strDumpFileName);
                if (strFuncName.Length <= 0) continue;

                string strPureDumpFileName = Path.GetFileNameWithoutExtension(strDumpFileName);
                string strPureDestFileName = strFuncName + strPureDumpFileName;

                Rename(strPureDumpFileName, strFuncName);
            }

        }
        
        private string MakeFuncName(string strDumpFileName)
        {
            string strAnalyzeFileName = XHelper.MakeAnalysisFileName(m_strPath, strDumpFileName);

            XCallStackParser parser = new XCallStackParser();
            string strFuncName = parser.Parse(strAnalyzeFileName);
            
            if(strFuncName.Length == 0)
                strFuncName += "unknown_function";

            if (strFuncName.Length > 100)
                strFuncName = strFuncName.Substring(0, 100) + "_blahblah_";

            // 리네임 되었는지의 식별자 이므로 없으면 붙여준다
            int n = strFuncName.IndexOf("0x");
            if (n <= 0)
                strFuncName += "+0x0";
            return strFuncName;
        }
    }
}