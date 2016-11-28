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
	void JoinSync(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	void LeaveSync(CID nCID, int nGID);
	void KickSync(CID nCID, int nGID);
	void OnLineSync(CID nCID, int nGID, int nFieldID, int nChannelID);
	void OffLineSync(CID nCID, int nGID);
	void MoveFieldSync(CID nCID, int nGID, int nFieldID, int nChannelID);
	void ChangeMasterSync(CID nOldCID, CID nNewCID, int nGID);
	void DepositStorageMoneySync(CID nCID, int nGID, int nDepositMoney, int nStorageMoney);
	void WithdrawStorageMoneySync(CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	void MoveStorageItemSync(CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);

private:
	void MoveStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
	void SplitStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID);
	void MergeStorageItem(GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, GItem* pToItem , SH_ITEM_SLOT_TYPE nToSlotType, int nAmount);
	void SwpaStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, GItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
};



