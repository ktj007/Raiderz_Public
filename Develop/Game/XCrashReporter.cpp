#include "stdafx.h"
#include "XCrashReporter.h"
#include <ShellAPI.h>
#include "XGlobal.h"
#include "SVNRevision.h"
#include "XMyPlayer.h"
#include "XPath.h"

#include "XGlobalVar.h"

bool XCrashReporter::bAutoReport = false;

void XCrashReporter::LogInfo()
{
	mlog("-- Crash! --\n");

	mlog("Build Revision=%s\n", SVN_REVISION);

	if (GlobalVar::GetInstancePtr() == NULL) return;

	mlog("FieldID=%d , GameTime=%d, Weather=%d\n", gvar.World.GetFieldID(), gvar.World.nGameTime, gvar.World.nWeather);

	if (gvar.Game.pMyPlayer)
	{
		vec3 pos = gvar.Game.pMyPlayer->GetPosition();
		mlog("Pos = (%.1f %.1f %.1f)\n", pos.x, pos.y, pos.z);
	}
}

void XCrashReporter::Report(const std::string &strLocalDumpName, const std::string &strAppName)
{
	string strDestIP = MLocale::ConvTCHARToAnsi(XCONST::CONST_CRASH_DUMP_DEST_IP_LOCAL.c_str());

	if(gvar.Network.PmangInfo.isLoginOnPmang)
		strDestIP = MLocale::ConvTCHARToAnsi(XCONST::CONST_CRASH_DUMP_DEST_IP.c_str());

	int strDestPort = XCONST::CONST_CRASH_DUMP_DEST_PORT;

	MCrashReporter reporter(strAppName, strDestIP, strDestPort, "raiderz", "ihatebug", bAutoReport);

	char szFileName[512]="";
	string strPath = MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str());
	
	sprintf_s(szFileName, "%s\\mlog.txt", strPath.c_str());

	std::string remote_prefix;
	if (gvar.MyInfo.strLoginID.empty())
	{
		remote_prefix = reporter.GetRemotePrefix(NULL);
	}
	else
	{
		remote_prefix = reporter.GetRemotePrefix(MLocale::ConvTCHARToAnsi( gvar.MyInfo.strLoginID.c_str()).c_str());
	}
    std::string local_mlog = szFileName;
    std::string local_dump = strLocalDumpName;
    std::string remote_mlog = remote_prefix + "_mlog.txt";
    std::string remote_dump = remote_prefix + ".dmp";

	std::vector<MCrashReportFilePair> vec;
	vec.push_back(MCrashReportFilePair(local_mlog, remote_mlog));
	vec.push_back(MCrashReportFilePair(local_dump, remote_dump));

	// config 등에서 읽어오지 않는 원본 상태
	reporter.Report(vec, false);
}
