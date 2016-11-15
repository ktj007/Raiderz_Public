#pragma once

enum LOOTING_RULE_RARE_FOR;
enum LOOTING_RULE_RARE;
enum LOOTING_RULE_COMMON;

class GDropItem;
struct LOOTING_RULE_DATA;
enum DROP_ITEM_TYPE;

class GDropItemAuthorizer : public MTestMemPool<GDropItemAuthorizer>
{
public:	
	void AuthorizeNormalItem(vector<GDropItem*>& outvecDropItem, const vector<int>& vecBeneficiaryCID, MUID nPartyUID, const LOOTING_RULE_DATA& lootingRuleData);	
	void AuthorizeQuestItem(vector<GDropItem*>& outvecDropItem, int nCID);
	void AuthorizeQuestPVPItem(vector<GDropItem*>& outvecDropItem, int nCID);
	void AuthorizeInteractItem(vector<GDropItem*>& outvecDropItem, int nCID);

private:
	void Authorize(GDropItem* pDropItem, const vector<int>& vecBeneficiaryCID, DROP_ITEM_TYPE nDIT);
	void Authorize(GDropItem* pDropItem, int nCurrentOrderCID, DROP_ITEM_TYPE nDIT);		

	int GetRoundRobinCurrentOdrer(const vector<int>& vecBeneficiaryCID, MUID nPartyUID);
	void SetRoundRobinLastOrder(MUID nPartyUID, int nLastOrderCID);

	bool IsRareItem(int nItemID, LOOTING_RULE_RARE_FOR nLRRF);	
};
