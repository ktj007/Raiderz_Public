using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Diagnostics;
using System.IO;

namespace ServerRuntimeLogChecker
{
    class Program
    {
        static int Main(string[] args)
        {
            // 인자검사. 인자는 1개만 받음.
            if (args.Length == 0)
            {
                Log("[실패] 파일명이 없습니다. 검사할 파일명을 입력해주세요.");
                return 1;
            }

            if (1 < args.Length)
            {
                Log("[실패] 인자가 많습니다. 하나의 파일명만 입력해주세요.");
                return 1;
            }

            
            // 검사 시작
            string strPath = args[0];
            XmlDocument docValidateLog = new XmlDocument();

            // 유효성 검사 로그가 남아있지 않으면 유효함
            try
            {
                docValidateLog.Load(strPath);
            }
            catch (FileNotFoundException)
            {
                Log("[성공] 검사할 파일을 찾을 수 없습니다.");
                return 0;
            }
            catch (DirectoryNotFoundException)
            {
                Log("[성공] 검사할 경로를 찾을 수 없습니다.");
                return 0;
            }
            catch (XmlException)
            {
                Log("[실패] XML 형식이 아니거나, 유효하지 않은 파일입니다.");
                return 1;
            }


            // 루트 엘리먼트가 존재하지 않아도 유효함
            XmlNode nodeRoot = docValidateLog.SelectSingleNode("maiettest-results");

            if (nodeRoot == null)
            {
                Log("[성공] 유효성검사 결과가 없습니다.");
                return 0;
            }


            // 레포트 엘리먼트가 없어도 유효함
            XmlNodeList nodeList = nodeRoot.SelectNodes("maietreport");                                                         

            if (nodeList.Count == 0)
            {
                Log("[성공] 유효성검사 실패 보고가 없습니다.");
                return 0;
            }


            // 레포트된 에러로그를 찾아냈다면, 유효하지 않음
            Log("[실패] 유효성검사 실패 보고가 발견되었습니다.");
            return 1;
        }

        static void Log(string message)
        {
            Console.WriteLine(message);
            Debug.WriteLine(message);
        }
    }
}
