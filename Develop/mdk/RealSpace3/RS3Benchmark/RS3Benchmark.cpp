#include "stdafx.h"
#include "RS3Benchmark.h"
#include "RBenchmarkApp.h"

rs3::RBenchmarkApp g_App;

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MInitLog( MLOG_LEVEL_DEBUG, MLOG_FILE | MLOG_DEBUG );
	g_App.Run();
}