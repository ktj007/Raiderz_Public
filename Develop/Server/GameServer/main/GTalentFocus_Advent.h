#pragma once

#include "GTalentFocus.h"

class GEntityPlayer;

class GTalentFocus_Advent : public GTalentFocus, public MTestMemPool<GTalentFocus_Advent>
{
public:
	GTalentFocus_Advent(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Advent(void);

	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();
	// 포커스 지속시간 반환, 0.0f을 반환하면 무한함을 뜻한다.
	virtual float				GetDurationTime();

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_ADVENT; }
};
