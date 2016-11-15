#pragma once

#pragma warning( disable : 4251 )

#ifndef MADB_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef MADB_EXPORT
// DLL library project uses this
#define MADB_API __declspec(dllexport)
#else
#ifdef MADB_IMPORT
// client of DLL uses this
#define MADB_API __declspec(dllimport)
#else
// static library project uses this
#define MADB_API
#endif
#endif // #ifdef MADB_EXPORT

#else
#define MADB_API
#endif // #ifdef _USRDLL

#endif