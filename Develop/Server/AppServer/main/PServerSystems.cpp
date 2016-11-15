#include "stdafx.h"
#include "PServerSystems.h"
#include "PServer.h"
#include "PSystem.h"
#include "PDBManager.h"
#include "PLoginLogic.h"
#include "PMsgCommandFacade.h"
#include "PPMSSystem.h"
#include "PPostOfficeSystem.h"
#include "PTradeMarketService.h"
#include "PTradeMarketServiceConsumer.h"

PServerSystems::PServerSystems() : PGlobalSystem()
{
}

PServerSystems::~PServerSystems()
{

}

void PServerSystems::Create()
{
	pServer						= new PServer();
	pSystem						= new PSystem();
	pDBManager					= new PDBManager();	
	pLoginLogic					= new PLoginLogic();
	pMsgCommandFacade			= new PMsgCommandFacade();
	pPostOfficeSystem			= new PPostOfficeSystem();
	pTradeMarketService			= new PTradeMarketService();
	pTradeMarketServiceConsumer	= new PTradeMarketServiceConsumer();

	gsys.pServer						= pServer;
	gsys.pSystem						= pSystem;
	gsys.pDBManager						= pDBManager;
	gsys.pLoginLogic					= pLoginLogic;
	gsys.pMsgCommandFacade				= pMsgCommandFacade;
	gsys.pPostOfficeSystem				= pPostOfficeSystem;
	gsys.pTradeMarketService			= pTradeMarketService;
	gsys.pTradeMarketServiceConsumer	= pTradeMarketServiceConsumer;
}

void PServerSystems::Destroy()
{
	SAFE_DELETE(pDBManager);
	SAFE_DELETE(pServer);
	SAFE_DELETE(pSystem);
	SAFE_DELETE(pLoginLogic);
	SAFE_DELETE(pMsgCommandFacade);
	SAFE_DELETE(pPostOfficeSystem);
	SAFE_DELETE(pTradeMarketService);
	SAFE_DELETE(pTradeMarketServiceConsumer);

	gsys.SetNull();
}
