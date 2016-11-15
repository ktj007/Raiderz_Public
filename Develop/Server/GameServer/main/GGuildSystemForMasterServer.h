#pragma once

struct TD_GUILD_ONLINE_MEMBER_INFO;
struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;
class GGuild;
class GItem;


class GGuildSystemForMasterServer : public MTestMemPool<GGuildSystemForMasterServer>
{
public:
	void MemberOnlineInfo(MUID uidPlayer, const vector<TD_GUILD_ONLINE_MEMBER_INFO>& vecOnlineMemberInfo);
	void DestroySync(int nGID);
	void JoinSync(int nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	void LeaveSync(int nCID, int nGID);
	void KickSync(int nCID, int nGID);
	void OnLineSync(int nCID, int nGID, int nFieldID, int nChannelID);
	void OffLineSync(int nCID, int nGID);
	void MoveFieldSync(int nCID, int nGID, int nFieldID, int nChannelID);
	void ChangeMasterSync(int nOldCID, int nNewCID, int nGID);
	void DepositStorageMoneySync(int nCID, int nGID, int nDepositMoney, int nStorageMoney);
	void WithdrawStorageMoneySync(int nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	void MoveStorageItemSync(int nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);

private:
	void MoveStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
	void SplitStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID);
	void MergeStorageItem(GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, GItem* pToItem , SH_ITEM_SLOT_TYPE nToSlotType, int nAmount);
	void SwpaStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, GItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
};



