#ifndef _MOCK_ENTITYNPC_H_
#define _MOCK_ENTITYNPC_H_

#include "GEntityNPC.h"
#include "GNPCInfo.h"
#include "GModuleAI.h"

class MockEntityNPC : public GEntityNPC
{
protected:
	GNPCInfo		m_NPCInfo;

public:
	using GEntityNPC::m_LastHitinfo;

	MockEntityNPC():m_nYellRange(0) 
	{
		m_NPCInfo.nID = 1;
		m_NPCInfo.nInitAttackable = NAT_NONE;
		SetNPCInfo(&m_NPCInfo);
		GetModuleAI()->Init();
	}
	virtual ~MockEntityNPC() {}

	virtual bool SetPos(vec3& vPos)
	{
		return GEntity::SetPos(vPos);
	}

	virtual void OnHandle_NoNPCInfo()
	{
		m_ChrStatus.nHP = 100;
		m_ChrStatus.nEN = 100;
		m_ChrStatus.nSTA = 100;
	}

	void SetTypeID(ENTITY_TYPEID type)		{ m_nTypeID = type; }

	virtual void RouteMove(vec3& tarPos, float fSpeed, bool bRun) {}
	virtual void RouteMoveStop() {}
	virtual void RouteChangeCapsule()	{}

	// 대화 관련
	wstring m_strLastSayText;
	wstring m_strLastSayType;
	virtual void doSay(const wchar_t* szText)
	{
		__super::doSay(szText);
		m_strLastSayType = L"SAY";
		m_strLastSayText = szText;
	}
	virtual void doShout( const wchar_t* szText )
	{
		__super::doShout(szText);
		m_strLastSayType = L"SHOUT";
		m_strLastSayText = szText;
	}
	virtual void doNarration( const wchar_t* szText )
	{
		__super::doNarration(szText);
		m_strLastSayType = L"NARRATION";
		m_strLastSayText = szText;
	}


	int m_nYellRange;
	virtual void doYell(int nRange=0) 
	{
		m_nYellRange = nRange;
	}

	void SetSpawnInfo(SPAWN_INFO* pSpawnInfo)
	{
		m_pSpawnInfo = pSpawnInfo;
	}
};

#endif //_MOCK_ENTITYNPC_H_