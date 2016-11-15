#include "StdAfx.h"
#include "ZAppServerObject.h"
#include "CTransData.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "ZGlobal.h"

ZAppServerObject::ZAppServerObject(MUID uidObject, ZAppServerInfo* pAppServerInfo)
: SCommObject(uidObject)
, m_pAppServerInfo(pAppServerInfo)
, m_nIP(0)
, m_nPort(0)
, m_eMode(APP_SERVER_CHAT)
{
	// do nothing
}

ZAppServerObject::~ZAppServerObject()
{
	// do nothing
}

uint32 ZAppServerObject::GetIP(void)
{
	return m_nIP;
}

int ZAppServerObject::GetPort(void)
{
	return m_nPort;
}

SAppServerMode ZAppServerObject::GetMode(void)
{
	return m_eMode;
}

void ZAppServerObject::SetIP(uint32 nIP)
{
	m_nIP = nIP;
}

void ZAppServerObject::SetPort(int nPort)
{
	m_nPort = nPort;
}

void ZAppServerObject::SetMode(SAppServerMode eAppServerMode)
{
	m_eMode = eAppServerMode;
}
