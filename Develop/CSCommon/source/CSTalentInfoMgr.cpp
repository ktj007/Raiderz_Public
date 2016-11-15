#include "stdafx.h"
#include "CSTalentInfoMgr.h"
#include <algorithm>
#include "CSChrInfo.h"
#include "CSTalentDef.h"
#include "CSTalentInfoParser.h"
#include "CSHitInfoLoader.h"
#include "CSHitInfoSaver.h"
#include "MLocale.h"

//////////////////////////////////////////////////////////////////////////


bool lessHitSegCheckTime(const CSHitSegment& c1, const CSHitSegment& c2)
{
	return (c1.m_fCheckTime < c2.m_fCheckTime);
}

void CSTalentInfoMgr::_SortHitInfos()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		CSTalentInfo* pTalentInfo = (*itor).second;
		sort(pTalentInfo->m_HitInfo.m_vSegments.begin(), pTalentInfo->m_HitInfo.m_vSegments.end(), lessHitSegCheckTime);

		if (pTalentInfo->ExistMode())
		{
			for (int i = 0; i < MAX_TALENT_MODE; i++)
			{
				if (pTalentInfo->GetMode(i))
				{
					CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(i);
					sort(pModeTalentInfo->m_HitInfo.m_vSegments.begin(), pModeTalentInfo->m_HitInfo.m_vSegments.end(), lessHitSegCheckTime);
				}
			}
		}
	}
}

void CSTalentInfoMgr::Cooking()
{
	_SortHitInfos();
	_CopyHitInfoRef();

	for (iterator itor = begin(); itor != end(); ++itor)
	{
		CSTalentInfo* pTalentInfo = (*itor).second;

		pTalentInfo->Cooking();
	}
}

void CSTalentInfoMgr::ClassifyGivedTalents(bool bNeedTraining, TALENT_EXTRA_PASSIVE_TYPE nExtraPassive, int nExtraPassiveParam)
{
	if (TEPT_NONE == nExtraPassive)
		return;
	
	if (bNeedTraining)
		return;

	GivedExtraPassiveMap::iterator extraPassiveItor = m_mapGivedExtraPassive.find(nExtraPassive);
	if (m_mapGivedExtraPassive.end() == extraPassiveItor)
	{
		m_mapGivedExtraPassive.insert(GivedExtraPassiveMap::value_type(nExtraPassive, nExtraPassiveParam));
	}
	else
	{
		int nExtraPassiveValue = extraPassiveItor->second;
		if (nExtraPassiveValue < nExtraPassiveParam)
		{
			extraPassiveItor->second = nExtraPassiveParam;
		}
	}

}

void CSTalentInfoMgr::Insert(CSTalentInfo* pTalentInfo )
{
	if (NULL == pTalentInfo) return;

	if (pTalentInfo->m_nMode > 0)
	{
		CSTalentInfo* pDefaultTalentInfo = Find(pTalentInfo->m_nID);
		if (!pDefaultTalentInfo)
		{
			assert(false);
			return;
		}
		
		pDefaultTalentInfo->InsertMode(pTalentInfo);
		return;
	}

	// 탤런트 라인이 지정되어 있지 않다면 탤런트 ID로 대체
	if (pTalentInfo->m_nTalentLine == 0)
	{
		pTalentInfo->m_nTalentLine = pTalentInfo->m_nID;
	}

	// 탤런트 라인별로 분류하기
	{
		TalentLineMap::iterator it = m_mapLineTalents.find(pTalentInfo->m_nTalentLine);
		if (m_mapLineTalents.end() == it)
		{
			vector<int> vecTalents;
			vecTalents.push_back(pTalentInfo->m_nID);
			m_mapLineTalents.insert(std::make_pair(pTalentInfo->m_nTalentLine, vecTalents));
		}
		else
		{
			it->second.push_back(pTalentInfo->m_nID);
		}
	}

	// 탤런트 쿨타임 라인별로 분류하기
	if (pTalentInfo->m_nCooltimeLine != INVALID_ID)
	{
		TalentLineMap::iterator it = m_mapCooltimeLineTalents.find(pTalentInfo->m_nCooltimeLine);
		if (m_mapCooltimeLineTalents.end() == it)
		{
			vector<int> vecTalents;
			vecTalents.push_back(pTalentInfo->m_nID);
			m_mapCooltimeLineTalents.insert(std::make_pair(pTalentInfo->m_nCooltimeLine, vecTalents));
		}
		else
		{
			it->second.push_back(pTalentInfo->m_nID);
		}
	}
	
	// 탤런트 라인별 랭크 구하기
	{
		TalentMaxRankMap::iterator lineItor = m_mapTalentLine.find(pTalentInfo->m_nTalentLine);
		if (m_mapTalentLine.end() == lineItor)
		{
			TalentRankMap mapTalentRank;
			mapTalentRank.insert(TalentRankMap::value_type(pTalentInfo->m_nRank, pTalentInfo));
			m_mapTalentLine.insert(TalentMaxRankMap::value_type(pTalentInfo->m_nTalentLine, mapTalentRank));
		}
		else
		{
			(*lineItor).second.insert(TalentRankMap::value_type(pTalentInfo->m_nRank, pTalentInfo));
		}
	}
	
	// 배우지 않아도 기본적으로 주어지는 탤런트 목럭 구하기
	ClassifyGivedTalents(pTalentInfo->m_bNeedTraining, pTalentInfo->m_nExtraPassive, pTalentInfo->m_nExtraPassiveParam);
	ClassifyGivedTalents(pTalentInfo->m_bNeedTraining, pTalentInfo->m_nExtraPassive2, pTalentInfo->m_nExtraPassiveParam2);
	
	// 해당 ID가 이미 있음
	if(find(pTalentInfo->m_nID) != end())
	{
		mlog("(%s) Talent (%d)ID Duplication\n", MLocale::ConvTCHARToAnsi(pTalentInfo->m_szName.c_str()).c_str(), pTalentInfo->m_nID);
//		_ASSERT(0);	
	}

	insert(value_type(pTalentInfo->m_nID, pTalentInfo));
}

void CSTalentInfoMgr::Clear()
{
	m_mapTalentLine.clear();
	m_mapLineTalents.clear();
	m_mapCooltimeLineTalents.clear();
	m_mapGivedExtraPassive.clear();	
}

CSTalentInfo* CSTalentInfoMgr::Find( int nID, int nMode, bool bModeExactly )
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;	

	CSTalentInfo* pTalentInfo = (*itor).second;

	if (nMode > 0)
	{
		if (bModeExactly)
		{
			return pTalentInfo->GetMode(nMode);
		}

		if (pTalentInfo->ExistMode())
		{
			CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(nMode);

			if (pModeTalentInfo) 
			{
				return pModeTalentInfo;
			}
		}
	}

	return pTalentInfo;
}

bool CSTalentInfoMgr::Exist( int nID )
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool CSTalentInfoMgr::Exist( int nID, int nMode )
{
	CSTalentInfo* pTalentInfo = Find(nID);
	if (pTalentInfo == NULL) return false;

	if (nMode > 0 && nMode < MAX_TALENT_MODE)
	{
		CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(nMode);
		if (pModeTalentInfo == NULL) return false;
	}

	return true;
}

void CSTalentInfoMgr::_CopyHitInfoRef()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		CSTalentInfo* pTalentInfo = (*itor).second;
		CopyHitFromReference(pTalentInfo);
	}
}

void CSTalentInfoMgr::CopyHitFromReference( CSTalentInfo* pTalentInfo )
{
	if (pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		CSTalentInfo* pRefTalentInfo = Find(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);
		if (pRefTalentInfo)
		{
			pRefTalentInfo->m_HitInfo.CopyTo(pTalentInfo->m_HitInfo);
		}
	}

	if (pTalentInfo->ExistMode())
	{
		for (int i = 0; i < MAX_TALENT_MODE; i++)
		{
			CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(i);

			if (pModeTalentInfo)
			{
				if (pModeTalentInfo->m_HitInfo.m_nReferenceID > 0)
				{
					CSTalentInfo* pRefTalentInfo = Find(pModeTalentInfo->m_HitInfo.m_nReferenceID, pModeTalentInfo->m_HitInfo.m_nReferenceMode);
					if (pRefTalentInfo)
					{
						pRefTalentInfo->m_HitInfo.CopyTo(pModeTalentInfo->m_HitInfo);
					}
				}
			}
		}
	}
}

vector<int> CSTalentInfoMgr::GetLineTalents( int nTalentLine ) const
{
	TalentLineMap::const_iterator it = m_mapLineTalents.find(nTalentLine);
	if (it == m_mapLineTalents.end())
		return vector<int>();

	return it->second;
}

vector<int> CSTalentInfoMgr::GetCooltimeLineTalents( int nTalentLine ) const
{
	if (nTalentLine == INVALID_ID)
		return vector<int>();

	TalentLineMap::const_iterator it = m_mapCooltimeLineTalents.find(nTalentLine);
	if (it == m_mapCooltimeLineTalents.end())
		return vector<int>();

	return it->second;
}

bool CSTalentInfoMgr::IsGivedExtraPassive( TALENT_EXTRA_PASSIVE_TYPE nPassiveType )
{
	if (m_mapGivedExtraPassive.end() == m_mapGivedExtraPassive.find(nPassiveType)) return false;

	return true;
}

bool CSTalentInfoMgr::IsGivedExtraPassive( TALENT_EXTRA_PASSIVE_TYPE nPassiveType, int nExtraPassiveLevel )
{
	GivedExtraPassiveMap::iterator itor = m_mapGivedExtraPassive.find(nPassiveType);
	if (m_mapGivedExtraPassive.end() == itor) return false;

	int nGivedExtraPsssiveLevel = (*itor).second;

	return (nExtraPassiveLevel <= nGivedExtraPsssiveLevel);
}


