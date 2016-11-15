#ifndef _G_ITEM_REMOVER_H_
#define _G_ITEM_REMOVER_H_

class GEntityPlayer;
class GItemUnequiper;
class GItemHolder;
class GItem;
class GItemData;

struct GITEM_STACK_AMT;
struct GDBT_MAIL_APPENDED_ITEM;

struct REMOVE_ITEM
{
	REMOVE_ITEM(const uint8 nSlotType, const int16 nSlotID, const int16 nToStackAmt, const int16 nStackModAmt);
	REMOVE_ITEM(const GITEM_STACK_AMT& itemStackAmt);

	uint8 m_nSlotType;
	int16 m_nSlotID;
	int16 m_nToStackAmt;
	int16 m_nStackModAmt;
};

TEST_FORWARD_FT(ItemRemover, Fixture, Check);
class GItemRemover : public MTestMemPool<GItemRemover>
{
TEST_FRIEND_FT(ItemRemover, Fixture, Check);
friend class GTradeExchangerForDBTask;
public:
	// 삭제예정
	bool Remove(GEntityPlayer* pPlayer, int nItemID, int nRemoveAmount);
	bool Remove(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID, int nRemoveAmount);
	// 삭제예정
	
	bool RemoveForDBTask(const MUID& nPlayerUID, vector<REMOVE_ITEM>& vecRemoveItem);
	bool RemoveForDBTask(const MUID& nPlayerUID, const REMOVE_ITEM& removeItem);

private:
	bool Check(GEntityPlayer* pPlayer, GItem* pItem, int nRemoveAmount);

	bool DB_Delete(GEntityPlayer* pPlayer, GItem* pItem);
	bool DB_Decrease(GEntityPlayer* pPlayer, GItem* pItem, int nDecreaseAmount);
	void Server_Delete(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID);
	void Server_Decrease(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID, int nDecreaseAmount);	

	void Route(GEntityPlayer* pPlayer, vector<REMOVE_ITEM>& vecRemoveItem);
};

#endif
