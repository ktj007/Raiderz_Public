// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifdef WIN32
//#define _WIN32_WINNT 0x0500 //XP 이상

#include <winsock2.h>
#include <Windows.h>
#endif

#include <iostream>
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>

#include <winuser.h>
#include "Shlwapi.h"

#include <crtdbg.h>
#include <list>
#include <map>
#include "MDebug.h"

#include "UnitTest++.h"
#include "TestReporter.h"

#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RUnitTestRunner.h"

using namespace rs3;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
