using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;

namespace DumpAnalyzer
{
    class XHelper
    {
        public static string MakeAnalysisFileName(string strPath, string strDumpFile)
        {
            string strAnalysisFileName = Path.GetFileNameWithoutExtension(strDumpFile) + ".dmp_analysis.txt";

            string strRet = Path.Combine(strPath, strAnalysisFileName);
            return strRet;
        }

        public static bool IsExistAnalyzeFile(string strPath, string strDumpFile)
        {
            string strAnalyzeFileName = XHelper.MakeAnalysisFileName(strPath, strDumpFile);
            return File.Exists(strAnalyzeFileName);
        }
        public static string AddSeparatorChar(string strPath)
        {
            strPath = strPath.Replace("\"", "");
            if (strPath[strPath.Length - 1] != '\\' &&
                strPath[strPath.Length - 1] != '/')
            {
                strPath += "\\";
            }

            return strPath;
        }

        public static bool IsDumpFile(string strFileName)
        {
            string strExt = Path.GetExtension(strFileName);
            if (strExt.ToLower() == ".dmp") return true;

            return false;
        }

    }

}