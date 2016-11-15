#include "stdafx.h"
#include "PTestHelper.h"
#include "MockLink.h"
#include "MockNetServer.h"
#include "PGlobal.h"
#include "PCommandCenter.h"
#include "SUnitTestUtil.h"

MockLink* PTestHelper::NewLink( MUID uidNew, bool bIgnoreAllSendCommandID /*= false*/ )
{
	if (uidNew.IsInvalid()) uidNew = SUnitTestUtil::NewUID();

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

void PTestHelper::ClearLinks()
{
	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return;

	pMockNetServer->ClearMockLinks();
}
