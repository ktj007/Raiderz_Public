#pragma once

enum SH_ITEM_SLOT_TYPE;
struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;

class ZGuildLogic
{
public:
	void MemberOnlineInfo(MUID uidSender, MUID uidPlayer, const vector<int>& vecMemberCID);
	void Destroy(int nGID);
	void Join(int nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	void Leave(int nCID, int nGID);
	void Kick(int nCID, int nGID);
	void OnLine(int nCID, int nGID, int nFieldID, int nChannelID);
	void OffLine(int nCID, int nGID);
	void MoveField(int nCID, int nGID, int nFieldID, int nChannelID);
	void ChangeMaster(int nOldCID, int nNewCID, int nGID);
	void DepositStorageMoney(MUID uidReqServer, int nCID, int nGID, int nDepoistMoney, int nStorageMoney);
	void WithdrawStorageMoney(MUID uidReqServer, int nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	void MoveStorageItem(MUID uidReqServer, int nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);
};
