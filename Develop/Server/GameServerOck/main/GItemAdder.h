#pragma once

class GEntityPlayer;
class GItem;
class GItemData;
struct TD_ITEM;
struct TD_ITEM_INSERT;
struct TD_ITEM_INCREASE;
class GItemHolder;

struct ADD_ITEM
{
	ADD_ITEM(uint8 nSlotType, const int16 nSlotID, const int16 nToStackAmt, const int16 nStackModAmt, 
			 const int64 nIUID=0, const int nItemID=0, const int nAddExpiMin=-1, const int nUsagePeriod=0, const int nDurablity=-1, const int nColor=0)
	: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nToStackAmt(nToStackAmt), m_nStackModAmt(nStackModAmt),
	  m_nIUID(nIUID), m_nItemID(nItemID), m_nAddExpiMin(nAddExpiMin), m_nUsagePeriod(nUsagePeriod), m_nDurability(nDurablity), m_nColor(nColor){}

	uint8	m_nSlotType;
	int16	m_nSlotID;
	int16	m_nToStackAmt;
	int16	m_nStackModAmt;

	uint64	m_nIUID;
	int		m_nItemID;
	int		m_nAddExpiMin;
	int		m_nUsagePeriod;
	int		m_nDurability;
	int		m_nColor;
};

struct CHECK_ADDABLE_ITEM
{
	CHECK_ADDABLE_ITEM(int nItemID, int nAmount)
	: m_nItemID(nItemID), m_nAmount(nAmount)
	{}

	int		m_nItemID;
	int		m_nAmount;
};

class GItemAdder : public MTestMemPool<GItemAdder>
{
friend class GTradeExchangerForDBTask;
public:
	bool Add(const MUID& uidPlayer, int nItemID, int nAddAmount=1);
	bool Add(GEntityPlayer* pPlayer, int nItemID, int nAddAmount=1);	

	bool AddForDBTask(const MUID& nPlayerUID, const ADD_ITEM& addItem, bool bRoute=true);	
	bool AddForDBTask(const MUID& nPlayerUID, vector<ADD_ITEM>& vecAddItem, bool bRoute=true);	

	bool CheckItemAddable(GEntityPlayer* pPlayer, const CHECK_ADDABLE_ITEM& checkAddableItem);
	bool CheckItemAddable(GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem);	
	
private:
	bool SingleAdd(GEntityPlayer* pPlayer, GItemData* pItemData, int nAddAmount=1);

	bool DB_Insert(GEntityPlayer* pPlayer, int nItemID, int nInsertAmount, int nDurability, const uint8 nTier);
	bool DB_Increase(GEntityPlayer* pPlayer, GItem* pItem, int nIncreaseAmount);

	bool CheckInvenSpace(GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem);
	bool CheckAmount(GEntityPlayer* pPlayer, const CHECK_ADDABLE_ITEM& checkAddableItem);
	bool CheckUnique(GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem);

	GItem*	Server_Insert(GEntityPlayer* pPlayer, uint8 nSlotType, const int nSlotID, const int nInsertAmount, const int64 nIUID, const int nItemID,  
		const int nAddExpiMin=-1, const int nUsagePeriod=0, const int nDurablity=-1, const int nColor=-1);
	void	Server_Increase(GEntityPlayer* pPlayer, uint8 nSlotType, const int nSlotID, int nIncreaseAmount);

	void Route(GEntityPlayer* pPlayer, vector<ADD_ITEM>& vecAddItem);
};
