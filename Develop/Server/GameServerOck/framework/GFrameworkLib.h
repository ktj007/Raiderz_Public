#pragma once

#pragma warning( disable : 4251 )

#ifndef GFRAMEWORK_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef GFRAMEWORK_EXPORT
// DLL library project uses this
#define GFRAMEWORK_API __declspec(dllexport)
#else
#ifdef GFRAMEWORK_IMPORT
// client of DLL uses this
#define GFRAMEWORK_API __declspec(dllimport)
#else
// static library project uses this
#define GFRAMEWORK_API
#endif
#endif // #ifdef GFRAMEWORK_EXPORT

#else
#define GFRAMEWORK_API
#endif // #ifdef _USRDLL

#endif