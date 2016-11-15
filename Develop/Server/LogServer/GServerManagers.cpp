#include "stdafx.h"
#include "GServerManagers.h"
#include "GGameServerMgr.h"

GServerManagers::GServerManagers() : GGlobalManager()
{
}

GServerManagers::~GServerManagers()
{

}

void GServerManagers::Create()
{
	pGameServerMgr	= new GGameServerMgr();

	gmgr.pGameServerMgr	= pGameServerMgr;
}

void GServerManagers::Destroy()
{
	SAFE_DELETE(pGameServerMgr);

	gmgr.SetNull();
}