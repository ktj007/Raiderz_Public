#pragma once

#include "GServerGuildRouter.h"

class GStandAloneModeGuildRouter : public GServerGuildRouter
{
public:
	virtual void GuilInfo(MUID uidPlayer, int nGID);
	virtual void Destroy(int nGID);
	virtual void Join(int nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	virtual void Leave(int nCID, int nGID);
	virtual void Kick(int nCID, int nGID);
	virtual void OnLine(int nCID, int nGID, int nFieldID, int nChannelID);
	virtual void OffLine(int nCID, int nGID);
	virtual void MoveField(int nCID, int nGID, int nFieldID, int nChannelID);
	virtual void ChangeMaster(int nOldCID, int nNewCID, int nGID);
	virtual void DepositStorageMoney(int nCID, int nGID, int nDepoistMoney, int nStorageMoney);
	virtual void WithdrawStorageMoney(int nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	virtual void MoveStorageItem(int nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);
};
