#pragma once
 
#include "RLib.h"
#include <vector>
#include <utility>
#include <string>
 
/// (local_file, remote_file)
typedef std::pair<std::string, std::string> MCrashReportFilePair;
 
// authored by beingryu @ 2010-05-20
class CML2_API MCrashReporter
{
public:
    static std::string MCrashReporter::GetReportDateFormat();
	std::string MCrashReporter::GetRemotePrefix(const char* _pSzCustomComputerName = NULL) const;

	inline void SetAutoAgree(bool bAutoAgree)
	{ m_bAutoAgree = bAutoAgree; }
 
    MCrashReporter(
        const std::string &strAppName,          // e.g. "raiderz"
        const std::string &strTargetHost,       // "icecream"
        const int nPort,                        // 21 for FTP
        const std::string &strID,               // "dev"
        const std::string &strPassword,         // "dev"
        const bool bAutoAgree = false
        );
 
    void Report(const std::vector< MCrashReportFilePair > &vecFiles, const bool bRemoveLocalFile = false) const;

protected:
    std::string m_strAppName;
    std::string m_strTargetHost;
    int m_nPort;
    std::string m_strID;
    std::string m_strPassword;
    bool m_bAutoAgree;
 
    // deprecated
    std::string m_strGID;
    std::string m_strUser;
 
private:
#pragma warning(push)
#pragma warning(disable: 4100)
    inline void CheckValidity(const std::string &strToValidate) const
    {
        _ASSERT(
            strToValidate.length() > 0 && 
            strToValidate.find(';') == strToValidate.npos && 
            strToValidate.find(',') == strToValidate.npos);
    }
#pragma warning(pop)
 
    std::string BuildArguments(const std::vector< MCrashReportFilePair > &vecFiles, const bool bRemoveLocalFile) const;
};