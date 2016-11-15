#include "stdafx.h"
 
#include <time.h>
#include <ShellAPI.h>
 
#include "MCrashReporter.h"
#include "MDebug.h"
#include <sstream>
 
MCrashReporter::MCrashReporter(const std::string &strAppName, const std::string &strTargetHost, const int nPort, const std::string &strID, const std::string &strPassword, const bool bAutoAgree)
: m_strAppName(strAppName),
  m_strTargetHost(strTargetHost),
  m_nPort(nPort),
  m_strID(strID),
  m_strPassword(strPassword),
  m_bAutoAgree(bAutoAgree)
{ }
 
std::string MCrashReporter::BuildArguments(const std::vector<MCrashReportFilePair> &vecFiles, const bool bRemoveLocalFile) const
{
    std::ostringstream arg_oss, local_file_oss, remote_file_oss;

	arg_oss << '"';
 
    CheckValidity(m_strAppName);
    arg_oss << "app=" << m_strAppName << ';';
 
    CheckValidity(m_strTargetHost);
    arg_oss << "addr=" << m_strTargetHost << ';';
    
    arg_oss << "port=" << m_nPort << ';';
 
    if (m_strID.length() > 0)
        arg_oss << "id=" << m_strID << ';';
 
    if (m_strPassword.length() > 0)
        arg_oss << "passwd=" << m_strPassword << ';';
    
    arg_oss << "srcdelete=" << (int)bRemoveLocalFile << ';';
 
    bool bAppendSeperator = false;
    for (std::vector<MCrashReportFilePair>::const_iterator 
        it = vecFiles.begin();it != vecFiles.end();it++)
    {
        if (bAppendSeperator)
        {
            local_file_oss << ',';
            remote_file_oss << ',';
        }
        CheckValidity(it->first);
        CheckValidity(it->second);
        local_file_oss << it->first;
        remote_file_oss << it->second;
 
        bAppendSeperator = true;
    }
 
    arg_oss << "localfile=" << local_file_oss.str() << ';';
    arg_oss << "remotefile=" << remote_file_oss.str() << ';';
 
    arg_oss << "agree=" << (int)m_bAutoAgree << ';';

	arg_oss << '"';
 
    return arg_oss.str();
}
 
void MCrashReporter::Report(const std::vector< MCrashReportFilePair > &vecFiles, const bool bRemoveLocalFile) const
{
    std::string command_line = BuildArguments(vecFiles, bRemoveLocalFile);
    ShellExecuteA(0, NULL, "BAReport.exe", command_line.c_str(), NULL, SW_NORMAL);
}
 
std::string MCrashReporter::GetReportDateFormat()
{
    time_t cur_time = time(NULL);
    tm* local_time = localtime(&cur_time);
 
    char szFileName[_MAX_DIR];
    sprintf_s(szFileName, "%04d%02d%02d_%02d%02d%02d", 
        local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
        local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
 
    return std::string(szFileName);
}

std::string MCrashReporter::GetRemotePrefix(const char* _pSzCustomComputerName) const
{
	std::string strComputerName;
	if (_pSzCustomComputerName == NULL)
	{
		DWORD dwComputerNameSize = MAX_COMPUTERNAME_LENGTH + 1;
		char szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = {0, };
		GetComputerName(szComputerName, &dwComputerNameSize);
		strComputerName = szComputerName;
	}
	else
	{
		strComputerName = _pSzCustomComputerName;
	}
 
    std::string date_field = MCrashReporter::GetReportDateFormat();
 
    std::ostringstream remote_prefix_builder;
	remote_prefix_builder << m_strAppName;
	remote_prefix_builder << '_';
    remote_prefix_builder << date_field;
    remote_prefix_builder << '_';
    remote_prefix_builder << strComputerName;
    
    return remote_prefix_builder.str();
}