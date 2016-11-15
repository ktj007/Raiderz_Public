#include "StdAfx.h"
#include "ZGlue.h"
#include "ZApplication.h"
#include "RConfiguration.h"



ZGlue::ZGlue()
{
}


ZGlue::~ZGlue()
{
}


ZGlue* ZGlue::GetInstance()
{
	static ZGlue glue;
	return &glue;
}


void ZGlue::Log( const char* szLog)
{
	string str = szLog;
	str += "\n";
	OutputDebugStr( str.c_str());
}


void ZGlue::Reload()
{
	ZGetApplication()->Reload();
}


void ZGlue::ChangeResolution()
{
	static int resolution = 2;
	resolution = (resolution + 1) % 4;

	if ( resolution == 0)		ZGetApplication()->ChangeResolution( 800, 600);
	else if ( resolution == 1)	ZGetApplication()->ChangeResolution( 1024, 700);
	else if ( resolution == 2)	ZGetApplication()->ChangeResolution( 1024, 768);
	else if ( resolution == 3)	ZGetApplication()->ChangeResolution( 1280, 1024);
}


void ZGlue::Profile()
{
	MGetMint()->StartMintProfiler();
}


void ZGlue::Test()
{
}
