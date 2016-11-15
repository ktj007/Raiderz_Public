#include "StdAfx.h"
#include "GCrashReporter.h"
#include "GScriptManager.h"
#include "GConfig.h"
#include "GGlobal.h"
#include "SCrashReporter.h"
#include "GPathFinder.h"
#include "SAppHelper.h"
#include "GServerDumper.h"

void GCrashReporter::CrashDumpHandler( const char* szFileName )
{
	CrashLog();
	SCrashReporter::ExecuteBAReport(GConfig::m_strServerName.c_str(), MLocale::ConvAnsiToUCS2(szFileName).c_str(), true, GConfig::m_strDumpDestServerIP.c_str(), GConfig::m_nDumpDestServerPort);
}

void GCrashReporter::CrashLog()
{
	mlog("\n\n=== Crash Dump Log Begin ===\n");
	mlog("\t>> system\n");
	float fMemMB = SAppHelper::CalcMemorySize(::GetCurrentProcessId()) / 1024.0f / 1024.0f;
	mlog("\t\tmemory: %.2fMB\n", fMemMB); 

	mlog("\t>> lua callstack\n");
	if (gsys.pScriptManager)
	{
		MWLua* pLua = gsys.pScriptManager->GetLua();
		if (pLua)
		{
			ShowCallStack(pLua->GetState() , 0);	
		}
	}
	mlog("\n\t>> pathfinder info\n");
	if (gsys.pPathFinder)
	{
		mlog("\t\tzone_id: %d, pathfind pos(raw) (start: %d %d %d) (dest: %d %d %d)\n", 
			gsys.pPathFinder->TEST_PathfindZoneID,
			gsys.pPathFinder->TEST_PathfindStartRawPos.x, gsys.pPathFinder->TEST_PathfindStartRawPos.y, gsys.pPathFinder->TEST_PathfindStartRawPos.cell, 
			gsys.pPathFinder->TEST_PathfindEndRawPos.x, gsys.pPathFinder->TEST_PathfindEndRawPos.y, gsys.pPathFinder->TEST_PathfindEndRawPos.cell);
		mlog("\t\tzone_id: %d, pathfind pos(real) (start: %d %d %d) (dest: %d %d %d)\n", 
			gsys.pPathFinder->TEST_PathfindZoneID,
			gsys.pPathFinder->TEST_PathfindStartPos.x, gsys.pPathFinder->TEST_PathfindStartPos.y, gsys.pPathFinder->TEST_PathfindStartPos.cell, 
			gsys.pPathFinder->TEST_PathfindEndPos.x, gsys.pPathFinder->TEST_PathfindEndPos.y, gsys.pPathFinder->TEST_PathfindEndPos.cell);
	}
	
	mlog("\n\t>> server dumper\n");
	gsys.pServerDumper->Dump(L"CrashReporter", &g_DefaultLogger);

	mlog("\n=== Crash Dump Log Finished ===\n");

}