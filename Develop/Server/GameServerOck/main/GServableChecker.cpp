#include "StdAfx.h"
#include "GServableChecker.h"
#include "GServerInfo.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GMasterServerFacade.h"
#include "GAppServerFacade.h"
#include "GManagedFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GSharedFieldMaster.h"
#include "GConfig.h"


bool GServableChecker::Check(const float fDelta)
{
	//////////////////////////////////////////////////////////////////////////
	///
	if (Check_IsStandAlone())
	{
		SetServerStateActive(true);
		return true;
	}
	///
	//////////////////////////////////////////////////////////////////////////

	if (!Check_IsConnectedMasterServer()) return false;
	if (!Check_IsConnectedAppServer()) return false;
	if (!Check_ManagedFIeldInfoCountFromMasterServer()) return false;
	if (!Check_CreatingSharedFIeld()) return false;

	SetServerStateActive(true);
	return true;
}

void GServableChecker::SetServerStateActive(bool bIsActive)
{
	if (bIsActive)
	{
		if (SERVER_STAT_ACTIVE == gsys.pServer->GetServerInfo().nStat)
			return;

		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_ACTIVE;
		mlog("server start! (Servable)\n");
	}
	else
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
	}
}



bool GServableChecker::Check_IsStandAlone()
{
	return GConfig::m_bStandAlone;
}

bool GServableChecker::Check_IsConnectedMasterServer()
{
	if (false == gsys.pMasterServerFacade->GetNetClient()->IsConnected())
	{
		SetServerStateActive(false);
		mlog("Not Servable. Not Connected To MasterServer.\n");
		return false;
	}

	return true;
}

bool GServableChecker::Check_IsConnectedAppServer()
{
	if (NULL == gsys.pAppServerFacade->GetNetClient())
	{
		SetServerStateActive(false);
		mlog("Not Servable. Not Created AppServer Network.\n");
		return false;
	}

	if (false == gsys.pAppServerFacade->GetNetClient()->IsConnected())
	{
		SetServerStateActive(false);
		mlog("Not Servable. Not Connected To AppServer.\n");
		return false;
	}

	return true;
}

bool GServableChecker::Check_ManagedFIeldInfoCountFromMasterServer()
{
	if (0 == gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->size())
	{
		SetServerStateActive(false);
		mlog("Not Servable. ManagedFIeldInfoCount receive from MasterServer is ZERO.\n");
		return false;
	}

	return true;
}

bool GServableChecker::Check_CreatingSharedFIeld()
{
	int nSharedFieldMasterQty = gmgr.pFieldMgr->GetSharedFieldQty();
	if (0 == nSharedFieldMasterQty)
	{
		SetServerStateActive(false);
		mlog("Not Servable. Created SharedFieldCount is ZERO.\n");
		return false;
	}

	int nManagedFIeldInfoQty = gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->size();
	if (nManagedFIeldInfoQty > nSharedFieldMasterQty)
	{
		SetServerStateActive(false);
		mlog("Not Servable. Low GameServer SharedFIeldCount Than ManagedFieldInfo(From MasterServer)Count.\n");
		return false;
	}

	int nWholeChannelCount = 0;
	GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator it = gmgr.pFieldMgr->GetSharedFieldMap().begin();
	for (; it != gmgr.pFieldMgr->GetSharedFieldMap().end(); ++it)
	{
		GSharedFieldMaster* pSharedFieldMaster = (*it).second;
		nWholeChannelCount += pSharedFieldMaster->GetChannels().size();
	}
	if (gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->GetAllChannelCount() != nWholeChannelCount)
	{
		SetServerStateActive(false);
		mlog("Not Servable. Not Equal Created WholeChannelCount And ManagedFieldChannel(From MasterServer)Count.\n");
		return false;
	}

	return true;
}
