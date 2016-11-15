#include "stdafx.h"
#include "XTalentInfoMgr.h"
#include "MUtil.h"
#include "MMath.h"
#include "MLocale.h"
#include "CSChrInfo.h"
#include "MCsvParser.h"
#include "XTalentEffectInfo.h"
#include "XTalentInfo.h"
#include "XTalentInfoLoader.h"

///////////////////////////////////////////////////////////////////////////////
XTalentInfoMgr::XTalentInfoMgr() : CSTalentInfoMgr()
{

}

XTalentInfoMgr::~XTalentInfoMgr()
{
	Destroy();
}


XTalentInfo* XTalentInfoMgr::Get( int nID, int nMode, bool bModeExactly )
{
	return static_cast<XTalentInfo*>(CSTalentInfoMgr::Find(nID, nMode, bModeExactly));
}


bool XTalentInfoMgr::IsPassiveTalent(int nID)
{
	XTalentInfo* pTalentInfo = Get(nID);
	if (pTalentInfo == NULL)	return false;

	if( pTalentInfo->IsPassiveSkill() )
		return true;

	return false;
}

XTalentInfo* XTalentInfoMgr::GetNextRankTalent(int nTalentID)
{
	XTalentInfo* pTalentInfo = Get(nTalentID);
	if (pTalentInfo == NULL) return NULL;

	if (0 >= pTalentInfo->m_nTalentLine) return NULL;
	if (INT_MAX <= pTalentInfo->m_nRank) return NULL;

	XTalentInfo* pHighRankTalent = GetByLineAndRank(pTalentInfo->m_nTalentLine, pTalentInfo->m_nRank+1);
	return pHighRankTalent;
}

XTalentInfo* XTalentInfoMgr::GetByLineAndRank(int nLine, int nRank)
{
	TalentMaxRankMap::iterator itorLine = m_mapTalentLine.find(nLine);
	if (m_mapTalentLine.end() == itorLine) return NULL;

	TalentRankMap mapTalentRank = (*itorLine).second;

	TalentRankMap::iterator itorRank = mapTalentRank.find(nRank);
	if (mapTalentRank.end() == itorRank) return NULL;

	XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>((*itorRank).second);

	return pTalentInfo;
}

XTalentInfo* XTalentInfoMgr::GetHighRankTalent(int nTalentID)
{
	XTalentInfo* pTalentInfo = Get(nTalentID);
	if (pTalentInfo == NULL) return NULL;

	if (0 >= pTalentInfo->m_nTalentLine) return NULL;
	if (INT_MAX <= pTalentInfo->m_nRank) return NULL;

	XTalentInfo* pHighRankTalent = GetByLineHighRank(pTalentInfo->m_nTalentLine);
	return pHighRankTalent;
}

XTalentInfo* XTalentInfoMgr::GetByLineHighRank(int nLine)
{
	TalentMaxRankMap::iterator itorLine = m_mapTalentLine.find(nLine);
	if (m_mapTalentLine.end() == itorLine) return NULL;

	TalentRankMap mapTalentRank = (*itorLine).second;

	TalentRankMap::reverse_iterator itorRank = mapTalentRank.rbegin();
	if (mapTalentRank.rend() == itorRank) return NULL;

	XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>((*itorRank).second);

	return pTalentInfo;
}

bool XTalentInfoMgr::Load( const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName )
{
	XTalentInfoLoader talentInfoLoader;
	if (talentInfoLoader.Load(this, szFileName, szExtFileName, szHitFileName) == false) return false;

	Cooking();

	return true;
}

///////////////////////////////////////////////////////////////////////////////


void XTalentInfoMgr::BuildGroupInfo()
{
	//for (iterator itor = begin(); itor != end(); ++itor)
	//{
	//	XTalentInfo* pTalent = (XTalentInfo*)(*itor).second;
	//	if (pTalent->m_nGroupID != 0)
	//	{
	//		m_mapGroup[pTalent->m_nGroupID].m_nGroupID = pTalent->m_nGroupID;

	//		if (pTalent->m_nStepIndex == 1)
	//			m_mapGroup[pTalent->m_nGroupID].m_vStep1.push_back(pTalent->m_nID);
	//		else if (pTalent->m_nStepIndex == 2)
	//			m_mapGroup[pTalent->m_nGroupID].m_vStep2.push_back(pTalent->m_nID);
	//		else if (pTalent->m_nStepIndex == 3)
	//			m_mapGroup[pTalent->m_nGroupID].m_vStep3.push_back(pTalent->m_nID);
	//	}
	//}
}

void XTalentInfoMgr::Cooking()
{
	CSTalentInfoMgr::Cooking();

	BuildGroupInfo();
}

bool XTalentInfoMgr::Reload( const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName )
{
	Destroy();

	return Load(szFileName, szExtFileName, szHitFileName);
}

void XTalentInfoMgr::Destroy()
{
	while(!empty()) 
	{
		delete begin()->second;
		erase(begin());
	}
}

void XTalentInfoMgr::GetSocialTalentID( vector<int>& vecSocial )
{
	for(map<int , CSTalentInfo* >::iterator it = begin(); it != end(); ++it)
	{
		if(it->second->m_nSkillType == ST_SOCIAL)
			vecSocial.push_back(it->first);
	}
}

//----------------------------------------------------------------------------//
// [2007/6/8 isnara]
int	XTalentEffectDataMgr::Get(TALENT_EFFECT_TYPE nType, vector<XTalentEffectInfo *> &TalentEffectInfo)
{
	int nInfoCount = 0;

	vector<XTalentEffectInfo *>::iterator itInfo = begin();
	while(itInfo != end())
	{
		if((*itInfo)->m_nEffectType == nType)
		{
			TalentEffectInfo.push_back(*itInfo);
			nInfoCount++;
		}

		itInfo++;
	}

	return nInfoCount;
}

void XTalentInfoMgr::Visit(IVisitor& visitor)
{
	for each (map<int , CSTalentInfo* >::value_type talentPair in *this)
	{
		CSTalentInfo* const talentInfo(talentPair.second);
		assert(NULL!=talentInfo);
		visitor.OnVisit(*talentInfo);
	}
}
