// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)		// sprintf_s ...
#pragma warning (disable : 4244)	// warning C4244: '=' : 'int'에서 'float'(으)로 변환하면서 데이터가 손실될 수 있습니다.
#pragma warning (disable : 4305)	// warning C4305: '인수' : 'double'에서 'float'(으)로 잘립니다.
#pragma warning (disable : 4267)	// warning C4267: '=' : 'size_t'에서 'int'(으)로 변환하면서 데이터가 손실될 수 있습니다.

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.
#include <windows.h>
// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <algorithm>
#include <cctype>
using namespace std;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <windows.h>
#include <mmsystem.h>

#include "XAntiHack.h"				// Anti-hack module
#include "XAntiHackModuleGameGuardKor.h"

#include "MUID.h"
#include "MRTTI.h"
#include "MFileSystem.h"
#include "MWLua.h"
#include "MUtil.h"
#include "MDebug.h"
#include "MBlobArray.h"
#include "FileInfo.h"
#include "MMemPool.h"
#include "MProfiler.h"
#include "MStatisticsProfiler.h"
#include "MCodeCoverage.h"

#include "CSDef.h"
#include "CTransData.h"

#include "RTypes.h"
#include "RFont.h"
#include "RRenderHelper.h"
#include "RApplication.h"
#include "RProfileInfo.h"
#include "Mint.h"
#include "Mint4R3.h"
#include "MWidget.h"
#include "MTime.h"

#include "UnitTest++.h"
#include "MTest.h"

#include "XScreenDebugger.h"
#include "XUtil.h"
#include "XGlobal.h"
#include "XGlobalVar.h"
#include "XCommandTable.h"
#include "XNetClient.h"
#include "XScript.h"
#include "XVirtualKeyDef.h"
#include "XKeyCodes.h"
#include "XKeyMapTable.h"
#include "XGameInput.h"
#include "XChatCmds.h"
#include "XHelloWorldFrame.h"
#include "XTestFrame.h"
#include "XStrCmd.h"
#include "XTimer.h"
#include "XItemData.h"
#include "XRenderHelper.h"
#include "Mint4Game.h"
#include "XConfig.h"
#include "XQuestInfo.h"
#include "XTerrain.h"
#include "XWorldEnv.h"
#include "XRealSound.h"
#include "XEvent.h"
#include "XModule.h"
#include "XActor.h"
#include "XNetPlayer.h"
#include "XUISystem.h"
#include "XCollision.h"
#include "XMath.h"
#include "XSTLUtil.h"
#include "XObjectManager.h"
#include "XMotionMgr.h"
#include "MLocale.h"
#include "CSFormatString.h"

using namespace rs3;
using namespace mint3;
using namespace minet;
//using namespace physx;


#pragma warning( disable : 4204 )

