// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ¿©±â´Ù include
#define _CRT_SECURE_NO_DEPRECATE

#ifdef WIN32
#include <winsock2.h>
#include <Windows.h>
#endif

#include <crtdbg.h>

#include <stdio.h>
#include <list>
#include <map>
#include <tchar.h>
#include <io.h>
#include <memory.h>

#define TIXML_USE_STL

#ifdef _DEBUG
#pragma comment(lib, "atlsd.lib")
#else
#pragma comment(lib, "atls.lib")

#endif


#endif