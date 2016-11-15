#pragma once

class GEntityPlayer;
class GItem;

class GItemEnchant : public MTestMemPool<GItemEnchant>
{
public:
	GItemEnchant(void);
	~GItemEnchant(void);

	// 강화석으로 아이템에 강화
	bool Enchant( GEntityPlayer* pPlayer, int nEnchantSlotID, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nAgentSlotID = INVALID_ID);
	bool Enchant( GEntityPlayer* pPlayer, GItem* pEStone, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, GItem* pEAgent );
	int EnchantCheck( GEntityPlayer* pPlayer, GItem* pEStone, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, GItem* pEAgent );
	void EnchantPrepare( GEntityPlayer* pPlayer );
	// 아이템에 버프 아이템 연결, 반환값은 강화된 버프의 슬롯 번호 (실패시 -1)
	int EnchantBuff( GEntityPlayer* pPlayer, GItem* pEStone, GItem* pTargetItem, ENCHANT_COLOR nEnchantColor=ENCHANT_NONE );
	// 강화 성공 확률
	float CalcEnchantChanceRate( GItem* pEStone, GItem* pTargetItem ) const;
	// 강화 성공 체크
	bool CheckChance(GItem* pEStone, GItem* pTargetItem) const;
	// 강화 취소
	void EnchantCancel( GEntityPlayer* pPlayer );

	// 강화제 아이템 아이디
	int GetAgentItemID(int nTier);

	// 강화 검사 응답 패킷 송신
	void RouteEnchantCheckResult(GEntityPlayer* pPlayer, int nErrorCode);
private:
	void EnchantBroken(GEntityPlayer* pPlayer, GItem* pEStone, GItem* pEAgent, GItem* pTargetItem);
	bool CheckLimitLevel(GEntityPlayer* pPlayer, GItem* pEStone);
	bool CheckLimitEquipments(GItem* pTargetItem, GItem* pEStone);
	bool CheckLimitWeapon(GItem* pTargetItem, GItem* pEStone);
	bool CheckAgent(GItem* pEStone, GItem* pEAgent);
	bool EnchantToDB( GEntityPlayer* pPlayer, GItem* pEStone, GItem* pEAgent, GItem* pTargetItem, bool bBroken );
	int GetEmptyEnchantSlotIdx(GItem* pEStone, GItem* pTargetItem, GItem* pEAgent);
};
