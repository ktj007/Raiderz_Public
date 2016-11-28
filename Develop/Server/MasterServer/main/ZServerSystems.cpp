#include "stdafx.h"
#include "ZServerSystems.h"
#include "ZServer.h"
#include "ZSystem.h"
#include "ZDBManager.h"
#include "ZPartyLogic.h"
#include "ZScriptManager.h"
#include "ZLootLogic.h"
#include "ZCharLogic.h"
#include "ZPMSSystem.h"
#include "ZGuildLogic.h"


ZServerSystems::ZServerSystems() : ZGlobalSystem()
{
}

ZServerSystems::~ZServerSystems()
{

}

void ZServerSystems::Create()
{
	pServer				= new ZServer();
	pSystem				= new ZSystem();
	pDBManager			= new ZDBManager();
	pPartyLogic			= new ZPartyLogic();
	pScriptManager		= new ZScriptManager();
	pLootLogic			= new ZLootLogic();
	pCharLogic			= new ZCharLogic();
	pGuildLogic			= new ZGuildLogic();

	gsys.pServer			= pServer;
	gsys.pSystem			= pSystem;
	gsys.pDBManager			= pDBManager;
	gsys.pPartyLogic		= pPartyLogic;
	gsys.pScriptManager		= pScriptManager;
	gsys.pLootLogic			= pLootLogic;
	gsys.pCharLogic			= pCharLogic;
	gsys.pGuildLogic		= pGuildLogic;
}

void ZServerSystems::Destroy()
{
	SAFE_DELETE(pServer);
	SAFE_DELETE(pSystem);
	SAFE_DELETE(pDBManager);
	SAFE_DELETE(pPartyLogic);
	SAFE_DELETE(pScriptManager);
	SAFE_DELETE(pLootLogic);
	SAFE_DELETE(pCharLogic);
	SAFE_DELETE(pGuildLogic);

	gsys.SetNull();
}
