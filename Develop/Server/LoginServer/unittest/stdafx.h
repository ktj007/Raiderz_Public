// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.

#include <atlstr.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <string>

#include "MDatabase.h"

#include <Winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>

#include "UnitTest++.h"

#include "MCommandDesc.h"
#include "MCommandParameter.h"

#include "STypes.h"

#include "MDebug.h"
#include "MXml.h"
#include "MTime.h"

#include "SUnitTest.h"

#include "MTstring.h"

using namespace minet;

// 정밀한 릭체크가 필요하면 디파인합니다.
#ifdef _DEBUG
#define VLD_CHECK_LEAK		
#endif


#pragma comment(lib,"winmm.lib")
