#pragma once

#include "GTalentFocus.h"

class GEntityPlayer;

class GTalentFocus_Precision : public GTalentFocus, public MTestMemPool<GTalentFocus_Precision>
{
public:
	GTalentFocus_Precision(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Precision(void);

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_PRECISION; }

	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();
};
