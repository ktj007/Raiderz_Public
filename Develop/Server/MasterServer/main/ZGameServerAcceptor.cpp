#include "stdafx.h"
#include "ZGameServerAcceptor.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZGlobal.h"
#include "ZCommandTable.h"
#include "ZServer.h"
#include "ZGameServerInfo.h"
#include "ZGameServerInfoManager.h"
#include "ZCommandCenter.h"
#include "ZFieldCommandRouter.h"
#include "ZSharedField.h"
#include "ZConfig.h"


bool ZGameServerAcceptor::IsAlreadyExist()
{
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_nServerID);
	if (pGameServerObject != NULL) return true;

	return false;
}

bool ZGameServerAcceptor::Accept()
{
	m_pGameServerInfo = gmgr.pGameServerInfoManager->GetInfo(m_nServerID);
	if (m_pGameServerInfo == NULL)
	{
		mlog("Error! Not Exist ServerInfo(ID: %d)\n", m_nServerID);
		return false;
	}

	// 서버 오브젝트 생성
	CreateGameServerObject(m_uidClient);

	// 정적 필드 인스턴스 생성하여 서버 오브젝트에 할당
	if (InitGameServerObject(m_uidClient) == false)
	{
		gmgr.pGameServerObjectManager->DeleteObject(m_uidClient);

		return false;
	}

	mlog("Game Server Logined : ServerID(%d)\n", m_nServerID);

	return true;
}

minet::MCommand* ZGameServerAcceptor::MakeResponseCommand()
{
	void* pBlobPtr = NULL;
	vector<TDMG_MANAGED_SHARED_FIELD> vecSharedFieldData;

	MakeParam_SharedFields(vecSharedFieldData);

	if (!vecSharedFieldData.empty()) pBlobPtr = &vecSharedFieldData[0];

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_RESPONSE_LOGIN_M2G
		, m_uidClient
		, 4
		, NEW_INT(CR_SUCCESS)
		, NEW_UID(m_uidClient)
		, NEW_UCHAR(ZConfig::m_nServerMode)
		, NEW_BLOB(pBlobPtr, sizeof(TDMG_MANAGED_SHARED_FIELD), (int)vecSharedFieldData.size()));

	return pNewCommand;
}

minet::MCommand* ZGameServerAcceptor::MakeFailCommand(int nCommandResult)
{
	MCommand* pFailCommand = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_RESPONSE_LOGIN_M2G, 
													m_uidClient, 
													4, 
													NEW_INT(nCommandResult), 
													NEW_UID(m_uidClient),
													NEW_UCHAR(ZConfig::m_nServerMode),
													NEW_BLOB(NULL, sizeof(TDMG_MANAGED_SHARED_FIELD), 0));
	return pFailCommand;
}

void ZGameServerAcceptor::MakeParam_SharedFields( vector<TDMG_MANAGED_SHARED_FIELD>& outvecParam )
{
	if (m_pGameServerInfo == NULL) return;

	vector<ZManagedSharedField>& vecAssignmentInfo = m_pGameServerInfo->m_vecManagedSharedField;
	if (vecAssignmentInfo.empty()) return;

	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_uidClient);
	if (pGameServerObject == NULL) return;

	outvecParam.resize(vecAssignmentInfo.size());

	for (size_t i = 0; i < vecAssignmentInfo.size(); i++)
	{
		outvecParam[i].nFieldID = vecAssignmentInfo[i].nFieldID;
		outvecParam[i].nChannelCount = (int)vecAssignmentInfo[i].vecChannelID.size();

		for (size_t channel_index = 0; channel_index < vecAssignmentInfo[i].vecChannelID.size(); channel_index++)
		{
			if (channel_index >= TDMG_MANAGED_SHARED_FIELD::MAX_SHARED_FIELD) 
			{
				_ASSERT(0);
				break;
			}

			// channel id
			outvecParam[i].nChannelID[channel_index] = vecAssignmentInfo[i].vecChannelID[channel_index];

			// field uid
			ZSharedField* pField = pGameServerObject->GetFieldMap().FindSharedField(outvecParam[i].nFieldID, outvecParam[i].nChannelID[channel_index]);
			if (pField)
			{
				_ASSERT(pField->GetUID().IsValid());
				outvecParam[i].uidField[channel_index] = pField->GetUID();
			}
			else
			{
				_ASSERT(0);
			}
		}
	}
}

bool ZGameServerAcceptor::InitGameServerObject( MUID uidClient )
{
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidClient);
	if (pGameServerObject == NULL) return false;

	pGameServerObject->Init(gmgr.pFieldManager);

	return true;
}

void ZGameServerAcceptor::CreateGameServerObject( MUID uidClient )
{
	ZGameServerObject* pClientObject = gmgr.pGameServerObjectManager->NewGameServerObject(uidClient, m_pGameServerInfo);
	if (pClientObject == NULL) return;

	gmgr.pGameServerObjectManager->AddObject(pClientObject);
}