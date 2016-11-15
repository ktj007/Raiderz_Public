#pragma once

class GItem;

enum ITEM_ACT_TYPE
{
	IAT_NONE = 0,

	IAT_DROP,			// 버리기
	IAT_SELL,			// 팔기
	IAT_TRADE,			// 거래	
	IAT_EQUIP,			// 장비
	IAT_UNEQUIP,		// 장비 해제
	IAT_USE,			// 사용
	IAT_CRAFT,			// 제작
	IAT_MOVE,			// 이동
	IAT_DYE,			// 염색
	IAT_ENCHANT,		// 인챈트
	IAT_REPAIR,			// 수리

	IAT_MAIL_APPEND,	// 우편 첨부

	IAT_MAX
};

class GItemActableChecker : public MTestMemPool<GItemActableChecker>
{
public:
	bool IsDropable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsSellable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsTradable(GEntityPlayer* pPlayer, GItem* pItem);	
	bool IsEquipable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsUnequipable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsUsable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsCraftable(GEntityPlayer* pPlayer, GItem* pItem);

	bool IsMovable(GEntityPlayer* pPlayer, GItem* pItem);

	bool IsDyeable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsEnchantable(GEntityPlayer* pPlayer, GItem* pItem);
	bool IsRepairable(GEntityPlayer* pPlayer, GItem* pItem);	

	bool IsMailAppendable(GEntityPlayer* pPlayer, const GItem* pItem) const;

private:
	bool IsItemActable(GEntityPlayer* pPlayer, const GItem* pItem, ITEM_ACT_TYPE nAct) const;
};