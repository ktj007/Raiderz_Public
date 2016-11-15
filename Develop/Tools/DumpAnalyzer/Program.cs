using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;

namespace DumpAnalyzer
{
    class Program
    {
        static int Main(string[] args)
        {
            try
            {
                XConfig.Init();

                if (!ParseCommandLine(args))
                {
                    return 1;
                }

                DumpFileNameList list = new DumpFileNameList();
                list.Check(XConfig.m_strPath, false);
                if (list.GetCount() > 0)
                {
                    // dump analysis
                    XDumpAnalyzer dumpAnalyzer = new XDumpAnalyzer();
                    dumpAnalyzer.AnalyzeAllFile(XConfig.m_strPath, list.m_FileNameArrayList);

                    // rename file
                    if (XConfig.m_bRenameFile)
                    {
                        XRenameProcess renameProcess = new XRenameProcess(XConfig.m_strPath, list.m_FileNameArrayList, XConfig.m_Postfixes);
                        renameProcess.RenameAll();
                    }
                }
                
                // sendmail
                if (XConfig.m_bSendEmail)
                {
                    DumpFileNameList newDumplist = new DumpFileNameList();  // Dump File 이름이 바꿔어서 다시 체크
                    newDumplist.Check(XConfig.m_strPath, !XConfig.m_bCheckAll);

                    if (newDumplist.GetCount() > 0)
                    {
                        MailSender mailSender = new MailSender(XConfig.m_EmailIDArrayList, newDumplist);
                        mailSender.Send();
                    }
                }
            }
            catch(System.Exception ex)
            {
                Console.WriteLine("Error: " + ex.ToString());
                return 1;
            }
            return 0;
        }
        static bool ParseCommandLine(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                if (args[i].CompareTo("/?") == 0 ||
                    args[i].CompareTo("-h") == 0)
                {
                    Console.WriteLine("");
                    Console.WriteLine(" usage : DumpAnalyzer [/?] [-h] [-path TARGETPATH] [-symbolpath TARGETPATH] [-config CONFIGFILE] [--sendmail] [--norename]");
                    Console.WriteLine("");
                    Console.WriteLine("  /? or -h    : 도움말 출력");
                    Console.WriteLine("  -path       : 대상 경로 설정 (기본값은 현재 경로)");
                    Console.WriteLine("  -symbolpath : 심볼 경로 설정 (기본값은 icecream의 심볼 경로)");
                    Console.WriteLine("  -config     : 외부 설정 파일");
                    Console.WriteLine("  --sendmail  : 보고서 메일 보냄");
                    Console.WriteLine("  --norename  : 덤프 파일 이름을 바꾸지 않음");
                    Console.WriteLine("");
                    Console.WriteLine(" Copyright (C) MAIET Entertainment 2009");
                    Console.WriteLine("");
                    return false;
                }
                else if (args[i].CompareTo("--sendmail") == 0)
                {
                    XConfig.m_bSendEmail = true;
                }
                else if (args[i].CompareTo("--norename") == 0)
                {
                    XConfig.m_bRenameFile = false;
                }
                else if (args[i].CompareTo("-path") == 0)
                {
                    if (i + 1 < args.Length)
                    {
                        string strPath = args[i + 1];
                        if (strPath.Length > 0)
                        {
                            XConfig.m_strPath = XHelper.AddSeparatorChar(strPath);
                        }

                        i++;
                    }
                }
                else if (args[i].CompareTo("-symbolpath") == 0)
                {
                    if (i + 1 < args.Length)
                    {
                        string strPath = args[i + 1];
                        if (strPath.Length > 0)
                        {
                            XConfig.m_strSymbolPath = XHelper.AddSeparatorChar(strPath);
                        }

                        i++;
                    }
                }
                else if (args[i].CompareTo("-config") == 0 )
                {
                    if (i + 1 < args.Length)
                    {
                        string strConfigFile = args[i + 1];
                        if (strConfigFile.Length > 0)
                        {
                            XConfig.LoadConfig( strConfigFile );
                        }

                        i++;
                    }
                }

            }

            return true;
        }
    }





}


