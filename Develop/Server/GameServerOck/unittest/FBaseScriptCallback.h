#pragma once

#include "GUTHelper.h"
#include "GTestWrapper.h"
#include "GTestForward.h"

class FBaseScriptCallback
{
public:
	FBaseScriptCallback();
	virtual ~FBaseScriptCallback();

	GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID);
	bool RunLuaString( const char* szFormat, ... );

public:
	GUTHelper		m_Helper;
	MockField*		m_pField;
	GFieldInfo*		m_pFieldInfo;
	GNPCInfo*		m_pNPCInfo;

	GTestSysWrapper<GScriptManager>				m_ScriptMgr;
	GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
	GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
	GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;	
	DECLWRAPPER_SimpleCombatCalc;	
	DECLWRAPPER_NPCMgr;
};
