#pragma once

#include "CSDef_Loot.h"

class GEntityPlayer;
class GLootInfo;
class GDropItem;
class GEntityNPC;
class GParty;
class GDropItemAuthorizer;

struct TD_ITEM_DROP;
struct LOOT_ITEM_INFO;
struct LOOTING_RULE_DATA;

enum LOOTING_RULE_RARE_FOR;

// 아이템 드랍
// 드랍된 아이템 관리

class GDropItems : public MTestMemPool<GDropItems>
{
public:
	GDropItems(GEntityNPC* pOwner);
	~GDropItems();

	void Clear();

	void DropByKill(GLootInfo* pLootInfo, CID nBeneficiaryCID, MUID nPartyUID, const LOOTING_RULE_DATA& lootingRuleData);
	void DropByKill(GLootInfo* pLootInfo, const vector<CID>& vecBeneficiaryCID, MUID nPartyUID, const LOOTING_RULE_DATA& lootingRuleData);	
	void DropByInteract(CID nBeneficiaryCID, GLootInfo* pLootInfo);

	void Remove(const MUID& nDropItemUID);


public: //helper
	const map<MUID, GDropItem*>& GetContainer();
	vector<TD_ITEM_DROP> MakeVisbleTDDropItem(CID nCID);

	bool IsEmpty();
	bool IsExist(const MUID& nDropItemUID);
	bool IsAuthorizedCID(CID nCID);
	bool IsViewableCID(CID nCID);	

	void GetViewableCID(set<CID>& outsetViewableCID);	
	GDropItem* GetDropItem(const MUID& nDropItemUID);
	vector<GDropItem*>	CollectDropItemByCID(CID nCID);
	vector<MUID>		CollectDropItemUIDByCID(CID nCID);
	int GetSumOfAmount();	

private:
	bool CheckBreakParts(LOOT_ITEM_INFO* pLootItemInfo);	

	int CalculLimitDropQuantity(LOOT_ITEM_INFO* pLootItemInfo, int nLimitDropQuantity);

	int DropNormalItem(const vector<CID>& vecBeneficiaryCID, MUID nPartyUID, LOOT_ITEM_INFO* pLootItemInfo, int nLimitDropQuantity, int nSumDropQuantity);	
	void DropQuestItem(const vector<CID>& vecBeneficiaryCID, LOOT_ITEM_INFO* pLootItemInfo);
	void DropQuestPVPItem(const vector<CID>& vecBeneficiaryCID, LOOT_ITEM_INFO* pLootItemInfo);
	int DropInteractItem(CID nBeneficiaryCID, LOOT_ITEM_INFO* pLootItemInfo, int nLimitDropQuantity, int nSumDropQuantity);

	vector<GDropItem*> DropItem(LOOT_ITEM_INFO* pLootItemInfo, int nLimitQuantity=INT_MAX);		
	void MakeDropItem( LOOT_ITEM_INFO* pLootItemInfo, vector<GDropItem*>& outVecDropItem, int nLimitQuantity);	
	void StoreDropItem(const vector<GDropItem*>& vecDropItem);

	MUID MakeNewDropItemUID();	

private:
	GEntityNPC*				m_pOwner;
	map<MUID, GDropItem*>	m_mapDropItem;
	MUID					m_nNowDropItemUID;

	LOOTING_RULE_DATA		m_lootingRuleData;

	GDropItemAuthorizer*	m_pDropItemAuthorizer;
};