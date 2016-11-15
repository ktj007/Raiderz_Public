#pragma once

#include "CSItemData.h"
#include "CSCommonLib.h"

/// 아이템 관련 헬퍼 클래스
class CSCOMMON_API CSItemHelper
{
public:
	/// 장비한 아이템으로 무기타입 알아내기
	static WEAPON_TYPE GetCurrentWeaponType(CSItemData* pRightItemData, CSItemData* pLeftItemData);
	static WEAPON_TYPE GetCurrentWeaponType(WEAPON_TYPE nRight, WEAPON_TYPE nLeft, bool bEquipShield=false);		// 방패는 둔기로 취급하기 때문에 방패를 차고 있는지 확인한다.

	/// 해당 무기타입이 양손으로 잡아야 하는 무기타입인지 여부
	static bool IsDualWieldWeaponType(WEAPON_TYPE nWeaponType);

	/// 방패인지 여부
	static bool IsShieldItemType(CSItemData* pItemData);

	/// 방패와 함께 착용이 가능한지 여부
	static bool IsEquipableWithShield(WEAPON_TYPE nWeaponType);

	/// 방어 액션이 가능한 무기타입인가?
	static bool IsGuardableWeaponType( WEAPON_TYPE nWeaponType );

	/// ITEMSLOT_LWEAPON 또는 ITEMSLOT_LWEAPON2 슬롯인지
	static bool IsLeftWeaponSlot(SH_ITEM_SLOT nSlot);
	static bool IsRightWeaponSlot(SH_ITEM_SLOT nSlot);
	static bool IsWeaponSlot(SH_ITEM_SLOT nSlot);
	static bool IsPrimaryWeaponSlot(SH_ITEM_SLOT nSlot);
	static bool IsSecondaryWeaponSlot(SH_ITEM_SLOT nSlot);
	static SH_ITEM_SLOT GetPrimaryWeaponSlot(SH_ITEM_SLOT nSlot);
	static SH_ITEM_SLOT GetSecondaryWeaponSlot(SH_ITEM_SLOT nSlot);
	static SH_ITEM_SLOT GetOppositeWeaponSlot(SH_ITEM_SLOT nSlot);


	// 메인슬롯으로 서브슬롯을 찾아낸다
	static SH_ITEM_SLOT GetSubSlot(SH_ITEM_SLOT nSlot, CSItemData* pItemData);
	// 착용가능한 슬롯인지 여부
	static bool IsEquipableSlot(SH_ITEM_SLOT nSlot, CSItemData* pItemData);
	// 함께 착용 가능한 아이템인지 여부
	static bool IsTogetherEquipableItemData(CSItemData* pItemData1, CSItemData* pItemData2);

	static SH_ITEM_SLOT GetProperWeaponSlot(SH_ITEM_SLOT nWishSlot, CSItemData* pItemData, CSItemData* pRightEquipedItemData, CSItemData* pLeftEquipedItemData);

	// Item Type
	static bool IsReusableItem(CSItemData* pItemData);
	static bool IsQuestTriggerItem(CSItemData* pItemData);
	static bool IsTalentTrainItem(CSItemData* pItemData);

	// pallete
	static int GetPalleteIndex(int nNum, int nSlot);

	static bool IsMustCheckPassiveType(TALENT_EXTRA_PASSIVE_TYPE nPassiveType);

	static TALENT_EXTRA_PASSIVE_TYPE GetTEPTForEquip(CSItemData* pItemData);
};