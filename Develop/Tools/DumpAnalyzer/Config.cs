using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;
using System.IO;
using System.Collections;
using System.Xml;


namespace DumpAnalyzer
{

    public class XConfig
    {
        // prog
        static public string m_strProgName;             /// 덤프 프로그램 이름
        static public bool m_bRenameFile;               /// 덤프 파일이름을 바꿀지 여부

        // path
        static public string m_strPath;                 /// 덤프가 있는 폴더
        static public string m_strSymbolPath;           /// 심볼 경로
        static public string m_strWindbgPath;           /// windbg 설치 경로

        // email
        static public bool m_bSendEmail;                /// 결과 email을 보낼지 여부
        static public string m_strSendEmailAddress;     /// 이메일 보낼 address
        static public string m_strSMTPServer;           /// 이메일 보낼 서버
        static public ArrayList m_EmailIDArrayList;     /// 이메일 리스트
                                                        /// 
        // ignore pattern
        static public ArrayList m_ignoreFunctionPatternList;    /// 무시할 함수 이름 패턴

        // check
        static public bool m_bCheckAll;                 /// 기간에 상관없이 모두 분석할지 여부
        static public int m_nCheckRecentHour;           /// 만약 최근 파일만 분석한다면 최근 몇시간까지 분석할 것인가

        static private List<string> _m_Postfixes = new List<string>();
        static public IEnumerable<string> m_Postfixes
        {
            get { return _m_Postfixes; }
        }

        static public void Init()
        {
            m_bRenameFile = true;
            m_strProgName = "Raiderz";

            m_strPath = ".\\";
            m_strSymbolPath = ".\\;\\\\icecream\\SYMBOLS\\PRODUCTSYMBOLS;\\\\icecream\\SYMBOLS\\OSSYMBOLS;\\\\gunz2\\SYMBOLS\\PRODUCTSYMBOLS";

            m_strWindbgPath = "C:\\Program Files\\Debugging Tools for Windows (x86)";
            

            m_bCheckAll = false;
            m_nCheckRecentHour = 24;

            m_bSendEmail = false;
            m_strSendEmailAddress = "bugs@maiet.net";
            m_strSMTPServer = "icecream";

            m_EmailIDArrayList = new ArrayList();

            // 그냥 DumpAnalyzer.exe만 실행해도 메일이 가도록
            // 기본적으로 soulhunt-dev 메일링 리스트에 자동적으로 보내도록 합니다. 

            m_EmailIDArrayList.Add("projecth_prog@maiet.net");

            // 여기에 이메일 추가 - config
            //m_EmailIDArrayList.Add("birdkr@gmail.com");
            //m_EmailIDArrayList.Add("poksion@maiet.net");
            //m_EmailIDArrayList.Add("madduck@maiet.net");
            //m_EmailIDArrayList.Add("billcap@maiet.net");
            //m_EmailIDArrayList.Add("popsongq@nate.com");
            //m_EmailIDArrayList.Add("mizelan@gmail.com");
            //m_EmailIDArrayList.Add("luna1x@gmail.com");
            //m_EmailIDArrayList.Add("isaak226@naver.com");
            //m_EmailIDArrayList.Add("dubble@maietgames.com");
            //m_EmailIDArrayList.Add("isnara@maiet.net");
            //m_EmailIDArrayList.Add("praptor@maiet.net");
            //m_EmailIDArrayList.Add("charmke@maiet.net");

            m_ignoreFunctionPatternList = new ArrayList();
            m_ignoreFunctionPatternList.Add("memcpy");
            m_ignoreFunctionPatternList.Add("std::");
            m_ignoreFunctionPatternList.Add("free.c");
            m_ignoreFunctionPatternList.Add("_CxxThrowException");
        }

        /// <summary>
        /// xml 파일로부터 설정을 읽어온다, 프로그램 이름과 이메일만 넣어놨다
        /// </summary>
        /// <param name="strFileName"></param>
        static public void LoadConfig(string strFileName)
        {
            try
            {
                m_EmailIDArrayList.Clear();

                var doc = new XmlDocument();
                doc.Load(strFileName);

                foreach (XmlNode node in doc.DocumentElement.ChildNodes)
                {
                    if (node.NodeType != XmlNodeType.Element)
                        continue;

                    var elem = node as XmlElement;

                    switch (elem.Name.ToLower())
                    {
                        case "email":
                            m_EmailIDArrayList.Add(elem.InnerText);
                            break;
                        case "name":
                            m_strProgName = elem.InnerText;
                            break;
                        case "mailserver":
                            m_strSMTPServer = elem.InnerText;
                            break;
                        case "postfix":
                            _m_Postfixes.Add(elem.InnerText);
                            break;
                        case "ignore":
                            m_ignoreFunctionPatternList.Add(elem.InnerText);
                            break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
    }

}