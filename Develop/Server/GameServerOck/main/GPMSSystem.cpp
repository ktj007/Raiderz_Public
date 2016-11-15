#include "StdAfx.h"
#include "GPMSSystem.h"
#include "GPMSObject.h"
#include "GConfig.h"
#include "PMServerInitLogger.h"

GPMSSystem::GPMSSystem(void)
{
	SetGameInfo(GConfig::m_SSN, GConfig::m_nMyWorldID);
	pCallbackObj = new GPMSObject(this);
}

GPMSSystem::~GPMSSystem(void)
{
	SAFE_DELETE(pCallbackObj);
}

IPMSObject* GPMSSystem::GetPMSObject()
{
	return pCallbackObj;
}