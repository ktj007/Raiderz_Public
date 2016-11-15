#pragma once

struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;

class GServerGuildRouter
{
public:
	virtual void GuilInfo(MUID uidPlayer, int nGID) = 0;
	virtual void Destroy(int nGID) = 0;
	virtual void Join(int nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID) = 0;
	virtual void Leave(int nCID, int nGID) = 0;
	virtual void Kick(int nCID, int nGID) = 0;
	virtual void OnLine(int nCID, int nGID, int nFieldID, int nChannelID) = 0;
	virtual void OffLine(int nCID, int nGID) = 0;
	virtual void MoveField(int nCID, int nGID, int nFieldID, int nChannelID) = 0;
	virtual void ChangeMaster(int nOldCID, int nNewCID, int nGID) = 0;
	virtual void DepositStorageMoney(int nCID, int nGID, int nDepoistMoney, int nStorageMoney) = 0;
	virtual void WithdrawStorageMoney(int nCID, int nGID, int nWithdrawMoney, int nStorageMoney) = 0;
	virtual void MoveStorageItem(int nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem) = 0;
};
