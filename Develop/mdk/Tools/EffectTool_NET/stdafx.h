// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
#pragma once

#pragma warning ( disable : 4146)
#pragma warning ( disable : 4793)
#pragma warning ( disable : 4996)

#using <mscorlib.dll>

#include <windows.h>

#include <atlstr.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include "MXml.h"
//#include "Global.h"

#include "RActorNode.h"
#include "RMeshNode.h"
#include "MDebug.h"
#include "MFileSystem.h"

// Leak 체크 Define ----------------------------------------------
// - 정밀하게 Leak 체크할 때 VLD_CHECK_LEAK를 디파인합니다.
//#define VLD_CHECK_LEAK
// - 빠른 Leak 체크할 때 M_CHECK_LEAK를 디파인합니다.
// #define M_CHECK_LEAK
// 
// #ifdef _DEBUG
// #ifdef VLD_CHECK_LEAK
// #include "../../../sdk/vld/vld.h"
// #pragma comment(lib,"vldmt.lib")
// #else
// #ifdef M_CHECK_LEAK
// #include "MMemoryLeak.h"
// #endif
// #endif
// #endif // _DEBUG
// // Leak 체크 Define ----------------------------------------------


#ifdef _DEBUG
#pragma comment(lib, "atlsd.lib")
#else
#pragma comment(lib, "atls.lib")
#endif
