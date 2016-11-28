#pragma once

struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;

class GServerGuildRouter
{
public:
	virtual void GuilInfo(MUID uidPlayer, int nGID) = 0;
	virtual void Destroy(int nGID) = 0;
	virtual void Join(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID) = 0;
	virtual void Leave(CID nCID, int nGID) = 0;
	virtual void Kick(CID nCID, int nGID) = 0;
	virtual void OnLine(CID nCID, int nGID, int nFieldID, int nChannelID) = 0;
	virtual void OffLine(CID nCID, int nGID) = 0;
	virtual void MoveField(CID nCID, int nGID, int nFieldID, int nChannelID) = 0;
	virtual void ChangeMaster(CID nOldCID, CID nNewCID, int nGID) = 0;
	virtual void DepositStorageMoney(CID nCID, int nGID, int nDepoistMoney, int nStorageMoney) = 0;
	virtual void WithdrawStorageMoney(CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney) = 0;
	virtual void MoveStorageItem(CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem) = 0;
};
