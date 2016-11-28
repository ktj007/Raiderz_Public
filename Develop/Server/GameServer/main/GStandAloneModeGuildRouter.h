#pragma once

#include "GServerGuildRouter.h"

class GStandAloneModeGuildRouter : public GServerGuildRouter
{
public:
	virtual void GuilInfo(MUID uidPlayer, int nGID);
	virtual void Destroy(int nGID);
	virtual void Join(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID);
	virtual void Leave(CID nCID, int nGID);
	virtual void Kick(CID nCID, int nGID);
	virtual void OnLine(CID nCID, int nGID, int nFieldID, int nChannelID);
	virtual void OffLine(CID nCID, int nGID);
	virtual void MoveField(CID nCID, int nGID, int nFieldID, int nChannelID);
	virtual void ChangeMaster(CID nOldCID, CID nNewCID, int nGID);
	virtual void DepositStorageMoney(CID nCID, int nGID, int nDepoistMoney, int nStorageMoney);
	virtual void WithdrawStorageMoney(CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney);
	virtual void MoveStorageItem(CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem);
};
