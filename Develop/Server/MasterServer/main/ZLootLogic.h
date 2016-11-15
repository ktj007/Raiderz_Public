#pragma once

struct TD_LOOT_MASTERLOOT_UI;
struct TD_LOOT_MASTERLOOT_NOTIFY;
struct TD_LOOT_ROLL_UI;
struct TD_LOOT_OTHERGAIN_ITEM;
struct TD_LOOT_GETTABLE_ITEM_ADD;
struct TD_LOOT_ROLL_ITEM;
struct TD_LOOT_ROLL_RESULT;

// 루팅 관련 게임서버 요청 처리

class ZLootLogic
{
public:
	ZLootLogic(void);
	~ZLootLogic(void);

	void MasterLootNotifyReq(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_MASTERLOOT_NOTIFY>& vecTDMasterLootNotify);	
	void OtherGainitemReq(const vector<int>& vecBeneficiaryCID, MUID nGainnerUID, const vector<TD_LOOT_OTHERGAIN_ITEM>& vecTDOtherGainItem);
	void GettableItemAddReq(int nBeneficiaryCID, const vector<TD_LOOT_GETTABLE_ITEM_ADD>& vecTDGettableItemAdd);
	void RollResultReq(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult);
};
