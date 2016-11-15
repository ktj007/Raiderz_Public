#pragma once

#pragma warning( disable : 4251 )

#ifndef CSCOMMON_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef CSCOMMON_EXPORT
// DLL library project uses this
#define CSCOMMON_API __declspec(dllexport)
#else
#ifdef CSCOMMON_IMPORT
// client of DLL uses this
#define CSCOMMON_API __declspec(dllimport)
#else
// static library project uses this
#define CSCOMMON_API
#endif
#endif // #ifdef CSCOMMON_EXPORT

#else
#define CSCOMMON_API
#endif // #ifdef _USRDLL

#endif