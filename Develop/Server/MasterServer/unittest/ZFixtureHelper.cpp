#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZTestHelper.h"
#include "MockLink.h"
#include "ZCommandTable.h"
#include "ZGameServerAcceptor.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerInfoManager.h"
#include "ZAppServerObject.h"
#include "MockFieldInfoManager.h"
#include "ZTestWrapper.h"
#include "ZSharedField.h"

//////////////////////////////////////////////////////////////////////////

ZSharedField* FBaseField::AddSharedField(int nFieldID, int nChannelID, int nGameServerID, bool bIsDefaultChannel)
{
	ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID, bIsDefaultChannel);
	ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
	if (NULL != pGameServer)
		pGameServer->GetFieldMap().AddSharedField(pField);
	
	return pField;
}

ZSharedField* FBaseField::AddSharedField(ZFieldInfo* pFieldInfo, int nChannelID, int nGameServerID, bool bIsDefaultChannel)
{
	return AddSharedField(pFieldInfo->m_nFieldID, nChannelID, nGameServerID, bIsDefaultChannel);
}

ZDynamicFieldMaster* FBaseField::AddFieldGroup(int nFieldGroupID, MUID uidOwner, int nGameServerID, DYNAMIC_FIELD_TYPE eType)
{
	ZDynamicFieldMaster* pFieldMaster = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, eType, uidOwner);
	ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
	if (NULL != pGameServer)
		pGameServer->GetFieldMap().AddDynamicFieldMaster(pFieldMaster);
	return pFieldMaster;
}

//////////////////////////////////////////////////////////////////////////

ZFieldInfo* FBaseFieldInfo::AddFieldInfo( int nFieldID, const wchar_t* szFieldName, const wchar_t* szFieldFile, bool bDynamic, int nDefaultChannelCount )
{
	MockFieldInfoManager* pFieldInfoManager = dynamic_cast<MockFieldInfoManager*>(gmgr.pFieldInfoManager);
	if (pFieldInfoManager == NULL) return NULL;

	pFieldInfoManager->AddFieldInfo(nFieldID, szFieldName, szFieldFile, bDynamic, nDefaultChannelCount);
	return gmgr.pFieldInfoManager->Find(nFieldID);
}

void FBaseFieldInfo::AddFieldGroupInfo(int nFieldGroupID, int nFirstDynamicFieldID)
{
	MockFieldInfoManager* pFieldInfoManager = dynamic_cast<MockFieldInfoManager*>(gmgr.pFieldInfoManager);
	if (pFieldInfoManager == NULL) return;

	pFieldInfoManager->AddFieldGroupInfo(nFieldGroupID, nFirstDynamicFieldID);
}

//////////////////////////////////////////////////////////////////////////

ZPlayer* FBasePlayer::AddNewPlayer(MUID uidPlayer/*=MUID::ZERO*/, AID nAID/*=0*/, std::wstring strUserID/*=L""*/)
{
	if (0 == uidPlayer)			uidPlayer = ZTestHelper::NewUID();
	if (0 == nAID)				nAID = ZTestHelper::NewAID();
	if (strUserID.empty())		strUserID = ZTestHelper::NewName();

	ZPlayer* pNewPlayer = gmgr.pPlayerManager->NewPlayer(uidPlayer, nAID, strUserID);
	gmgr.pPlayerManager->Add(pNewPlayer);

	return pNewPlayer;
}

ZPlayer* FBasePlayer::AddNewPlayerInWorld(int nGameServerID, AID nAID, std::wstring strUserID, CID nCID, MUID uidPlayer, std::wstring strPlayerName, int nGID)
{
	if (0 == nGameServerID)		nGameServerID = ZTestHelper::NewID();
	if (0 == nAID)				nAID = ZTestHelper::NewAID();
	if (strUserID.empty())		strUserID = ZTestHelper::NewName();
	if (0 == nCID)				nCID = ZTestHelper::NewCID();
	if (0 == uidPlayer)			uidPlayer = ZTestHelper::NewUID();
	if (strPlayerName.empty())	strPlayerName = ZTestHelper::NewName();
	if (0 == nGID)				nGID = ZTestHelper::NewID();

	ZPlayer* pNewPlayer = gmgr.pPlayerManager->NewPlayer(uidPlayer, nAID, strUserID);
	gmgr.pPlayerManager->Add(pNewPlayer);
	
	pNewPlayer->SetWorldInfo(nCID, nGameServerID, uidPlayer, strPlayerName, nGID);
	gmgr.pPlayerManager->UpdateCIDUID(pNewPlayer, nCID, uidPlayer);

	pNewPlayer->SetState(PS_IN_WORLD);


	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
	if (pGameServerObject != NULL)
	{
		pGameServerObject->GetPlayerMap().Add(pNewPlayer);
	}

	return pNewPlayer;
}

//////////////////////////////////////////////////////////////////////////

ZGameServerInfo* FBaseGameServer::AddGameServerInfo( int nServerID, int nFieldID, int nChannelIDCount, int nFirstChannelID, ... )
{
	ZGameServerInfoManager* pMgr = gmgr.pGameServerInfoManager;
	ZGameServerInfo* pInfo = pMgr->GetInfo(nServerID);
	if (pInfo == NULL)
	{
		pInfo = new ZGameServerInfo();
		pInfo->m_nID = nServerID;

		pMgr->Insert(pInfo);
	}

	ZManagedSharedField managedSharedField;
	managedSharedField.nFieldID = nFieldID;


	vector<int> vecChannelIDs;

	if (nChannelIDCount > 0)
	{
		va_list marker;

		int channel_id = nFirstChannelID;

		va_start( marker, nFirstChannelID );     /* Initialize variable arguments. */
		for (int i = 0; i < nChannelIDCount; i++)
		{
			vecChannelIDs.push_back(channel_id);

			channel_id = va_arg( marker, int);
		}
		va_end( marker );              /* Reset variable arguments.      */
	}

	managedSharedField.vecChannelID = vecChannelIDs;

	pInfo->m_vecManagedSharedField.push_back(managedSharedField);

	return pInfo;
}

ZGameServerObject* FBaseGameServer::AddGameServerObject( ZGameServerInfo* pGameServerInfo )
{
	MUID uidNew = ZTestHelper::NewUID();
	ZGameServerObject* pGameServerObject = new ZGameServerObject(uidNew, pGameServerInfo);
	pGameServerObject->Init(gmgr.pFieldManager);

	gmgr.pGameServerObjectManager->AddObject(pGameServerObject);

	return pGameServerObject;
}

ZGameServerObject* FBaseGameServer::AddGameServerObject( int nServerID, int nFieldID )
{
	if (0 == nServerID)
	{
		nServerID = ZTestHelper::NewID();
	}

	if (0 == nFieldID)
	{
		nFieldID = ZTestHelper::NewID();
	}		

	AddGameServerInfo(nServerID, nFieldID, 1, 1);

	ZGameServerInfo* pGameServerInfo = gmgr.pGameServerInfoManager->GetInfo(nServerID);
	if (pGameServerInfo == NULL)
	{
		pGameServerInfo = AddGameServerInfo(nServerID, nFieldID, 1, 1);
	}

	return AddGameServerObject(pGameServerInfo);
}
void FBaseServerField::InitDefaultGameServer(int nServerID, int nFieldID, int nChannelCount)
{
	m_nServerID = nServerID;
	m_nFieldID = nFieldID;

	AddFieldInfo(m_nFieldID, L"", L"", false, 1);

	if (nChannelCount == 1)
	{
		AddGameServerInfo(m_nServerID, m_nFieldID, 1, 1);
	}
	else if (nChannelCount == 2)
	{
		AddGameServerInfo(m_nServerID, m_nFieldID, 2, 1, 2);
	}
	else if (nChannelCount == 3)
	{
		AddGameServerInfo(m_nServerID, m_nFieldID, 3, 1, 2, 3);
	}
	else if (nChannelCount == 4)
	{
		AddGameServerInfo(m_nServerID, m_nFieldID, 4, 1, 2, 3, 4);
	}
	else
	{
		AddGameServerInfo(m_nServerID, m_nFieldID, 1, 1);
	}

	ZGameServerInfo* pGameServerInfo = gmgr.pGameServerInfoManager->GetInfo(m_nServerID);
	ZGameServerObject* pGameServerObject = AddGameServerObject(pGameServerInfo);
	if (NULL != pGameServerObject)
	{
		m_uidClient = pGameServerObject->GetUID();
	}
}

//////////////////////////////////////////////////////////////////////////

ZLoginServerInfo* FBaseLoginServer::AddLoginServerInfo( int nServerID/*=0*/ )
{
	if (nServerID == 0) nServerID = ZTestHelper::NewID();

	ZLoginServerInfoManager* pMgr = gmgr.pLoginServerInfoManager;
	ZLoginServerInfo* pInfo = pMgr->GetInfo(nServerID);
	if (pInfo == NULL)
	{
		pInfo = new ZLoginServerInfo();
		pInfo->m_nID = nServerID;

		pMgr->Insert(pInfo);
	}

	return pInfo;
}

MUID FBaseLoginServer::Connected( ZLoginServerInfo* pLoginServerInfo )
{
	MUID uidNew = ZTestHelper::NewUID();
	gmgr.pLoginServerFacade->Connected(uidNew, pLoginServerInfo);

	return uidNew;
}

MUID FBaseLoginServer::Connected( int nServerID/*=0*/ )
{
	AddLoginServerInfo(nServerID);

	ZLoginServerInfo* pLoginServerInfo = gmgr.pLoginServerInfoManager->GetInfo(nServerID);
	if (pLoginServerInfo == NULL)
	{
		pLoginServerInfo = AddLoginServerInfo(nServerID);
	}

	return Connected(pLoginServerInfo);
}


//////////////////////////////////////////////////////////////////////////

ZAppServerInfo* FBaseAppServer::AddAppServerInfo( int nServerID )
{
	ZAppServerInfoManager* pMgr = gmgr.pAppServerInfoManager;
	ZAppServerInfo* pInfo = pMgr->GetInfo(nServerID);
	if (pInfo == NULL)
	{
		pInfo = new ZAppServerInfo();
		pInfo->m_nID = nServerID;

		pMgr->Insert(pInfo);
	}

	return pInfo;
}

MUID FBaseAppServer::AddAppServerObject( ZAppServerInfo* pAppServerInfo )
{
	MUID uidNew = ZTestHelper::NewUID();
	ZAppServerObject* pAppServerObject = new ZAppServerObject(uidNew, pAppServerInfo);

	gmgr.pAppServerObjectManager->AddObject(pAppServerObject);

	return uidNew;
}

MUID FBaseAppServer::AddAppServerObject( int nServerID )
{
	AddAppServerInfo(nServerID);

	ZAppServerInfo* pAppServerInfo = gmgr.pAppServerInfoManager->GetInfo(nServerID);
	if (pAppServerInfo == NULL)
	{
		pAppServerInfo = AddAppServerInfo(nServerID);
	}

	return AddAppServerObject(pAppServerInfo);
}