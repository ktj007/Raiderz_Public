#pragma once

#include "TAnimationHelper.h"

class TAnimationResourceController;
struct ANIMATION_INFO;
struct TALENT_ANIMATION_INFO;
//////////////////////////////////////////////////////////////////////////
class TTalentActAnimationTime
{
private:
	TAnimationResourceController* m_pAniResourceController;

private:
	float					GetAnimationTime(ANIMATION_INFO& aniInfo);
	float					GetAnimationRealTime(int nMaxFrame, float fAnimationSpeed);

	bool					CheckDifferentData(CSTalentInfo* pTalentInfo, vector<CSTalentActAnimationTimeInfo>& vecActAniTimeInfo);

	void					MakeTalentActAnimationTime(TALENT_ANIMATION_INFO& talentAniInfo, vector<CSTalentActAnimationTimeInfo>& vecActAniTimeInfo, TALENT_ANIMATION_TYPE eType = TAT_USE);

	bool					CalTalentUseAnimationTime(CSTalentInfo* pTalentInfo);
	bool					CalTalentExtraAnimationTime(CSTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eType);

public:
	TTalentActAnimationTime();
	virtual ~TTalentActAnimationTime();

	bool					CalTalentActAnimationTime(CSTalentInfo* pTalentInfo);
};