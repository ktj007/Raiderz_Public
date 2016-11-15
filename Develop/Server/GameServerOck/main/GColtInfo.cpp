#include "stdafx.h"
#include "GColtInfo.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GColtDef.h"

struct ColtClear
{
	template<typename T>
	void operator() (T* ptr) const
	{
		ptr->Clear();
		SAFE_DELETE(ptr);
	}
};



GColtAction::GColtAction()
: nType(CAT_NONE)
, nRate(0)
, nMaxCount(INVALID_AI_ACTION_COUNT)
, bAutoRun(false)
, fCycleTime(0.0f)
, fDurationTime(JOB_DURATION_INFINITE_TIME)
, bGainStress(true)
, bModifyDurationTime(false)
{ 
	qParams.assign(MAX_AI_INFO_PARAM, L""); 
}

bool GColtAction::IsPickable( const GColtActionInst& inst ) const
{
	if (IsExpiredCount(inst))
		return false;	// 최대 사용횟수 초과

	if (!inst.IsEnableCycleTime())
		return false;	//대기시간 있음

	return true;
}

bool GColtAction::IsExpiredCount( const GColtActionInst& inst ) const
{
	if (nMaxCount == INVALID_AI_ACTION_COUNT)
		return false;	// 최대 사용횟수가 지정되어있음

	return (nMaxCount <= inst.GetUseCount()); // 최대 사용횟수를 넘었음
}







GColtCheck::GColtCheck()
: nType(AST_NONE)
, pCheckDefault(NULL)
, nPickRate(MAX_AI_RATE)
{
	vecParams.assign(MAX_AI_INFO_PARAM, pair<bool,int>(false,0)); 
}

GColtCheck::~GColtCheck() 
{ 
	
}

void GColtCheck::Clear()
{
	nType = AST_NONE;

	vecParams.clear();
	vecActions.clear();
	vecAutoRunActions.clear();

	for_each(vecChecks.begin(), vecChecks.end(), ColtClear());
	vecChecks.clear();

	SAFE_DELETE(pCheckDefault);
}

bool GColtCheck::CheckActionConditions() const
{
	if (!vecAutoRunActions.empty())	
		return true; // 오토런이 있음
	if (vecActions.empty())			
		return false; // 액션이 없음

	return true;
}

bool GColtCheck::HasInnerChecks() const
{
	return !vecChecks.empty();
}

bool GColtCheck::HasActions() const
{
	if (!vecActions.empty())
		return true;

	if (!vecAutoRunActions.empty())
		return true;

	return false;
}

bool GColtCheck::HasAutoActions() const
{
	return !vecAutoRunActions.empty();
}

void GColtCheck::Copy(GColtCheck& new_check) const
{
	new_check = *this;
	if (this->pCheckDefault)
	{
		new_check.pCheckDefault = new GColtCheck;
		*new_check.pCheckDefault = *this->pCheckDefault;
	}

	new_check.vecChecks.clear();
	for each (GColtCheck* each in vecChecks)
	{
		GColtCheck* check = new GColtCheck;
		*check = *each;
		new_check.vecChecks.push_back(check); 
	}
	
}



GColtGroupInfo::CombatAttr::CombatAttr()
: nDownAttackTalentID(INVALID_TALENT_ID)
, nVictoryTalentID(INVALID_TALENT_ID)
{	
}

void GColtGroupInfo::CombatAttr::Clear()
{
	nDownAttackTalentID = INVALID_TALENT_ID;
	nVictoryTalentID = INVALID_TALENT_ID;
}

GColtGroupInfo::GColtGroupInfo()
{ 
}

GColtGroupInfo::~GColtGroupInfo()	
{ 
	Clear();
}

void GColtGroupInfo::_EachClear( vector<GColtCheck*>& vecCheckinfos )
{
	for_each(vecCheckinfos.begin(), vecCheckinfos.end(), ColtClear());
	vecCheckinfos.clear();	
}

void GColtGroupInfo::_EachClone( const vector<GColtCheck*>& source, vector<GColtCheck*>& dest )
{
	dest.clear();
	for each (const GColtCheck* each in source)
	{
		GColtCheck* check = new GColtCheck();
		each->Copy(*check);
		dest.push_back(check);
	}
}

void GColtGroupInfo::Clear()
{
	for (int i=0; i<NPC_MODE_MAX; ++i)
	{
		attrCombat[i].Clear();
		_EachClear(vecCombatChecks[i]);
		checkCombatDefault[i].Clear();
	}
	_EachClear(vecReactionChecks);
	_EachClear(vecAggroChecks);
	_EachClear(vecStressChecks);
	_EachClear(vecIdleChecks);
	_EachClear(vecPrologue);
	_EachClear(vecEpilogue);
		
	checkIdleDefault.Clear();
	checkAggroDefault.Clear();
}

void GColtGroupInfo::Copy(GColtGroupInfo& new_info)
{
	new_info = *this;
	for (int i=0; i<NPC_MODE_MAX; ++i)
	{
		_EachClone(new_info.vecCombatChecks[i], vecCombatChecks[i]);
	}
	_EachClone(new_info.vecReactionChecks, vecIdleChecks);
	_EachClone(new_info.vecAggroChecks, vecIdleChecks);
	_EachClone(new_info.vecStressChecks, vecIdleChecks);
	_EachClone(new_info.vecIdleChecks, vecIdleChecks);
	_EachClone(new_info.vecPrologue, vecIdleChecks);
	_EachClone(new_info.vecEpilogue, vecIdleChecks);
}








GColtActionInst::GColtActionInst() 
: m_nModifiedRate(0)
, m_nCurCount(0)
, m_dwCycleTime(0)
, m_fDurationTime(JOB_DURATION_INFINITE_TIME)
, m_bModifyDurationTime(false)
, m_bGainStress(true)
{

}

GColtActionInst::GColtActionInst(const GColtAction& action) 
: m_nModifiedRate(0)
, m_nCurCount(0)
, m_dwCycleTime(0)
, m_fDurationTime(action.fDurationTime)
, m_bModifyDurationTime(action.bModifyDurationTime)
, m_bGainStress(action.bGainStress)
{
	SetEnableCycleTime();

	for (size_t j=0; j<action.vecSeqActions.size(); j++)
	{
		const GColtAction& eachInnerAction = action.vecSeqActions[j];
		m_vecInnerInfos.push_back(GColtActionInst(eachInnerAction));
	}
}

bool GColtActionInst::IsEnableCycleTime() const
{
	DWORD dwNowTime = gsys.pSystem->GetNowTime();
	return m_dwCycleTime <= dwNowTime;
}

void GColtActionInst::SetEnableCycleTime()
{
	m_dwCycleTime	= gsys.pSystem->GetNowTime();
}

void GColtActionInst::ResetCycleTime( float fDelaySecond )
{
	m_dwCycleTime	= gsys.pSystem->GetNowTime() + (DWORD)(fDelaySecond * 1000);
}

bool GColtActionInst::IsInfinityTime()
{
	if (!m_bModifyDurationTime)	return false;
	return m_fDurationTime == -1.0f;
}

void GColtActionInst::IncreaseUseCount()
{
	++m_nCurCount;
}

int GColtActionInst::GetUseCount() const
{
	return m_nCurCount;
}

bool GColtActionInst::operator==( const GColtActionInst& other ) const
{
	return m_nModifiedRate == other.m_nModifiedRate &&
		m_nCurCount == other.m_nCurCount &&
		m_fDurationTime == other.m_fDurationTime &&
		m_bGainStress == other.m_bGainStress;
}

int GColtActionInst::GetRate() const
{
	return m_nModifiedRate;
}

vector<GColtActionInst>& GColtActionInst::GetInnerInfos()
{
	return m_vecInnerInfos;
}

float GColtActionInst::GetDurationTime() const
{
	return m_fDurationTime;
}

void GColtActionInst::SetRate( int nRate )
{
	m_nModifiedRate = nRate;
}

bool GColtActionInst::IsGainStress() const
{
	return m_bGainStress;
}

bool GColtActionInst::IsModifiedDurationTime() const
{
	return m_bModifyDurationTime;
}

DWORD GColtActionInst::GetCycleTime() const
{
	return m_dwCycleTime;
}