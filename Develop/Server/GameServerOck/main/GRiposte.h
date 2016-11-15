#pragma once

#include "CSTalentInfo.h"

class GRiposte
{
public:
	GRiposte(void);
	~GRiposte(void);

	// 되받아치기 정보를 추가, Rate는 되받아칠 보정치, 같은 종류의 되받아치기는 중첩됨
	void	AddRiposte(TALENT_CATEGORY nTalentCategory, float fRate);
	// 되받아치기 정보를 제거, Rate는 되받아칠 보정치
	void	EraseRiposte(TALENT_CATEGORY nTalentCategory, float fRate);
	// 되받아치기가 있는지 여부 반환
	bool	HasRiposte(TALENT_CATEGORY nTalentCategory);
	// 되받아치기 보정치를 반환, 1.0f을 반환하면 받을 피해 그대로를 되돌려 줌
	float	GetRiposteRate(TALENT_CATEGORY nTalentCategory);
	// 되받아치기 정보 초기화
	void	Clear();

private:
	// 탤런트 카테고리별 되받아치기 보정값들
	float m_Rate[TC_MAX];
};
