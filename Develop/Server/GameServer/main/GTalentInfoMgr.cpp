#include "stdafx.h"
#include "GTalentInfoMgr.h"
#include "CSChrInfo.h"
#include "GTalentInfoLoader.h"
#include "AStlUtil.h"

///////////////////////////////////////////////////////////////////////////////
GTalentInfoMgr::GTalentInfoMgr() : CSTalentInfoMgr()
{

}

GTalentInfoMgr::~GTalentInfoMgr()
{
	Clear();
}

void GTalentInfoMgr::Clear()
{
	__super::Clear();

	while(!empty()) 
	{
		GTalentInfo* pTalentInfo = static_cast<GTalentInfo*>(begin()->second);
		delete pTalentInfo;
		erase(begin());
	}
}

GTalentInfo* GTalentInfoMgr::Find( int nID, int nMode, bool bModeExactly )
{
	return static_cast<GTalentInfo*>(CSTalentInfoMgr::Find(nID, nMode, bModeExactly));
}

GTalentInfo* GTalentInfoMgr::GetByLineAndRank(int nLine, int8 nRank)
{
	TalentMaxRankMap::iterator itorLine = m_mapTalentLine.find(nLine);
	if (m_mapTalentLine.end() == itorLine) return NULL;

	TalentRankMap mapTalentRank = (*itorLine).second;

	TalentRankMap::iterator itorRank = mapTalentRank.find(nRank);
	if (mapTalentRank.end() == itorRank) return NULL;

	GTalentInfo* pTalentInfo = static_cast<GTalentInfo*>((*itorRank).second);

	return pTalentInfo;
}

GTalentInfo* GTalentInfoMgr::GetLowRankTalent(int nTalentID)
{
	GTalentInfo* pTalentInfo = Find(nTalentID);
	if (NULL == pTalentInfo) return NULL;

	if (0 >= pTalentInfo->m_nTalentLine) return NULL;
	if (1 >= pTalentInfo->m_nRank) return NULL;

	GTalentInfo* pLowRankTalent = GetByLineAndRank(pTalentInfo->m_nTalentLine, pTalentInfo->m_nRank - 1);

	return pLowRankTalent;
}

GTalentInfo* GTalentInfoMgr::GetHighRankTalent(int nTalentID)
{
	GTalentInfo* pTalentInfo = Find(nTalentID);
	if (NULL == pTalentInfo) return NULL;

	if (0 >= pTalentInfo->m_nTalentLine) return NULL;
	if (INT_MAX <= pTalentInfo->m_nRank) return NULL;

	GTalentInfo* pHighRankTalent = GetByLineAndRank(pTalentInfo->m_nTalentLine, pTalentInfo->m_nRank + 1);

	return pHighRankTalent;
}

bool GTalentInfoMgr::Load(const wchar_t* szRootPath)
{
	GTalentInfoPathParam talentInfoPathParam;
	talentInfoPathParam.strTalentInfo = wstring(szRootPath) + FILENAME_TALENTINFO;
	talentInfoPathParam.strTalentInfoExt = wstring(szRootPath) + FILENAME_TALENTINFO_EXT;
	talentInfoPathParam.strTalentPosInfo = wstring(szRootPath) + FILENAME_TALENT_POS_INFO;
	talentInfoPathParam.strTalentHitInfo = wstring(szRootPath) + FILENAME_TALENT_HIT_INFO;

	GTalentInfoLoader talentInfoLoader;
	if (talentInfoLoader.Load(this, talentInfoPathParam) == false)
	{
		return false;
	}

	return true;
}

void GTalentInfoMgr::Cooking()
{
	CSTalentInfoMgr::Cooking();

	for (iterator itor = begin(); itor != end(); ++itor)
	{
		CSTalentInfo* pTalentInfo = (*itor).second;

		MapSecondVectorInserter(m_mapStyleTalentID, pTalentInfo->m_nStyle, pTalentInfo->m_nID);
	}
}

void GTalentInfoMgr::Erase( int nID )
{
	iterator iter = find(nID);
	if (iter == end())	return;

	delete iter->second;
	erase(iter);
}

vector<int> GTalentInfoMgr::GetPlayerTalentIDs() const
{
	vector<int> vecTalents;
	for each (pair<int,CSTalentInfo*> each in *this)
	{
		CSTalentInfo* pEachTalentInfo = each.second;
		if (!pEachTalentInfo->m_setNPCID.empty())
			continue; // NPC가 사용중인 탤런트

		vecTalents.push_back(each.first);
	}
	return vecTalents;
}

bool GTalentInfoMgr::IsFocusTalent( int nID )
{
	for (int i=TFT_ADVENT; i<TFT_SIZE; i++)
	{
		int nFocustTalentID = TALENT_FOCUS_ID[i];
		if (nFocustTalentID == nID)
			return true;
	}

	return false;
}

void GTalentInfoMgr::GetStyleTalentID(TALENT_STYLE nStyle, vector<int>& vecTalentID) const
{
	map<TALENT_STYLE, vector<int>>::const_iterator itor = m_mapStyleTalentID.find(nStyle);
	if (m_mapStyleTalentID.end() == itor) return;

	vecTalentID.insert(vecTalentID.end(), itor->second.begin(), itor->second.end());
}
