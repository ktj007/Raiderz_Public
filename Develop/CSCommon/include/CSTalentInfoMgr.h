#pragma once

#include "CSCommonLib.h"
#include "CSTalentInfo.h"
#include "CSTalentInfoDef.h"

typedef map<TCHAR, CSTalentInfo*>	TalentRankMap;
typedef map<int, TalentRankMap>		TalentMaxRankMap;
typedef map<int, vector<int>>		TalentLineMap;
typedef map<TALENT_EXTRA_PASSIVE_TYPE, int> GivedExtraPassiveMap;	


class CSCOMMON_API CSTalentInfoMgr : public map<int , CSTalentInfo* > 
{
protected:
	TalentMaxRankMap		m_mapTalentLine;
	TalentLineMap			m_mapLineTalents;
	TalentLineMap			m_mapCooltimeLineTalents;
	GivedExtraPassiveMap	m_mapGivedExtraPassive;		// 생성시 주어진 탤런트로 인해 획득한 엑스트라패시브
private:	
	CSTalentInfoMgr(CSTalentInfoMgr& other);
protected:
	void CopyHitFromReference( CSTalentInfo* pTalentInfo );
private:
	void ClassifyGivedTalents(bool bNeedTraining, TALENT_EXTRA_PASSIVE_TYPE nExtraPassive, int nExtraPassiveParam);
public:
	CSTalentInfoMgr() {}
	virtual ~CSTalentInfoMgr() {}
	virtual void Cooking();		// Loading후 TalentInfo의 추가적인 정보 작성
	void _SortHitInfos();
	void _CopyHitInfoRef();

	void Insert(CSTalentInfo* pTalentInfo);
	void Clear();

	// 같은 계열의 탤런트를 전부 반환
	vector<int>	GetLineTalents(int nTalentLine) const;
	vector<int>	GetCooltimeLineTalents(int nTalentLine) const;

	// bModeExactly를 false로 하면 mode가 없을때에는 기본 탤런트를 반환함
	CSTalentInfo* Find(int nID, int nMode=0, bool bModeExactly=false);	

	bool Exist(int nID);
	bool Exist( int nID, int nMode );

	bool IsGivedExtraPassive(TALENT_EXTRA_PASSIVE_TYPE nPassiveType);
	bool IsGivedExtraPassive(TALENT_EXTRA_PASSIVE_TYPE nPassiveType, int nExtraPassiveLevel);
};


