#pragma once

#pragma once

#ifndef CORETOOL_PHYSX_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef CORETOOL_PHYSX_EXPORT
// DLL library project uses this
#define CORETOOL_PHYSX_API __declspec(dllexport)
#else
#ifdef CORETOOL_PHYSX_IMPORT
// client of DLL uses this
#define CORETOOL_PHYSX_API __declspec(dllimport)
#else
// static library project uses this
#define CORETOOL_PHYSX_EXPORT
#endif
#endif

#else
#define CORETOOL_PHYSX_API
#endif // #ifdef _USRDLL

#endif
