#pragma once

#include "CSDef.h"
#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GTalentInfo;
class CSTalentInfoMgr;

/// 탤런트 정보 검증
class GVALIDATOR_API GTalentInfoValidator : public CSBaseValidator
{
private:
	void CheckTalentExtInfo(GTalentInfo* pTalentInfo);
	void CheckPointAndRelation(GTalentInfo* pTalentInfo);
	void CheckRank(GTalentInfo* pTalentInfo);
	void CheckLineNRank( GTalentInfo* pTalentInfo );
	void CheckMotionFactor(GTalentInfo* pTalentInfo);
	void CheckBuff( GTalentInfo* pTalentInfo );
	void CheckTNeedLine(GTalentInfo* pTalentInfo);
	void CheckModifier(GTalentInfo* pTalentInfo);
	void CheckActTime( GTalentInfo* pTalentInfo );
	void CheckExtraActive(GTalentInfo* pTalentInfo);
	void CheckExtraActiveToggle(GTalentInfo* pTalentInfo, TALENT_FOCUS_TYPE nFocusType);
	void CheckDamageAttrib(GTalentInfo* pTalentInfo);
	void CheckTalentPosInfo();
	void CheckActEvent(GTalentInfo* pTalentInfo);
	void CheckGather(GTalentInfo* pTalentInfo);
	void CheckAABBSize(GTalentInfo* pTalentInfo);
	bool Load( map<int, int>* vecTalentList, const wchar_t * szFileName );

	void CheckNPC(GTalentInfo* pTalentInfo);

public:
	virtual bool Check();	
};
