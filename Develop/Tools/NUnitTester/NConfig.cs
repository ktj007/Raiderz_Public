/*
 * @brief   필터를 설정을 포함한 테스트 설정값을 저장
 * @author  aibeast
 * @date    2009/03/20
 * @desc    현재 사용하지 않습니다. 추후 필터 및 테스트 환경 설정 기능이 추가되면 사용합니다.
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;


namespace NUnitTester
{
    class NConfig
    {
        static public readonly int MAX_FILTER_TEST_COUNT = 4;
        static public readonly int MAX_FILTER_DISABLED_TEST_COUNT = 4;
        static public readonly string STRING_INIFILE_NAME = "\\unittest.ini";

        static public string[] m_strFilter_TEST = new string[MAX_FILTER_TEST_COUNT];
        static public string[] m_strFilter_DISABLED_TEST = new string[MAX_FILTER_DISABLED_TEST_COUNT];

        #region GetPrivateProfileString 함수 Import
        [DllImport("kernel32.dll")]
        private static extern int GetPrivateProfileString(    // INI Read
            String section,
            String key,
            String def,
            StringBuilder retVal,
            int size,
            String filePath);
        #endregion

        static public bool Init_INI()
        {
            String strINIFilePath = System.Environment.CurrentDirectory + STRING_INIFILE_NAME;

            StringBuilder strFilter_TEST = new StringBuilder();
            GetPrivateProfileString("FILTER", "TEST1", "", strFilter_TEST, 2000, strINIFilePath);
            m_strFilter_TEST[0] = strFilter_TEST.ToString();
            GetPrivateProfileString("FILTER", "TEST2", "", strFilter_TEST, 2000, strINIFilePath);
            m_strFilter_TEST[1] = strFilter_TEST.ToString();
            GetPrivateProfileString("FILTER", "TEST3", "", strFilter_TEST, 2000, strINIFilePath);
            m_strFilter_TEST[2] = strFilter_TEST.ToString();
            GetPrivateProfileString("FILTER", "TEST4", "", strFilter_TEST, 2000, strINIFilePath);
            m_strFilter_TEST[3] = strFilter_TEST.ToString();


            StringBuilder strFilter_DISABLED_SUITE = new StringBuilder();
            GetPrivateProfileString("FILTER", "DISABLED_TEST1", "", strFilter_DISABLED_SUITE, 2000, strINIFilePath);
            m_strFilter_DISABLED_TEST[0] = strFilter_DISABLED_SUITE.ToString();
            GetPrivateProfileString("FILTER", "DISABLED_TEST2", "", strFilter_DISABLED_SUITE, 2000, strINIFilePath);
            m_strFilter_DISABLED_TEST[1] = strFilter_DISABLED_SUITE.ToString();
            GetPrivateProfileString("FILTER", "DISABLED_TEST3", "", strFilter_DISABLED_SUITE, 2000, strINIFilePath);
            m_strFilter_DISABLED_TEST[2] = strFilter_DISABLED_SUITE.ToString();
            GetPrivateProfileString("FILTER", "DISABLED_TEST4", "", strFilter_DISABLED_SUITE, 2000, strINIFilePath);
            m_strFilter_DISABLED_TEST[3] = strFilter_DISABLED_SUITE.ToString();

            return true;
        }
    }
}
