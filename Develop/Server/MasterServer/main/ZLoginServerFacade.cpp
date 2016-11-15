#include "StdAfx.h"
#include "ZLoginServerFacade.h"
#include "ZLoginServerInfo.h"

ZLoginServerFacade::ZLoginServerFacade()
: m_pLoginServerInfo(NULL)
{

}

ZLoginServerFacade::~ZLoginServerFacade()
{

}

bool ZLoginServerFacade::Connected( MUID uid, ZLoginServerInfo* pLoginServerInfo )
{
	m_UID = uid;
	m_pLoginServerInfo = pLoginServerInfo;
	return true;
}

void ZLoginServerFacade::Disconnect()
{
	m_UID.SetZero();
	m_pLoginServerInfo = NULL;
}

MUID ZLoginServerFacade::GetUID()
{
	return m_UID;
}

int ZLoginServerFacade::GetID()
{
	if (m_pLoginServerInfo == NULL) return 0;
	return m_pLoginServerInfo->m_nID;
}