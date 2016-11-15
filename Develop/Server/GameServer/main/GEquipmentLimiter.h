#pragma once

#include "CSItemData.h"

class GItemHolder;

// 장비슬롯으로부터 유효성 처리를 하기 위한 클래스
class GEquipmentLimiter : public MTestMemPool<GEquipmentLimiter>
{
public:
	GEquipmentLimiter(GItemHolder* pItemHolder);
	~GEquipmentLimiter(void);

	// 착용아이템 제한조건을 추가
	void AddEnableWeapon(WEAPON_TYPE nWeaponType);
	// 착용아이템이 제한조건을 충족시키는지 여부, 아무런 조건도 등록하지 않았다면 true를 반환
	bool IsEnable();
private:
	// 무기에 대한 제한조건을 충족시키는지 여부
	bool IsEnableWeapon();
	// 해당 무기조건이 등록되어있는지 여부
	bool IsRegisterEnableWeapon(WEAPON_TYPE nWeaponType);
private:
	// 플레이어의 아이템관리자
	GItemHolder*		m_pItemHolder;
	// 사용가능한 무기종류
	set<WEAPON_TYPE>	m_setEnableWeapons;
};
