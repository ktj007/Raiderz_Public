#pragma once

#include "GTalentFocus.h"

class GEntityActor;

class GTalentFocus_Counter : public GTalentFocus, public MTestMemPool<GTalentFocus_Counter>
{
public:
	GTalentFocus_Counter(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Counter(void);

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_COUNTER; }
	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();
	// 포커스 지속시간 반환, 0.0f을 반환하면 무한함을 뜻한다.
	virtual float				GetDurationTime();
	// 일회성인지 여부. 포커스탤런트를 성공적으로 사용하고 포커스를 잃을지 결정한다.
	virtual bool				IsOnce()				{ return true; }
	
	// 라이센스가 있는지 여부
	virtual bool				CheckLicense() const override;

	// 방어 이벤트
	void OnGuard(GEntityActor* pAttacker);
	void OnAbsoluteGuard(GEntityActor* pAttacker);
private:
	// 가드를 성공적으로 한 횟수
	int m_nGuardCount;
};
