#pragma once

#pragma warning( disable : 4251 )

#ifndef MINET_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef MINET_EXPORT
// DLL library project uses this
#define MINET_API __declspec(dllexport)
#else
#ifdef MINET_IMPORT
// client of DLL uses this
#define MINET_API __declspec(dllimport)
#else
// static library project uses this
#define MINET_API
#endif
#endif // #ifdef MINET_EXPORT

#else
#define MINET_API
#endif // #ifdef _USRDLL

#endif