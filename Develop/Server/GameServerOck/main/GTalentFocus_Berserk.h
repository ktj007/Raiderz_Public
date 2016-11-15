#pragma once

#include "MTime.h"
#include "GTalentFocus.h"

class GTalentInfo;

class GTalentFocus_Berserk : public GTalentFocus, public MTestMemPool<GTalentFocus_Berserk>
{
public:
	GTalentFocus_Berserk(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus_Berserk(void);

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()				{ return TFT_BERSERK; }
	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate();
	// 포커스가 활성화될 때 호출
	virtual void				Active();
	// 매틱마다 호출됨, 반환값은 포커스가 만료됐는지 여부
	virtual bool				Update(float fDelta);
	// 포커스 지속시간 반환, 0.0f을 반환하면 무한함을 뜻한다.
	virtual float				GetDurationTime();
		
	// 라이센스가 있는지 여부
	virtual bool				CheckLicense() const override;

	// 근접탤런트로 때렸을때 이벤트
	void OnHitMeleeTalent(GTalentInfo* pHitTalentInfo);
	// 탤런트로 때린 횟수 반환
	int GetHitCount();
	// 포커스가 걸리기 위해 필요한 때린 횟수 반환
	int GetMaxHitCount();
private:
	// 탤런트로 때린 횟수가 초기화됨
	void ResetCounter();

private:
	// 탤런트로 때린 횟수
	int			m_nHitTalentCounter;
	// 탤런트 히트 만료 타이머, 포커스를 얻기 위해 탤런트로 히트할때 일정 시간내에 쳐야만 하게할때 쓰임
	MRegulator	m_rgrRelayHit;
};
