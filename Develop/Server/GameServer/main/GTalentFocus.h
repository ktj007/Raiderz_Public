#pragma once

#include "CSItemData.h"
#include "CSDef.h"

class GEntityPlayer;
class GEquipmentLimiter;


// 탈런트 포커스의 최상위 클래스
class GTalentFocus
{
public:
	GTalentFocus(GEntityPlayer* pOnwer);
	virtual ~GTalentFocus();

	// 해당 포커스 타입을 반환
	virtual TALENT_FOCUS_TYPE	GetType()=0;
	// 포커스가 활성화될 수 있는지 여부, 활성화 될 수 있다면 true를 반환한다.
	virtual bool				CheckActivate()=0;
	// 포커스가 활성화될 때 호출
	virtual void				Active();
	// 포커스가 비활성화될 때 호출
	virtual void				Deactive();
	// 활성화 됐는지 여부
			bool				IsActive()				{ return m_bActive; }
	// 일회성인지 여부. 포커스탤런트를 성공적으로 사용하고 포커스를 잃을지 결정한다.
	virtual bool				IsOnce()				{ return false; }
	// 매틱마다 호출됨, 반환값은 포커스가 만료됐는지 여부
	virtual bool				Update(float fDelta)	{ return false; }
	// 장비아이템이 포커스를 얻을 수 있는 조건이 되는지 여부
			bool				CheckEquipItem();
	// 포커스 지속시간 반환, 0.0f을 반환하면 무한함을 뜻한다.
	virtual float				GetDurationTime()		{ return BUFF_DURATION_INFINITY; }
	// 해당 포커스와 함께할 버프 아이디를 반환
			int					GetFocusBuffID();
	// 포커스 버프의 종류를 설정
			void				SetBuffType(int nBuffType);

	// 라이센스가 있는지 여부
	virtual bool				CheckLicense() const	{ return true; }
protected:
	// 착용아이템 제한조건을 추가
	void	AddEnableWeapon(WEAPON_TYPE nWeaponType);

	// 버프로 인해 포커스가 걸리는지 여부
	virtual bool IsGainViaBuff() { return false; }
private:
	// 포커스 버프 얻기
	void GainFocusBuff();
	// 포커스 버프 잃기
	void LostFocusBuff();
private:
	// 포커스가 활성화됐는지 여부
	bool			m_bActive;
	// 포커스 버프의 종류
	int				m_nBuffType;
protected:
	// 포커스 소유자
	GEntityPlayer*		m_pOwner;
	// 착용아이템에 대한 제한사항 관리자
	GEquipmentLimiter*	m_pEquipmentLimiter;
};
