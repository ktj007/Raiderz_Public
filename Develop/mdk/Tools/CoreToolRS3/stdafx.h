// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#include "CorePrecompiledHeader.h"

#include "IL/il.h"
#include "IL/ilu.h"

template<class T>
inline bool ToBool(T boolValue)
{
	return boolValue != 0;
}


#include <MXml.h>
#include <tchar.h>
#include "resource.h"
#include <afxdhtml.h>

#include "CoreTypes.h"
#include "CoreToolLib.h"

#include "C_ControlMediator.h"
#include "MDebug.h"

#include "RealSpace3.h"
#include "RTypes.h"

#define FILENAME_CONFIG		"config.xml"


//#define _DEBUG_RED_ALERT
