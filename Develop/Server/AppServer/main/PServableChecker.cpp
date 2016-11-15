#include "StdAfx.h"
#include "PServableChecker.h"
#include "PServerInfo.h"
#include "PGlobal.h"
#include "PServer.h"

#include "PMasterServerFacade.h"
#include "PMasterNetClient.h"


bool PServableChecker::Check(const float fDelta)
{
	if (!Check_IsConnectedMasterServer()) return false;

	if (SERVER_STAT_ACTIVE != gsys.pServer->GetServerInfo().nStat)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_ACTIVE;
		mlog("server start! (Servable)\n");
	}

	return true;
}

bool PServableChecker::Check_IsConnectedMasterServer()
{
	if (false == gsys.pMasterServerFacade->GetNetClient()->IsConnected())
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. Not Connected To MasterServer.\n");
		return false;
	}
	return true;
}

