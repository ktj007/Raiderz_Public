#pragma once

enum SH_ITEM_SLOT_TYPE;
struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;

class ZGuildLogic
{
public:
	void MemberOnlineInfo(MUID uidSender, MUID uidPlayer, const vector<CID>& vecMemberCID);
	void Destroy(int nGID);
	void Join(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	void Leave(CID nCID, int nGID);
	void Kick(CID nCID, int nGID);
	void OnLine(CID nCID, int nGID, int nFieldID, int nChannelID);
	void OffLine(CID nCID, int nGID);
	void MoveField(CID nCID, int nGID, int nFieldID, int nChannelID);
	void ChangeMaster(CID nOldCID, CID nNewCID, int nGID);
	void DepositStorageMoney(MUID uidReqServer, CID nCID, int nGID, int nDepoistMoney, int nStorageMoney);
	void WithdrawStorageMoney(MUID uidReqServer, CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	void MoveStorageItem(MUID uidReqServer, CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);
};
