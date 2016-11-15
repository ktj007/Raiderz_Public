// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

// window 98 이상
#define _WIN32_WINNT 0x0500
#define WINVER 0x0410

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

//#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)		// sprintf_s ...
#pragma warning (disable : 4244)	// warning C4244: '=' : 'int'에서 'float'(으)로 변환하면서 데이터가 손실될 수 있습니다.
#pragma warning (disable : 4305)	// warning C4305: '인수' : 'double'에서 'float'(으)로 잘립니다.
#pragma warning (disable : 4267)	// warning C4267: '=' : 'size_t'에서 'int'(으)로 변환하면서 데이터가 손실될 수 있습니다.


//#define _SECURE_SCL 0
//#define _HAS_ITERATOR_DEBUGGING 0

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <cstdio>
#include <cmath>
#include <tchar.h>

#include <list>
#include <vector>
#include <map>
#include <hash_map>
#include <set>
#include <string>
#include <algorithm>
#include <functional>

// BVH 카메라를 사용할거냐 말거냐
#define BVH_CAMERA

#include "RPreDefine.h"

// win32 베이스
#include <windows.h>
#include "crtdbg.h"

// 엔진 베이스
#include "MXml.h"
#include "mempool.h"
#include "MFileSystem.h"
#include "MRTTI.h"
#include "MSingleton.h"
#include "MProfiler.h"

#include "RTypes.h"
#include "RTimer.h"
#include "RConfiguration.h"
#include "RProfileInfo.h"
#include "REngine.h"
#include "RProfileInfo.h"

#include "RResource.h"
#include "RResourceManager.h"
#include "RD3DVertexUtil.h"
#include "RTechnique.h"

#include "RSceneNode.h"
#include "RCameraSceneNode.h"
#include "RSceneManager.h"
// 엔진 / 장치
#include "RDevice.h"

// 기본 Application
#include "RApplication.h"

//프로파일러
#include "RProfiler.h"

#ifdef _DEBUG
#pragma comment(lib, "atlsd.lib")
#else
#pragma comment(lib, "atls.lib")
#endif
