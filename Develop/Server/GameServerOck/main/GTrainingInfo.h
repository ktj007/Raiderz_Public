#ifndef _G_TRAINING_INFO_H_
#define _G_TRAINING_INFO_H_

class GTalentInfo;
class GConditionsInfo;

enum TRAINING_TYPE
{
	TRA_NONE = 0,
	TRA_COMBAT,
	TRA_CRAFT,

	TRA_MAX
};

class GTrainingInfo
{
public:
	int				m_nID;

	GDialogInfo*	m_pDialogInfo;
	TRAINING_TYPE	nTrainingType;

	set<GTalentInfo*>					m_setNormalTalent;		///< 아무나 배울 수 있는 탤런트
	map<GTalentInfo*, GConditionsInfo*>	m_mapConditionsTalent;	///< 컨디션을 만족하는 자만 배울 수 있는 탤런트
	
	GConditionsInfo* GetConditionsInfo(GTalentInfo* pTalentInfo);


public:
	GTrainingInfo();
	~GTrainingInfo();
};

#endif//_G_TRAINING_INFO_H_
