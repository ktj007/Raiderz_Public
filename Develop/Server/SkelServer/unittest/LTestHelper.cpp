#include "stdafx.h"
#include "LTestHelper.h"
#include "MockLink.h"
#include "MockNetServer.h"
#include "LGlobal.h"
#include "LCommandCenter.h"

MUID ZTestHelper::m_uidGenerator = MUID(2545425, 0);

MockLink* ZTestHelper::NewLink( bool bIgnoreAllSendCommandID /*= false*/ )
{
	MUID uidNew = NewUID();

	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return NULL;

	MockLink* pNewMockLink = dynamic_cast<MockLink*>(pMockNetServer->AddMockLink(uidNew));
	if (!pNewMockLink) return NULL;

	if (bIgnoreAllSendCommandID)
	{
		pNewMockLink->IgnoreAll();
	}

	return pNewMockLink;

}

void ZTestHelper::ClearLinks()
{
	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return;

	pMockNetServer->ClearMockLinks();
}

MUID ZTestHelper::NewUID()
{
	return (++m_uidGenerator);
}