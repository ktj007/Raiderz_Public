#include "stdafx.h"
#include "GServerSystems.h"
#include "GServer.h"
#include "GSystem.h"
#include "GDBManager.h"

GServerSystems::GServerSystems() : GGlobalSystem()
{
}

GServerSystems::~GServerSystems()
{

}

void GServerSystems::Create()
{
	pServer				= new GServer();
	pSystem				= new GSystem();
	pDBManager			= new GDBManager();

	gsys.pServer			= pServer;
	gsys.pSystem			= pSystem;
	gsys.pDBManager			= pDBManager;
;
}

void GServerSystems::Destroy()
{
	SAFE_DELETE(pServer);
	SAFE_DELETE(pSystem);
	SAFE_DELETE(pDBManager);

	gsys.SetNull();
}

