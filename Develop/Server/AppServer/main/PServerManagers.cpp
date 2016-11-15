#include "stdafx.h"
#include "PServerManagers.h"
#include "PGameServerObjectManager.h"
#include "PChatChannelManager.h"
#include "PFieldInfoManager.h"

PServerManagers::PServerManagers() : PGlobalManager()
{
}

PServerManagers::~PServerManagers()
{

}

void PServerManagers::Create()
{
	this->pGameServerObjectManager = new PGameServerObjectManager();
	gmgr.pGameServerObjectManager	= this->pGameServerObjectManager;

	this->pChatChannelManager = new PChatChannelManager();
	gmgr.pChatChannelManager	= this->pChatChannelManager;

	this->pFieldInfoManager = new PFieldInfoManager();
	gmgr.pFieldInfoManager = this->pFieldInfoManager;
}

void PServerManagers::Destroy()
{
	SAFE_DELETE(this->pGameServerObjectManager);
	SAFE_DELETE(this->pChatChannelManager);
	SAFE_DELETE(this->pFieldInfoManager);

	gmgr.SetNull();
}
