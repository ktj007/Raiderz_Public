// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.
#include <windows.h>
// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>

// cml
#include "MTypes.h"
#include "MDebug.h"
#include "MThread.h"
#include "MUID.h"

#include "MXml.h"

#include "Tdef.h"

#include "QuestForm.h"
#include "DialogForm.h"

using namespace QuestAndDialog_NET;
using namespace System::Data::OleDb;

ref class TQuestControllerGlobal
{
public:
	static QuestForm^			m_QuestForm;
	static DataGridViewCell^	m_dgvcSelected;
	static Object^				m_dgvcPrevSelectedValue;
};

ref class TDialogControllerGlobal
{
public:
	static DialogForm^		m_DialogForm;
	static TreeNode^		m_tnPreSelectedDialogList;
};


ref class TDBManagerGlobal
{
public:
	static DataSet^ m_dsQuest;
};

ref class TDatabaseGlobal
{
public:
	static OleDbConnection^	m_DBConnection;
};





// Leak 체크 Define ----------------------------------------------
// - 정밀하게 Leak 체크할 때 VLD_CHECK_LEAK를 디파인합니다.
//#define VLD_CHECK_LEAK
// - 빠른 Leak 체크할 때 M_CHECK_LEAK를 디파인합니다.
#define M_CHECK_LEAK

#ifdef _DEBUG
#ifdef VLD_CHECK_LEAK
#include "../../../sdk/vld/vld.h"
#pragma comment(lib,"vldmt.lib")
#else
#ifdef M_CHECK_LEAK
#include "MMemoryLeak.h"
#endif
#endif
#endif // _DEBUG
// Leak 체크 Define ----------------------------------------------

#pragma comment(lib, "../../../sdk/zlib/lib/zlib.lib")
#pragma comment(lib,"winmm.lib")