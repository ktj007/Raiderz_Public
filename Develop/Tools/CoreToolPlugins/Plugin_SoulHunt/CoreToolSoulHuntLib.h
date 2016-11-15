#pragma once

#ifndef CORETOOL_SOULHUNT_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef CORETOOL_SOULHUNT_EXPORT
// DLL library project uses this
#define CORETOOL_SOULHUNT_API __declspec(dllexport)
#else
#ifdef CORETOOL_SOULHUNT_IMPORT
// client of DLL uses this
#define CORETOOL_SOULHUNT_API __declspec(dllimport)
#else
// static library project uses this
#define CORETOOL_SOULHUNT_API
#endif
#endif // #ifdef REALSPACE3_EXPORT

#else
#define CORETOOL_API
#endif // #ifdef _USRDLL

#endif
