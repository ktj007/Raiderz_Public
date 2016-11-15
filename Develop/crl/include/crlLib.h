#pragma once

#pragma warning( disable : 4251 )

#ifndef CRL_API

/////////////////////////////////////////////////////////////////////
// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef CRL_EXPORT
// DLL library project uses this
#define CRL_API __declspec(dllexport)
#else
#ifdef CRL_IMPORT
// client of DLL uses this
#define CRL_API __declspec(dllimport)
#else
// static library project uses this
#define CRL_API
#endif
#endif // #ifdef CRL_EXPORT

#else
#define CRL_API
#endif // #ifdef _USRDLL

namespace crl {

}

#endif