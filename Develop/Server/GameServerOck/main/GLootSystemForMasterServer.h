#pragma once

struct TD_LOOT_MASTERLOOT_UI;
struct TD_LOOT_MASTERLOOT_NOTIFY;
struct TD_LOOT_OTHERGAIN_ITEM;
struct TD_LOOT_GETTABLE_ITEM_ADD;
struct TD_LOOT_ROLL_ITEM;
struct TD_LOOT_ROLL_RESULT;

// 루팅 관련 마스터 서버 요청 처리

class GLootSystemForMasterServer : public MTestMemPool<GLootSystemForMasterServer>
{
public:
	GLootSystemForMasterServer(void);
	~GLootSystemForMasterServer(void);

	void MasterLootNotify(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_MASTERLOOT_NOTIFY>& vecTDMasterLootNotify);
	void OtherGainItem(const vector<int>& vecBeneficiaryCID, const MUID& nGainerUID, const vector<TD_LOOT_OTHERGAIN_ITEM>& vecTDOtherGainItem);
	void GettableItemAdd(int nBeneficiaryCID, const vector<TD_LOOT_GETTABLE_ITEM_ADD>& vecTDGettableItemAdd);
	void RollResult(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult);
};
