#include "stdafx.h"
#include "XGlueDebugFunction.h"
#include "XPost_Local.h"

void XGlueDebugFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueDebugFunction>(pLua->GetState(), "_gamedebug")

		.def("AddLogFilter",			&XGlueDebugFunction::AddLogFilter)
		.def("Console",					&XGlueDebugFunction::Console)
		.def("ConsoleCmd",				&XGlueDebugFunction::ConsoleCmd)
		.def("Log",						&XGlueDebugFunction::Log)
		;
}

void XGlueDebugFunction::AddLogFilter( const char* szFilter )
{
#ifndef _PUBLISH
	MAddLogFilter(szFilter);
#endif
}

void XGlueDebugFunction::Console( const char* szCommand )
{
#ifndef _PUBLISH

	if (global.ui)	global.ui->InputConsoleCmd(MLocale::ConvAnsiToUTF16(szCommand).c_str());
#endif
}

void XGlueDebugFunction::ConsoleCmd( const char* szCmd )
{
#ifndef _PUBLISH

	if (global.ui)	global.ui->InputConsoleCmd(MLocale::ConvAnsiToUTF16(szCmd).c_str());
#endif
}

void XGlueDebugFunction::Log(const char* szMsg)
{
#ifndef _PUBLISH
	mlog("script : %s\n", szMsg);
#endif
}

