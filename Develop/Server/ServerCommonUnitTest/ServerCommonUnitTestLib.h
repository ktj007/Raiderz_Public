#pragma once

#pragma warning( disable : 4251 )

#ifndef SCOMMON_UNITTEST_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef SCOMMON_UNITTEST_EXPORT
// DLL library project uses this
#define SCOMMON_UNITTEST_API __declspec(dllexport)
#else
#ifdef SCOMMON_UNITTEST_IMPORT
// client of DLL uses this
#define SCOMMON_UNITTEST_API __declspec(dllimport)
#else
// static library project uses this
#define SCOMMON_UNITTEST_API
#endif
#endif // #ifdef SCOMMON_UNITTEST_EXPORT

#else
#define SCOMMON_UNITTEST_API
#endif // #ifdef _USRDLL

#endif
