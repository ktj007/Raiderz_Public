#include "stdafx.h"
#include "LServerSystems.h"
#include "LServer.h"
#include "LSystem.h"

LServerSystems::LServerSystems() : LGlobalSystem()
{
}

LServerSystems::~LServerSystems()
{

}

void LServerSystems::Create()
{
	pServer				= new LServer();
	pSystem				= new LSystem();

	gsys.pServer			= pServer;
	gsys.pSystem			= pSystem;
;
}

void LServerSystems::Destroy()
{
	SAFE_DELETE(pServer);
	SAFE_DELETE(pSystem);

	gsys.SetNull();
}

