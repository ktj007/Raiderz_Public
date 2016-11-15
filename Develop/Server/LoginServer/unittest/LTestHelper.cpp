#include "stdafx.h"
#include "LTestHelper.h"
#include "MockLink.h"
#include "MockNetServer.h"
#include "LGlobal.h"
#include "LCommandCenter.h"
#include "LUnitTestUtil.h"
#include "MockPlayerObject.h"
#include "LPlayerObjectManager.h"

MUID LTestHelper::m_uidGenerator = MUID(2545425, 0);
int LTestHelper::m_IDGenerator = 0;
int LTestHelper::m_nName = 0;

MockLink* LTestHelper::NewLink( MUID uidNew, bool bIgnoreAllSendCommandID /*= false*/ )
{
	if (uidNew.IsInvalid()) uidNew = NewUID();

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

void LTestHelper::ClearLinks()
{
	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return;

	pMockNetServer->ClearMockLinks();
}

MUID LTestHelper::NewUID()
{
	return (++m_uidGenerator);
}

int LTestHelper::NewID()
{
	return (++m_IDGenerator);
}

tstring LTestHelper::NewName()
{
	m_nName++;

	CString strName;
	strName.Format(_T("%d"), m_nName);

	return strName.GetString();
}

MockPlayerObject* LTestHelper::NewMockPlayerObject(MUID uidNew)
{
	if (uidNew == MUID::ZERO) uidNew = LUnitTestUtil::NewUID();

	MockPlayerObject* pNewPlayerObject = new MockPlayerObject(uidNew);
	pNewPlayerObject->Create();
	gmgr.pPlayerObjectManager->AddObject(pNewPlayerObject);
	return pNewPlayerObject;
}
