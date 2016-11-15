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
    class XCallStackParser
    {
        private bool IsExistStr(string str, string token)
        {
            int n = str.IndexOf(token);
            if (n > 0) return true;
            return false;
        }
        private bool ValidateString(string str)
        {
            for (int i = 0; i < XConfig.m_ignoreFunctionPatternList.Count; i++)
            {
                string strIgnorePattern = (string)XConfig.m_ignoreFunctionPatternList[i];
                if (IsExistStr(str, strIgnorePattern)) return false;
            }

//             if (IsExistStr(str, "memcpy")) return false;
//             if (IsExistStr(str, "std::")) return false;
//             if (IsExistStr(str, "free.c")) return false;
//             if (IsExistStr(str, "_CxxThrowException")) return false;

            return true;
        }
        private string ReplaceInvalidChars(string str)
        {
            string strRet = str;
            strRet = strRet.Replace("Raiderz!", "");
            strRet = strRet.Replace("GameServer!", "");

            strRet = strRet.Replace("::", "-");

            strRet = strRet.Replace("\\", "_");
            strRet = strRet.Replace("/", "_");
            strRet = strRet.Replace(":", ";");
            strRet = strRet.Replace("?", "_");
            strRet = strRet.Replace("<", "(");
            strRet = strRet.Replace(">", ")");
            strRet = strRet.Replace("|", "_");
            strRet = strRet.Replace("*", "#");

            return strRet;
        }

        public string Parse(string strAnalyzeFileName)
        {
            string strRet = "";

            bool bFind = false;

            StreamReader streamReader;
            string S;
            streamReader = File.OpenText(strAnalyzeFileName);
            S = streamReader.ReadLine();
            while (bFind == false && S != null)
            {
                string sPattern = " @ [1234567890]";

                if (Regex.IsMatch(S, sPattern))
                {
                    string strLine = S;

                    if (ValidateString(strLine))
                    {
                        string[] strSplit = strLine.Split(' ');

                        string str1 = strSplit[2];

                        // 템플릿 함수 같은경우에 함수 이름에 공백이 낄수 있어서 마저 합쳐준다
                        for (int i = 3; i < strSplit.Length; i++)
                        {
                            if (IsExistStr(strSplit[i], "+0x"))
                            {
                                for( int j = 3; j<=i; j++ )
                                {
                                    str1 += " " + strSplit[j];
                                }
                                break;
                            }
                        }

                        str1 = ReplaceInvalidChars(str1);

                        if (str1.Length > 0)
                        {
                            bFind = true;
                            strRet = str1;
                        }
                    }
                }

                S = streamReader.ReadLine();
            }
            streamReader.Close();

            return strRet;
        }
    }

}