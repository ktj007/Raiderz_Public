#include "stdafx.h"
#include "LServerManagers.h"
#include "LCommObjectManager.h"

LServerManagers::LServerManagers() : LGlobalManager()
{
}

LServerManagers::~LServerManagers()
{

}

void LServerManagers::Create()
{
	this->pCommObjectManager = new LCommObjectManager();
	gmgr.pCommObjectManager	= this->pCommObjectManager;
}

void LServerManagers::Destroy()
{
	SAFE_DELETE(this->pCommObjectManager);

	gmgr.SetNull();
}