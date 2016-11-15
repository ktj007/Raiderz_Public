#pragma once

#include "GTalentFocus.h"
#include "MTime.h"

class GTalentInfo;

class GTalentFocus_Enlighten : public GTalentFocus, public MTestMemPool<GTalentFocus_Enlighten>
{
public:
	GTalentFocus_Enlighten(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Enlighten(void);

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_ENLIGHTEN; }

	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();

protected:
	virtual bool IsGainViaBuff() { return true; }
};
