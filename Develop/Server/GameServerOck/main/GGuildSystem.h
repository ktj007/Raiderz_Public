#ifndef _G_GUILD_SYSTEM_H_
#define _G_GUILD_SYSTEM_H_

#include "MCommand.h"
using namespace minet;

#include "CTransData.h"

class GGuild;
class GEntityPlayer;
class GDBTaskGuildSerializeData;
class GGuildSystemForDBTask;
class GGuildSystemForMasterServer;

class GGuildSystem : public MTestMemPool<GGuildSystem>
{
public:
	GGuildSystem();
	~GGuildSystem();	

	enum
	{
		GUILD_ERR_DUPLICATED_NAME = 0,
	};

	GGuildSystemForDBTask& ForDBTask();
	GGuildSystemForMasterServer& ForMasterServer();

	void Serialize(GEntityPlayer* pPlayer);
	bool Create(GEntityPlayer* pPlayer, const wchar_t* szGuildName);

	bool Invite_Req(GEntityPlayer* pInvitor, GEntityPlayer* pGuest);
	bool Invite_Res(GEntityPlayer* pGuest, bool bAccept);

	bool Leave(GEntityPlayer* pPlayer);
	bool Kick(GEntityPlayer* pPlayer, const wchar_t* szTargetName);

	bool Destroy(GEntityPlayer* pPlayer);

	void OnLine(GEntityPlayer* pPlayer);
	void OffLine(GEntityPlayer* pPlayer);
	void MoveField(GEntityPlayer* pPlayer);
	bool ChangeMaster(GEntityPlayer* pOldMaster, const wchar_t* szName);

	bool ShowStorage(GEntityPlayer* pPlayer);
	bool DepositStorageMoney(GEntityPlayer* pPlayer, int nMoney);
	bool WithdrawStorageMoney(GEntityPlayer* pPlayer, int nMoney);

public:
	void RouteToMember(GGuild* pGuild, MCommand* pCmd, int nExceptMemberCID=0);
	void RouteToStorageInteractionMember(GGuild* pGuild, MCommand* pCmd, int nExceptMemberCID=0);
	void RouteGuildInfo(const MUID& uidPlayer, const int nGID);
	void RouteReject(GEntityPlayer* pPlayer, const MUID& uidRejector);
	void RouteJoinSector(GEntityPlayer* pPlayer, const wstring& strGuildName);	
	void RouteLeaveSector(GEntityPlayer* pPlayer);	
	void RouteInsertGuildFailed(const MUID& uidCreator, const int nErrCode);
	bool RouteShowCreateUI(GEntityPlayer* pPlayer);
	bool RouteShowDestroyUI(GEntityPlayer* pPlayer);
	void RouteSyncFail(MUID uidPlayer);

	void MakeTD_GUILDAndTD_GUILD_MEMBER(int nGID, vector<TD_GUILD>& outvecTDGuild, vector<TD_GUILD_MEMBER>& outvecTDGuildMember);

protected :
	GGuildSystemForDBTask*			m_pForDBTask;
	GGuildSystemForMasterServer*	m_pMasterServer;
};

#endif
