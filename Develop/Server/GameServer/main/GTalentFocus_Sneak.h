#pragma once

#include "GTalentFocus.h"

class GTalentFocus_Sneak : public GTalentFocus, public MTestMemPool<GTalentFocus_Sneak>
{
public:
	GTalentFocus_Sneak(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Sneak(void);

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_SNEAK; }

	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();

	// 일회성인지 여부. 포커스탤런트를 성공적으로 사용하고 포커스를 잃을지 결정한다.
	virtual bool				IsOnce()				{ return true; }
};
