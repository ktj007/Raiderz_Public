#include "stdafx.h"
#include "RSoundError.h"
#include "fmod.h"
#include "fmod_errors.h"
#include <stdio.h>
#include <stdarg.h>
#include "MDebug.h"



// _ERROR
bool _ERROR( int _err)
{
	return ( ((FMOD_RESULT)_err == FMOD_OK) ? false : true);
}


// _ERRORMSG
void _ERRORMSG( int _err, const char* _str)
{
#ifndef _DEBUG
	if ( _err == FMOD_ERR_FILE_NOTFOUND)	return;
#endif

	char _buff[ 512];
#if _MSC_VER < 1400
	if ( _str != NULL)	sprintf( _buff, "[REALSOUND3] - [ERROR] %s (%s)\n", FMOD_ErrorString( (FMOD_RESULT)_err), _str);
	else				sprintf( _buff, "[REALSOUND3] - [ERROR] %s\n", FMOD_ErrorString( (FMOD_RESULT)_err));
#else
	if ( _str != NULL)	sprintf_s( _buff, "[REALSOUND3] - [ERROR] %s (%s)\n", FMOD_ErrorString( (FMOD_RESULT)_err), _str);
	else				sprintf_s( _buff, "[REALSOUND3] - [ERROR] %s\n", FMOD_ErrorString( (FMOD_RESULT)_err));
#endif

//	::OutputDebugString( _buff);
	mlog( _buff);
}


// _MESSAGE
void _MESSAGE( const char* szFormat, ...)
{
	va_list argList;
	va_start( argList, szFormat);

	char _buff[ 512];
#if _MSC_VER < 1400
	vsprintf( _buff, szFormat, argList);
#else
	vsprintf_s( _buff, szFormat, argList);
#endif
	va_end( argList);

	char _str[ 512];
#if _MSC_VER < 1400
	sprintf( _str, "[REALSOUND3] %s\n", _buff);
#else
	sprintf_s( _str, "[REALSOUND3] %s\n", _buff);
#endif

//	::OutputDebugString( _str);
	mlog( _str);
}
