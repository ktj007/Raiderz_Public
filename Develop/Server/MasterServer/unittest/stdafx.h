// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.

// atl
#include <atlstr.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <unordered_map>

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


#include "gmock/gmock.h"

// googlemock와 충돌나서 undef
#ifdef TEST
#undef TEST		
#endif

#include "UnitTest++.h"


// 정밀한 릭체크가 필요하면 디파인합니다.
#ifdef _DEBUG
#define VLD_CHECK_LEAK		
#endif

#if defined(VLD_CHECK_LEAK)

#define VLD_MAX_DATA_DUMP		0xFF		// vld에서 데이터 덤프는 FF Byte만 해랑.

#include "../../../sdk/vld/vld.h"
#pragma comment(lib,"vldmt.lib")

#endif


#pragma comment(lib,"winmm.lib")

// cml
#include "MTypes.h"
#include "MDebug.h"
#include "MMath.h"
#include "MThread.h"
#include "MUID.h"
#include "MXml.h"
#include "MSync.h"
#include "MCrashDump.h"
#include "MFileSystem.h"
#include "MTime.h"
#include "MCodeCoverage.h"

// minet
#include "MiNet.h"
#include "MCommand.h"
#include "MCommandParameter.h"
#include "MCommandCommunicator.h"
#include "MCommandManager.h"
#include "MCommandDesc.h"
#include "MiNetHelper.h"
#include "MiNetCommon.h"

// CSCommon
#include "CCommandTable.h"
#include "CCommandResultTable.h"
#include "CStringRes.h"

using namespace minet;

// ServerCommon
#include "STypes.h"

// MasterServer_unittest
#include "ZTestWrapper.h"
#include "ZUnitTestStage1.h"
#include "MockDBManager.h"
#include "MockLink.h"
#include "MockServer.h"
#include "ZUnitTestRunner.h"