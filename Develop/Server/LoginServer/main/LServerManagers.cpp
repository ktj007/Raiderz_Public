#include "stdafx.h"
#include "LServerManagers.h"
#include "LPlayerObjectManager.h"
#include "LMasterServerMgr.h"
#include "LTimeoutManager.h"
#include "LPlayerAcceptManager.h"


LServerManagers::LServerManagers() : LGlobalManager()
{
}

LServerManagers::~LServerManagers()
{

}

void LServerManagers::Create()
{
	this->pPlayerObjectManager = new LPlayerObjectManager();
	gmgr.pPlayerObjectManager	= this->pPlayerObjectManager;

	this->pMasterServerMgr = new LMasterServerMgr();
	gmgr.pMasterServerMgr	= this->pMasterServerMgr;

	this->pTimeoutManager = new LTimeoutManager();
	gmgr.pTimeoutManager = this->pTimeoutManager;

	this->pPlayerAcceptManager = new LPlayerAcceptManager();
	gmgr.pPlayerAcceptManager = this->pPlayerAcceptManager;
}

void LServerManagers::Destroy()
{
	SAFE_DELETE(this->pPlayerObjectManager);
	SAFE_DELETE(this->pMasterServerMgr);
	SAFE_DELETE(this->pTimeoutManager);
	SAFE_DELETE(this->pPlayerAcceptManager);

	gmgr.SetNull();
}
