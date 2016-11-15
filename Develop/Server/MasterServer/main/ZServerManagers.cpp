#include "stdafx.h"
#include "ZServerManagers.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerInfoManager.h"
#include "ZLoginServerFacade.h"
#include "ZLoginServerInfoManager.h"
#include "ZAppServerObjectManager.h"
#include "ZAppServerInfoManager.h"
#include "ZFieldInfoManager.h"
#include "ZFieldManager.h"
#include "ZPlayerManager.h"
#include "ZDuplicationLoginManager.h"
#include "ZPartyManager.h"
#include "ZFixedPartyInfoManager.h"

ZServerManagers::ZServerManagers() : ZGlobalManager()
{
}

ZServerManagers::~ZServerManagers()
{

}

void ZServerManagers::Create()
{
	this->pGameServerInfoManager = new ZGameServerInfoManager();
	gmgr.pGameServerInfoManager	= this->pGameServerInfoManager; 

	this->pGameServerObjectManager = new ZGameServerObjectManager();
	gmgr.pGameServerObjectManager	= this->pGameServerObjectManager;

	this->pLoginServerInfoManager = new ZLoginServerInfoManager();
	gmgr.pLoginServerInfoManager	= this->pLoginServerInfoManager; 

	this->pLoginServerFacade = new ZLoginServerFacade();
	gmgr.pLoginServerFacade	= this->pLoginServerFacade;

	this->pAppServerInfoManager = new ZAppServerInfoManager();
	gmgr.pAppServerInfoManager = this->pAppServerInfoManager;

	this->pAppServerObjectManager = new ZAppServerObjectManager();
	gmgr.pAppServerObjectManager = this->pAppServerObjectManager;

	this->pFieldInfoManager = new ZFieldInfoManager();
	gmgr.pFieldInfoManager	= this->pFieldInfoManager;

	this->pFieldManager = new ZFieldManager();
	gmgr.pFieldManager	= this->pFieldManager; 

	this->pPlayerManager = new ZPlayerManager();
	gmgr.pPlayerManager	= this->pPlayerManager;

	this->pDuplicationLoginManager = new ZDuplicationLoginManager();
	gmgr.pDuplicationLoginManager = this->pDuplicationLoginManager;

	this->pPartyManager = new ZPartyManager();
	gmgr.pPartyManager = this->pPartyManager;

	this->pFixedPartyInfoManager = new ZFixedPartyInfoManager();
	gmgr.pFixedPartyInfoManager = this->pFixedPartyInfoManager;
}

void ZServerManagers::Destroy()
{
	SAFE_DELETE(this->pFixedPartyInfoManager);
	SAFE_DELETE(this->pPartyManager);
	SAFE_DELETE(this->pDuplicationLoginManager);
	SAFE_DELETE(this->pPlayerManager);
	SAFE_DELETE(this->pFieldManager);
	SAFE_DELETE(this->pGameServerObjectManager);
	SAFE_DELETE(this->pLoginServerFacade);
	SAFE_DELETE(this->pAppServerObjectManager);

	SAFE_DELETE(this->pGameServerInfoManager);
	SAFE_DELETE(this->pFieldInfoManager);
	SAFE_DELETE(this->pLoginServerInfoManager);
	SAFE_DELETE(this->pAppServerInfoManager);

	gmgr.SetNull();
}
