#include "StdAfx.h"
#include "GStandAloneModeGuildRouter.h"
#include "GGuildSystem.h"
#include "GGlobal.h"
#include "GGuildMgr.h"
#include "GGuildSystemForMasterServer.h"
#include "STransData_M2G.h"


void GStandAloneModeGuildRouter::GuilInfo(MUID uidPlayer, int nGID)
{
	gsys.pGuildSystem->ForMasterServer().MemberOnlineInfo(uidPlayer, vector<TD_GUILD_ONLINE_MEMBER_INFO>());
}

void GStandAloneModeGuildRouter::Destroy(int nGID)
{
	gsys.pGuildSystem->ForMasterServer().DestroySync(nGID);
}

void GStandAloneModeGuildRouter::Join(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID)
{
	gsys.pGuildSystem->ForMasterServer().JoinSync(nCID, nAID, nGID, strName, nLevel, nFieldID, nChannelID);
}

void GStandAloneModeGuildRouter::Leave( CID nCID, int nGID )
{
	gsys.pGuildSystem->ForMasterServer().LeaveSync(nCID, nGID);
}

void GStandAloneModeGuildRouter::Kick( CID nCID, int nGID )
{
	gsys.pGuildSystem->ForMasterServer().KickSync(nCID, nGID);
}

void GStandAloneModeGuildRouter::OnLine( CID nCID, int nGID, int nFieldID, int nChannelID )
{
	gsys.pGuildSystem->ForMasterServer().OnLineSync(nCID, nGID, nFieldID, nChannelID);
}

void GStandAloneModeGuildRouter::MoveField(CID nCID, int nGID, int nFieldID, int nChannelID)
{
	gsys.pGuildSystem->ForMasterServer().MoveFieldSync(nCID, nGID, nFieldID, nChannelID);
}

void GStandAloneModeGuildRouter::OffLine( CID nCID, int nGID )
{
	gsys.pGuildSystem->ForMasterServer().OffLineSync(nCID, nGID);
}

void GStandAloneModeGuildRouter::ChangeMaster( CID nOldCID, CID nNewCID, int nGID )
{
	gsys.pGuildSystem->ForMasterServer().ChangeMasterSync(nOldCID, nNewCID, nGID);
}

void GStandAloneModeGuildRouter::DepositStorageMoney( CID nCID, int nGID, int nDepoistMoney, int nStorageMoney )
{
	
}

void GStandAloneModeGuildRouter::WithdrawStorageMoney( CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney )
{
	
}

void GStandAloneModeGuildRouter::MoveStorageItem(CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem)
{
	
}
