#include "stdafx.h"
#include "XApp.h"
#include "MDebug.h"
#include <shlwapi.h>

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_DEBUG | MLOG_FILE, NULL, "TalentExporter.txt");

	XApp app;

	// Current Directory¸¦ ¸ÂÃá´Ù.
	TCHAR szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

	app.Run();

	return 0;
}



