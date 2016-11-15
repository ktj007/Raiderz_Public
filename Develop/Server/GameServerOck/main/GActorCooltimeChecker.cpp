#include "stdafx.h"
#include "GActorCooltimeChecker.h"
#include "GModuleCombat.h"
#include "GCommandTable.h"
#include "GEntityPlayer.h"
#include "GTalentInfoMgr.h"
#include "GCommand.h"
#include "GTalent.h"
#include "GGlobal.h"
#include "GEntityActor.h"

////////////////////////////////////////////////////////////////
// CoolTime
////////////////////////////////////////////////////////////////

GActorCooltimeChecker::GActorCooltimeChecker( GEntityActor* pOwner )
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GActorCooltimeChecker::~GActorCooltimeChecker()
{
	m_pOwner->DetachObserver(this);
}

void GActorCooltimeChecker::UpdateCoolTime( MAP_COOLTIME& mapCoolTime, float fDelta )
{
	vector<int> vExpairedLines;
	for (MAP_COOLTIME::iterator itor = mapCoolTime.begin(); itor != mapCoolTime.end(); ++itor)
	{
		const int nTalentID		= (*itor).first;
		float& fCurTime		= (*itor).second.first;
		float& fLimitTime	= (*itor).second.second;

		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		DCHECK(pTalentInfo);
		if (!pTalentInfo)
			continue;

		if (pTalentInfo->IsToggle())
		{
			int nToggleBuffID = pTalentInfo->GetToggleBuffID();
			if (m_pOwner->HasBuff(nToggleBuffID))
				continue; // 토글 버프가 걸려있다면 쿨타임 무시
		}

		fCurTime += fDelta;

		if (fCurTime >= fLimitTime)
			vExpairedLines.push_back(nTalentID);
	}

	for each (int nTalentID in vExpairedLines)
	{
		mapCoolTime.erase(nTalentID);
		RouteEndTalentCooldown(m_pOwner, nTalentID);
	}
}

void GActorCooltimeChecker::Update( float fDelta )
{
	UpdateCoolTime(m_mapTalentID, fDelta);
}

bool GActorCooltimeChecker::IsCooldown( int nDelegateTalentID )
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nDelegateTalentID);
	if (!pTalentInfo)
		return false;

	vector<int> vecSameLineTalents = 
		GetRelativeTalentsID(pTalentInfo);
	
	for each (int each in vecSameLineTalents)
	{
		GTalentInfo* pEachTalentInfo = gmgr.pTalentInfoMgr->Find(each);
		if (!pEachTalentInfo)
			continue;	// 유효하지 않은 탤런트
		
		if (m_mapTalentID.find(each) == m_mapTalentID.end()) 
			continue;	// 쿨타임에 등록되지 않음

		if (pEachTalentInfo->IsToggle())
		{
			if (m_pOwner->HasBuff(pTalentInfo->GetToggleBuffID()))
				return false; // 토글 버프가 있다면 쿨타임 무시
		}

		return true; // 같은 탤런트라인의 탤런트가 쿨타임 중
	}

	return false;
}

void GActorCooltimeChecker::Clear()
{
	m_mapTalentID.clear();
}

void GActorCooltimeChecker::RouteEndTalentCooldown( GEntityActor* pOwner, int nDelegateTalentID )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_END_TALENT_COOLDOWN, 1, NEW_INT(nDelegateTalentID));
	pOwner->RouteToMe(pNewCommand);
}

void GActorCooltimeChecker::GetTalentRemainCoolTime(vector<pair<int, float>>& outvecTalentRemainCoolTime)
{
	for each (pair<int, pair<float, float>> each in m_mapTalentID)
	{
		int nTalentID = each.first;
		float fElapsedCoolTime = each.second.first;
		float fCoolTime = each.second.second;
		float fRemainCoolTime = fCoolTime - fElapsedCoolTime;

		if (1.0f >= fRemainCoolTime)
			continue;

		outvecTalentRemainCoolTime.push_back(pair<int, float>(nTalentID, fRemainCoolTime));
	}
}

void GActorCooltimeChecker::InsertTalentCooltime( int nDelegateTalentID, float fLimitTime )
{
	const float COOLTIME_TOLER = 0.0f;

	if (fLimitTime <= COOLTIME_TOLER)		return;
	fLimitTime -= COOLTIME_TOLER;

	m_mapTalentID.insert(MAP_COOLTIME::value_type(nDelegateTalentID, pair<float,float>(0.0f, fLimitTime)));
}

void GActorCooltimeChecker::InsertTalentRemainCoolTime(const vector<pair<int, float>>& vecTalentRemainCoolTime)
{
	for each (pair<int, float> each in vecTalentRemainCoolTime)
	{
		int nTalentID = each.first;
		float fRemainCoolTime = each.second;

		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		if (NULL == pTalentInfo) continue;

		float fElapsedCoolTime = pTalentInfo->m_fCoolTime - fRemainCoolTime;

		m_mapTalentID.insert(MAP_COOLTIME::value_type(nTalentID, pair<float,float>(fElapsedCoolTime, pTalentInfo->m_fCoolTime)));
	}
}

void GActorCooltimeChecker::OnEnterPhaseAct( GTalent* pTalent )
{
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);

	InsertTalentCooltime(pTalent->GetID(), pTalentInfo->m_fCoolTime);
}

vector<string> GActorCooltimeChecker::GetDescription() const
{
	vector<string> ret;
	for (MAP_COOLTIME::const_iterator iter = m_mapTalentID.begin(); iter != m_mapTalentID.end(); ++iter)
	{
		const int nID				= iter->first;
		pair<float, float> Data		= iter->second;

		char buff[512];
		sprintf(buff, "%d = %.1f/%.1f", nID, Data.first, Data.second);
		ret.push_back(buff);
	}

	return ret;
}

vector<int> GActorCooltimeChecker::GetRelativeTalentsID(GTalentInfo* pTalentInfo)
{
	vector<int> ret;
	if (!pTalentInfo)
		return ret;

	ret = gmgr.pTalentInfoMgr->GetLineTalents(pTalentInfo->m_nTalentLine);
	if (pTalentInfo->m_nCooltimeLine != INVALID_ID)
	{
		vector<int> vecSameCooltimeLineTalents = gmgr.pTalentInfoMgr->GetCooltimeLineTalents(pTalentInfo->m_nCooltimeLine);
		std::copy(vecSameCooltimeLineTalents.begin(), vecSameCooltimeLineTalents.end(), std::back_inserter(ret));
		sort(ret.begin(), ret.end());
		ret.erase(unique(ret.begin(), ret.end()), ret.end());
	}

	return ret;
}
