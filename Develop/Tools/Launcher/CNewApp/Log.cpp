/******************************************************************
   
   Log.cpp

     Corperation : MAIET entertainment
     Programmer  : Lim Dong Hwan
	 Date        : 22.June.2005

*******************************************************************/


#include "stdafx.h"
#include "Log.h"
#include <sys/timeb.h>


void InitLog()
{
	FILE* pFile;
	fopen_s( &pFile, "patchlog.txt", "w");
	if ( pFile == NULL)
		return;


	__time64_t ltime;
	_time64( &ltime);
	char buff[ 128];
	_ctime64_s( buff, 128, &ltime);
	fprintf_s( pFile, "Log start : %s\n", buff);


	char szDir[ 512];
	GetCurrentDirectory( sizeof( szDir), szDir);
	fprintf( pFile, "Current Directory : %s\n", szDir);

	fclose( pFile);
}


void PutLog( const char* pszLog)
{
	FILE* pFile;
	fopen_s( &pFile, "patchlog.txt", "a");
	if ( pFile == NULL)
		return;

	char szBuff[128];
	_strtime_s( szBuff);
	struct __timeb64 tstruct;
	_ftime64_s( &tstruct);
	fprintf( pFile, "%s:%03d   ", szBuff, tstruct.millitm);

	fprintf( pFile, pszLog);
	fprintf( pFile, "\n");

	fclose( pFile);
}
