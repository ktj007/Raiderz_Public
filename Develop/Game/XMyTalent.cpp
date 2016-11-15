#include "stdafx.h"
#include "XMyTalent.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfo.h"

void XMyPassiveTalent::GainEffect(XTalentInfo* pTalentInfo)
{
	m_ModEffector.AddActorModifier(gvar.MyInfo.ChrStatus.ActorModifier, pTalentInfo->m_ActorModifier);
	m_ModEffector.AddPlayerModifier(gvar.MyInfo.PlayerModifier, pTalentInfo->m_PlayerModifier);

	if (pTalentInfo->m_nSkillType == ST_PASSIVE || pTalentInfo->m_nSkillType == ST_EXTRA_PASSIVE)
	{
		if (pTalentInfo->m_nExtraPassive > TEPT_NONE && pTalentInfo->m_nExtraPassive < TEPT_MAX)
		{
			m_bHasExtraPassiveTalent[pTalentInfo->m_nExtraPassive] = true;
			m_nExtraPassiveTalentLevel[pTalentInfo->m_nExtraPassive]++;
		}
	}
}

void XMyPassiveTalent::LostEffect( XTalentInfo* pTalentInfo)
{
	m_ModEffector.EraseActorModifier(gvar.MyInfo.ChrStatus.ActorModifier, pTalentInfo->m_ActorModifier);
	m_ModEffector.ErasePlayerModifier(gvar.MyInfo.PlayerModifier, pTalentInfo->m_PlayerModifier);

	if (pTalentInfo->m_nSkillType == ST_PASSIVE || pTalentInfo->m_nSkillType == ST_EXTRA_PASSIVE)
	{
		if (pTalentInfo->m_nExtraPassive > TEPT_NONE && pTalentInfo->m_nExtraPassive < TEPT_MAX)
		{
			m_bHasExtraPassiveTalent[pTalentInfo->m_nExtraPassive] = false;
			m_nExtraPassiveTalentLevel[pTalentInfo->m_nExtraPassive]--;
			if (m_nExtraPassiveTalentLevel[pTalentInfo->m_nExtraPassive] < 0) m_nExtraPassiveTalentLevel[pTalentInfo->m_nExtraPassive] = 0;
		}
	}
}

XMyPassiveTalent::XMyPassiveTalent()
{
	for (int i = 0; i < TEPT_MAX; i++) 
	{
		m_bHasExtraPassiveTalent[i] = false;
		m_nExtraPassiveTalentLevel[i] = 0;

		if (CSTalentInfoHelper::IsInitialLearnedExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE(i)))
		{
			m_bHasExtraPassiveTalent[i] = true;
		}
	}
}

void XMyTalent::GainInstantEffect(XTalentInfo* pTalentInfo)
{
	_ASSERT(NULL != pTalentInfo && "pTalentInfo is NULL");
	if (pTalentInfo == NULL)	return;

	m_ModEffector.ApplyInstantModidier(pTalentInfo->m_InstantModifier);
}

void XMyTalent::GainMaintainEffect(XTalentInfo* pTalentInfo)
{
	_ASSERT(NULL != pTalentInfo && "pTalentInfo is NULL");
	if (pTalentInfo == NULL)	return;

	m_Passive.GainEffect(pTalentInfo);
}

void XMyTalent::LostMaintainEffect(int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (NULL == pTalentInfo) return;

	LostMaintainEffect(pTalentInfo);
}

void XMyTalent::LostMaintainEffect(XTalentInfo* pTalentInfo)
{
	_ASSERT(NULL != pTalentInfo && "pTalentInfo is NULL");
	if (pTalentInfo == NULL)	return;

	m_Passive.LostEffect(pTalentInfo);
}

bool XMyTalent::Train(XTalentInfo* pTalentInfo, bool bConsumeTP)
{
	if (NULL == pTalentInfo) return false;
	if (IsLearnedTalent(pTalentInfo->m_nID)) return false; 	// 이미 배운것이면.

	m_setTalentID.insert(pTalentInfo->m_nID);
	m_setStyleTalentID[pTalentInfo->m_nStyle].insert(pTalentInfo->m_nID);

	if (ST_NONE != pTalentInfo->m_nSkillType)
	{
		GainMaintainEffect(pTalentInfo);
	}

	if (true == bConsumeTP)
	{
		m_nRemainTP -= CONSUME_TP_FOR_LEARN_TALENT;
	}

	/// 채집탤런트 학습 등록
	if (TT_GATHER == pTalentInfo->m_nTalentType)
	{
		m_GatherTalentRank.Insert(pTalentInfo->m_nRank);
	}

	return true;
}

void XMyTalent::UntrainAll()
{
	SET_TALENTID tempSetTalentID = m_setTalentID;

	for each(int nTalentID in tempSetTalentID)
	{
		Untrain(nTalentID);
	}
}

void XMyTalent::Untrain(int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (NULL == pTalentInfo) return;
	if (false == pTalentInfo->m_bUntrainable) return;

	m_setTalentID.erase(nTalentID);
	m_setStyleTalentID[pTalentInfo->m_nStyle].erase(nTalentID);

	if (ST_NONE != pTalentInfo->m_nSkillType)
	{
		gvar.MyInfo.Talent.LostMaintainEffect(pTalentInfo);
	}

	/// 채집탤런트 학습 등록해제
	if (TT_GATHER == pTalentInfo->m_nTalentType)
	{
		m_GatherTalentRank.Delete(pTalentInfo->m_nRank);
	}

	m_nRemainTP += 1;
}

void XMyTalent::GetAllTalentID(vector<int>& outvecTalentID)
{
	outvecTalentID.insert(outvecTalentID.end(), m_setTalentID.begin(), m_setTalentID.end());
}

bool XMyTalent::IsLearnedTalent(int nTalentID)
{	
	XTalentInfo* pTalentInfo = dynamic_cast<XTalentInfo*>(info.talent->Find(nTalentID));
	if (NULL == pTalentInfo) return false;

	return IsLearnedTalent(pTalentInfo);
}

bool XMyTalent::IsLearnedTalent(XTalentInfo* pTalentInfo)
{
	if (false == pTalentInfo->m_bNeedTraining) return true;
	if (m_setTalentID.find(pTalentInfo->m_nID) != m_setTalentID.end()) return true;
	if (m_setBuffTalentID.find(pTalentInfo->m_nID) != m_setBuffTalentID.end()) return true;

	return false;
}

bool XMyTalent::IsLearnedTalentLine(int nTalentLine)
{
	for each (int nTalentID in m_setTalentID)
	{
		XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
		if (NULL == pTalentInfo) continue;

		if (nTalentLine == pTalentInfo->m_nTalentLine) return true;
	}

	return false;
}

// 배운탈렌트중에서 nTalent와 같은 라인을 가진 탈렌트 중에서 가장 높은 랭크의 
// 탈레트 아이디를 구한다.
int XMyTalent::GetHighestRankByTalentLine( int nTalentID)
{
	if( nTalentID == 0 ) return 0;

	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	_ASSERT(NULL != pTalentInfo);
	if (pTalentInfo == NULL)	return 0;

	for each (int nTalentID1 in m_setTalentID)
	{
		XTalentInfo* pInfo = info.talent->Get(nTalentID1);
		if (pInfo == NULL)	continue;

		if( pTalentInfo->m_nTalentLine == pInfo->m_nTalentLine )
		{
			if (pTalentInfo->m_nRank < pInfo->m_nRank)
			{
				pTalentInfo = pInfo;
			}
		}
	}

	return pTalentInfo->m_nID;
}

void XMyTalent::GetHighestRankListOfTalent(vector<XTalentInfo*>& outvecTalentID)
{
	for each (int nTalentID in m_setTalentID)
	{
		XTalentInfo* pInfo = info.talent->Get(nTalentID);
		if (pInfo == NULL)	continue;

		bool bHasLine = false;
		for (size_t i=0; i<outvecTalentID.size(); i++)
		{
			XTalentInfo* pInfo2 = outvecTalentID[i];
			if (pInfo->m_nTalentLine == pInfo2->m_nTalentLine)
			{
				bHasLine = true;
				if (pInfo->m_nRank >= pInfo2->m_nRank)
				{
					outvecTalentID[i] = pInfo;
					break;
				}
			}
		}

		if (!bHasLine)
			outvecTalentID.push_back(pInfo);
	}
}

int XMyTalent::GetTalentID( int index)
{
	SET_TALENTID::iterator itr = m_setTalentID.begin();
	for ( int i = 0;  i < index;  i++, itr++) ;

	return (*itr);
}


int XMyTalent::GetEmotionalTalentCount() const
{
	return m_setSocialTalentID.size();
}

int XMyTalent::GetEmotionalTalentID( int index)
{
	if(GetEmotionalTalentCount() <= index)
		return 0;

	int nCount = 0;
	for(SET_TALENTID::iterator it = m_setSocialTalentID.begin(); it != m_setSocialTalentID.end(); ++it)
	{
		if(nCount == index)
			return *it;

		++nCount;
	}

	return 0;
}

bool XMyTalent::IsLearnedGatherTalent( int nRank )
{
	return m_GatherTalentRank.HasGatherTalent(nRank);
}

void XMyTalent::SetSocialTalentID( int nID )
{
	m_setSocialTalentID.insert(nID);
}

void XMyTalent::SetBuffTalentID( int arrPalettes[] )
{
	for (int i=0; i<PALETTESLOT_MAX; ++i)
	{
		if(arrPalettes[i] != 0)
			m_setBuffTalentID.insert(arrPalettes[i]);
	}
}

void XMyTalent::ReSetBuffTalentID()
{
	m_setBuffTalentID.clear();
}

bool XMyTalent::IsUsableBuffTalentID( int nID )
{
	if (m_setBuffTalentID.find(nID) != m_setBuffTalentID.end()) return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////

void XMyTalent::XGatherTalentRank::Insert(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return;

	m_setGatherTalentRank.insert(nRank);
}

void XMyTalent::XGatherTalentRank::Delete(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return;

	m_setGatherTalentRank.erase(nRank);
}

bool XMyTalent::XGatherTalentRank::HasGatherTalent(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return false;
	if (m_setGatherTalentRank.empty()) return false;

	set<int>::reverse_iterator iter = m_setGatherTalentRank.rbegin();
	int nLearnedMaxRank = *iter;
	if (nLearnedMaxRank >= nRank)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

XCoolTimeMgr::XCoolTimeMgr()
{
}

XCoolTimeMgr::~XCoolTimeMgr()
{
	Clear();
}

bool XCoolTimeMgr::IsToggleTalent(XTalentInfo* pTalentInfo)
{
	if (pTalentInfo->IsToggle())
	{
		int buffid = pTalentInfo->GetToggleBuffID();
		if (gvar.MyInfo.BuffExist( buffid))
		{
			return true;
		}
	}

	return false;
}

bool XCoolTimeMgr::IsToggleTalent(int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL) return false;

	return IsToggleTalent(pTalentInfo);
}

void XCoolTimeMgr::Update( float fDelta)
{
	// 경과 시간 업데이트
	for ( map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.begin();  itr != m_mapCoolTime.end();  itr++)
	{
		int nTalentID = (*itr).first;
		XCOOLTIME_UNIT& unit = (*itr).second;

		if (IsToggleTalent(nTalentID)) continue;

		if ( unit.nState == XCS_END)
			continue;

		unit.fElapsedTime += fDelta;

		if ( unit.fElapsedTime >= unit.fCoolTime)
			unit.nState = XCS_TIMEOUT;
	}


	// 시간 초과임에도 일정 시간이 지났으면 삭제함
	for ( map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.begin();  itr != m_mapCoolTime.end();  itr++)
	{
		XCOOLTIME_UNIT& unit = (*itr).second;

		if ( unit.nState == XCS_TIMEOUT  &&  unit.fElapsedTime > (unit.fCoolTime + 5.0f))
		{
			m_mapCoolTime.erase( itr);
			return;
		}
	}
}


void XCoolTimeMgr::Clear()
{
	m_mapCoolTime.clear();
	global.ui->InventoryUIRefresh();
}

vector<int> XCoolTimeMgr::GetRelativeTalentsID(CSTalentInfo* pTalentInfo)
{
	vector<int> ret;
	if (!pTalentInfo)
		return ret;

	ret = info.talent->GetLineTalents(pTalentInfo->m_nTalentLine);
	if (pTalentInfo->m_nCooltimeLine != INVALID_ID)
	{
		vector<int> vecSameCooltimeLineTalents = info.talent->GetCooltimeLineTalents(pTalentInfo->m_nCooltimeLine);
		std::copy(vecSameCooltimeLineTalents.begin(), vecSameCooltimeLineTalents.end(), std::back_inserter(ret));
		sort(ret.begin(), ret.end());
		ret.erase(unique(ret.begin(), ret.end()), ret.end());
	}

	return ret;
}

bool XCoolTimeMgr::IsCoolDown( int nTalentID)
{
	CSTalentInfo* pTalentInfo = info.talent->Find(nTalentID);
	if (!pTalentInfo)
		return false;

	vector<int> vecSameLineTalents = 
		GetRelativeTalentsID(pTalentInfo);
	for each (int each in vecSameLineTalents)
	{
		CSTalentInfo* pEachTalentInfo = info.talent->Find(each);
		if (!pEachTalentInfo)
			continue;	// 유효하지 않은 탤런트

		map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( each);
		if ( itr == m_mapCoolTime.end())
			continue;	// 쿨타임에 등록되지 않음

		if (IsToggleTalent(nTalentID)) 
			return false;	// 토글 버프가 있다면 쿨타임 무시

		XCOOLTIME_UNIT& unit = (*itr).second;
		switch ( unit.nState)
		{
		case XCS_END :
		case XCS_TIMEOUT :
			return false;

		case XCS_REQ_USE_TALENT :
		case XCS_CASTING :
		case XCS_REQ_ACT_TALENT :
		case XCS_USE_TALENT :
			{
				if ( unit.fElapsedTime < unit.fCoolTime)
					return true;
				break;
			}
		}
	}
	
	return false;
}


float XCoolTimeMgr::GetRemainedTime(int nTalentID)
{
	if ( nTalentID == 0)
		return 0.0f;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return 0.0f;

	vector<int> vecSameLineTalents = 
		GetRelativeTalentsID(pTalentInfo);
	for each (int each in vecSameLineTalents)
	{
		map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( each);
		if ( itr == m_mapCoolTime.end())
			continue; // 등록되지 않음

		XCOOLTIME_UNIT& unit = (*itr).second;
		switch ( unit.nState)
		{
		case XCS_END :
		case XCS_TIMEOUT :
			return 0.0f;

		case XCS_REQ_USE_TALENT :
		case XCS_CASTING :
		case XCS_REQ_ACT_TALENT :
			return pTalentInfo->m_fCoolTime;

		case XCS_USE_TALENT :
			return max( 0.0f,  unit.fCoolTime - unit.fElapsedTime);
		}
	}

	return 0.0f;
}


float XCoolTimeMgr::GetRemainedTimeRatio(int nTalentID)
{
	if ( nTalentID == 0)
		return 0.0f;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return 0.0f;

	if ( pTalentInfo->m_fCoolTime == 0.0f)	
		return 0.0f;

	vector<int> vecSameLineTalents = 
		GetRelativeTalentsID(pTalentInfo);
	for each (int each in vecSameLineTalents)
	{
		map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( each);
		if ( itr == m_mapCoolTime.end())
			continue; // 등록되지 않음

		XCOOLTIME_UNIT& unit = (*itr).second;
		switch ( unit.nState)
		{
		case XCS_END :
		case XCS_TIMEOUT :
			return 0.0f;

		case XCS_REQ_USE_TALENT :
		case XCS_CASTING :
		case XCS_REQ_ACT_TALENT :
			return 1.0f;

		case XCS_USE_TALENT :
			return max( 0.0f, unit.fCoolTime - unit.fElapsedTime) / unit.fCoolTime;
		}
	}

	return 0.0f;
}


void XCoolTimeMgr::RequestUseTalent( int nTalentID)
{
	if ( nTalentID == 0)
		return;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	// 탈랜트가 없으면 추가
	if ( itr == m_mapCoolTime.end())
	{
		_add_cooltime( nTalentID, XCS_REQ_USE_TALENT, 1.0f);
		return;
	}

	XCOOLTIME_UNIT& unit = (*itr).second;
	switch ( unit.nState)
	{
	case XCS_END :
	case XCS_TIMEOUT :
	case XCS_REQ_USE_TALENT :
		unit.nState = XCS_REQ_USE_TALENT;
		unit.fCoolTime = 1.0f;
		unit.fElapsedTime = 0.0f;
		return;

	case XCS_CASTING :
	case XCS_REQ_ACT_TALENT :
	case XCS_USE_TALENT :
		return;
	}
}


void XCoolTimeMgr::CastingTalent( int nTalentID)
{
	if ( nTalentID == 0)
		return;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	// 탈랜트가 없으면 추가
	if ( itr == m_mapCoolTime.end())
	{
		_add_cooltime( nTalentID, XCS_CASTING, pTalentInfo->m_fCastingTime + 1.0f);
		return;
	}

	XCOOLTIME_UNIT& unit = (*itr).second;
	switch ( unit.nState)
	{
	case XCS_END :
	case XCS_TIMEOUT :
	case XCS_REQ_USE_TALENT :
	case XCS_CASTING :
		unit.nState = XCS_CASTING;
		unit.fCoolTime = pTalentInfo->m_fCastingTime + 1.0f;
		unit.fElapsedTime = 0.0f;
		return;

	case XCS_REQ_ACT_TALENT :
	case XCS_USE_TALENT :
		return;
	}
}


void XCoolTimeMgr::RequestActTalent( int nTalentID)
{
	if ( nTalentID == 0)
		return;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	// 탈랜트가 없으면 추가
	if ( itr == m_mapCoolTime.end())
	{
		_add_cooltime( nTalentID, XCS_REQ_ACT_TALENT, 1.0f);
		return;
	}

	XCOOLTIME_UNIT& unit = (*itr).second;
	switch ( unit.nState)
	{
	case XCS_END :
	case XCS_TIMEOUT :
	case XCS_REQ_USE_TALENT :
	case XCS_CASTING :
	case XCS_REQ_ACT_TALENT :
		unit.nState = XCS_REQ_ACT_TALENT;
		unit.fCoolTime = 1.0f;
		unit.fElapsedTime = 0.0f;
		return;

	case XCS_USE_TALENT :
		return;
	}
}


void XCoolTimeMgr::UseTalent( int nTalentID)
{
	if ( nTalentID == 0)
		return;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	// 탈랜트가 없으면 추가
	if ( itr == m_mapCoolTime.end())
	{
		_add_cooltime( nTalentID, XCS_USE_TALENT, pTalentInfo->m_fCoolTime + 0.2f);
		return;
	}

	XCOOLTIME_UNIT& unit = (*itr).second;
	unit.nState = XCS_USE_TALENT;
	unit.fCoolTime = pTalentInfo->m_fCoolTime + 0.2f;
	unit.fElapsedTime = 0.0f;
}


void XCoolTimeMgr::Canceled( int nTalentID)
{
	if ( nTalentID == 0)
		return;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	if ( itr != m_mapCoolTime.end())
		m_mapCoolTime.erase( itr);

	global.ui->InventoryUIRefresh();
}


void XCoolTimeMgr::SetRemained( int nTalentID, float fRemainedTime)
{
	if ( nTalentID == 0)
		return;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)
		return;

	float fElapsedTime = pTalentInfo->m_fCoolTime - fRemainedTime;

	map< int, XCOOLTIME_UNIT>::iterator itr = m_mapCoolTime.find( nTalentID);

	if ( itr == m_mapCoolTime.end())
	{
		_add_cooltime( nTalentID, XCS_USE_TALENT, pTalentInfo->m_fCoolTime + 0.2f, fElapsedTime);
		return;
	}
}


void XCoolTimeMgr::_add_cooltime( int nTalentID, XCOOLTIME_STATE nState, float fCoolTime, float fElapsedTime)
{
	XCOOLTIME_UNIT unit;
	unit.nState = nState;
	unit.fCoolTime = fCoolTime;
	unit.fElapsedTime = fElapsedTime;

	m_mapCoolTime.insert( map< int, XCOOLTIME_UNIT>::value_type( nTalentID, unit));

	global.ui->InventoryUIRefresh();
}
