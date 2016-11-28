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
	void AuthorizeNormalItem(vector<GDropItem*>& outvecDropItem, const vector<CID>& vecBeneficiaryCID, MUID nPartyUID, const LOOTING_RULE_DATA& lootingRuleData);	
	void AuthorizeQuestItem(vector<GDropItem*>& outvecDropItem, CID nCID);
	void AuthorizeQuestPVPItem(vector<GDropItem*>& outvecDropItem, CID nCID);
	void AuthorizeInteractItem(vector<GDropItem*>& outvecDropItem, CID nCID);

private:
	void Authorize(GDropItem* pDropItem, const vector<CID>& vecBeneficiaryCID, DROP_ITEM_TYPE nDIT);
	void Authorize(GDropItem* pDropItem, CID nCurrentOrderCID, DROP_ITEM_TYPE nDIT);		

	CID GetRoundRobinCurrentOdrer(const vector<CID>& vecBeneficiaryCID, MUID nPartyUID);
	void SetRoundRobinLastOrder(MUID nPartyUID, CID nLastOrderCID);

	bool IsRareItem(int nItemID, LOOTING_RULE_RARE_FOR nLRRF);	
};
