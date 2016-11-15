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
    class XDumpAnalyzer
    {
        private string m_strPath;

        public XDumpAnalyzer()
        {

        }

        public void AnalyzeAllFile(string strPath, ArrayList DumpFileNameArrayList)
        {
            m_strPath = strPath;

            for (int i = 0; i < DumpFileNameArrayList.Count; i++)
            {
                string strDumpFileName = ((XFileInfo)DumpFileNameArrayList[i]).FileName;

                if (XHelper.IsDumpFile(strDumpFileName) == false) continue;
                if (XHelper.IsExistAnalyzeFile(strPath, strDumpFileName) == true) continue;

                Analyze(strPath, strDumpFileName);
            }
        }

        public void Analyze(string strPath, string strDumpFileName)
        {
            m_strPath = strPath;

            ExecWinDbg(strDumpFileName);
        }

        private string MakeExecString(string strDumpFile, string strLogFileName)
        {
            string strRet;

            string strWinDbg = "windbg.exe";
            strWinDbg = "";

            strRet = strWinDbg + " -logo \""
                        + strLogFileName
                        + "\" -Q -y \""
                        + XConfig.m_strSymbolPath
                        + "\" -z \""
                        + strDumpFile
                        + "\" -c \".reload /f;!analyze ?v;.ecxr;k;q\"";

            return strRet;
        }
        private void ExecWinDbg(string strDumpFileName)
        {
            string strLogFullFileName = XHelper.MakeAnalysisFileName(m_strPath, strDumpFileName);
            string strDumpFullFileName = Path.Combine(m_strPath, strDumpFileName);

            ProcessStartInfo startInfo = new ProcessStartInfo(Path.Combine(XConfig.m_strWindbgPath, "windbg.exe"));
            //startInfo.WorkingDirectory = "";
            startInfo.Arguments = MakeExecString(strDumpFullFileName, strLogFullFileName);

            Process.Start(startInfo);

            WaitForWinDbgExit();
        }
        private void WaitForWinDbgExit()
        {
            System.Diagnostics.Process[] myProcesses;
            myProcesses = System.Diagnostics.Process.GetProcessesByName("windbg");
            foreach (System.Diagnostics.Process instance in myProcesses)
            {
                instance.WaitForExit(300000);
            }

        }
    }
}
