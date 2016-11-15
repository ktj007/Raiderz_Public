#pragma once

#include "GTalentInfo.h"
#include "CSTalentInfoMgr.h"

/// 스킬 정보 관리자
class GTalentInfoMgr : public CSTalentInfoMgr, public MTestMemPool<GTalentInfoMgr>
{
private:
	GTalentInfoMgr(GTalentInfoMgr& other);
public:
	GTalentInfoMgr();
	virtual ~GTalentInfoMgr();
	virtual void Cooking();

	bool					Load(const wchar_t* szRootPath = L"./");
	void					Clear();

	GTalentInfo*			Find(int nID, int nMode=0, bool bModeExactly=false);

	GTalentInfo*			GetByLineAndRank(int nLine, int8 nRank);
	GTalentInfo*			GetLowRankTalent(int nTalentID);
	GTalentInfo*			GetHighRankTalent(int nTalentID);	

	void					Erase(int nID);
	vector<int>				GetPlayerTalentIDs() const;
	bool					IsFocusTalent(int nID);

	void					GetStyleTalentID(TALENT_STYLE nStyle, vector<int>& vecTalentID) const;

private:
	map<TALENT_STYLE, vector<int>>	m_mapStyleTalentID;
};	
