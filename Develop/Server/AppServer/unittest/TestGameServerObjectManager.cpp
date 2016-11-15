#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"
#include "SUnitTestUtil.h"

SUITE(GameServerObjectManager)
{
	struct Fixture
	{
		Fixture()
		{
			m_pTestGameServerObjectManager = new PGameServerObjectManager();
		}
		~Fixture()
		{
			SAFE_DELETE(m_pTestGameServerObjectManager);
		}

		PGameServerObject* CreateGameServer()
		{
			MUID uidNew = SUnitTestUtil::NewUID();
			int nServerID = SUnitTestUtil::NewID();
			SCommObject* pGameServerObject = m_pTestGameServerObjectManager->NewObject(uidNew, nServerID);
			m_pTestGameServerObjectManager->AddObject(pGameServerObject);
			return static_cast<PGameServerObject*>(pGameServerObject);
		}

		PGameServerObjectManager*	m_pTestGameServerObjectManager;
	};

	TEST_FIXTURE(Fixture, TestGameServerObjectManager_AddObject)
	{
		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetGameServerIDCount());

		MUID uidNew = SUnitTestUtil::NewUID();
		int nServerID = 10;
		SCommObject* pGameServerObject = m_pTestGameServerObjectManager->NewObject(uidNew, nServerID);
		m_pTestGameServerObjectManager->AddObject(pGameServerObject);

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());

		m_pTestGameServerObjectManager->AddObject(pGameServerObject);

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());
	}
	TEST_FIXTURE(Fixture, TestGameServerObjectManager_DeleteObject)
	{
		PGameServerObject* pGameServerObject = CreateGameServer();

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());

		m_pTestGameServerObjectManager->DeleteObject(pGameServerObject->GetUID());

		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetGameServerIDCount());
	}
	TEST_FIXTURE(Fixture, TestGameServerObjectManager_DeleteObject_InvalidUID)
	{
		CreateGameServer();

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());

		m_pTestGameServerObjectManager->DeleteObject(MUID::ZERO);

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());
	}
	TEST_FIXTURE(Fixture, TestGameServerObjectManager_Clear)
	{
		PGameServerObject* pGameServer = CreateGameServer();

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());

		m_pTestGameServerObjectManager->Clear();

		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetGameServerIDCount());

		pGameServer->Destroy();
		SAFE_DELETE(pGameServer);
	}
	TEST_FIXTURE(Fixture, TestGameServerObjectManager_DeleteAllObject)
	{
		CreateGameServer();

		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(1, m_pTestGameServerObjectManager->GetGameServerIDCount());

		m_pTestGameServerObjectManager->DeleteAllObject();

		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(0, m_pTestGameServerObjectManager->GetGameServerIDCount());
	}

	TEST_FIXTURE(Fixture, TestGameServerObjectManager_FindByUID)
	{
		CreateGameServer();
		CreateGameServer();
		PGameServerObject* pGameServer = CreateGameServer();
		MUID uidGameServer = pGameServer->GetUID();

		CHECK_EQUAL(3, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(3, m_pTestGameServerObjectManager->GetGameServerIDCount());

		PGameServerObject* pFindedGameServer = m_pTestGameServerObjectManager->Find(uidGameServer);

		CHECK_EQUAL(pGameServer,			pFindedGameServer);
		CHECK_EQUAL(pGameServer->GetUID(),	pFindedGameServer->GetUID());
	}
	TEST_FIXTURE(Fixture, TestGameServerObjectManager_FindByServerID)
	{
		CreateGameServer();
		CreateGameServer();
		PGameServerObject* pGameServer = CreateGameServer();
		int nServerID = pGameServer->GetServerID();

		CHECK_EQUAL(3, m_pTestGameServerObjectManager->GetClientsCount());
		CHECK_EQUAL(3, m_pTestGameServerObjectManager->GetGameServerIDCount());

		PGameServerObject* pFindedGameServer = m_pTestGameServerObjectManager->Find(nServerID);

		CHECK_EQUAL(pGameServer,				pFindedGameServer);
		CHECK_EQUAL(pGameServer->GetServerID(),	pFindedGameServer->GetServerID());
	}

}