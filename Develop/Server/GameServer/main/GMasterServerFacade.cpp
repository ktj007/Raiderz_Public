#include "stdafx.h"
#include "GMasterServerFacade.h"
#include "GManagedFieldInfoMgr.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GFieldFactory.h"
#include "GGlobal.h"
#include "GManagedFieldInfoMgr.h"
#include "GMasterNetClient.h"
#include "GClientConnectionKeeper.h"
#include "GCommandTable.h"
#include "GEntityPlayer.h"
#include "GConfig.h"
#include "GProxyField.h"
#include "GProxyFieldManager.h"
#include "GProxyParty.h"
#include "MiNetHelper.h"
#include "GSystem.h"
#include "GPlayerGameData.h"
#include "GMasterServerPartyRouter.h"
#include "GStandAloneModePartyRouter.h"
#include "GServerGateRouter.h"
#include "GMasterServerPlayerRouter.h"
#include "GStandAloneModePlayerRouter.h"
#include "GServerFieldRouter.h"
#include "SProxyPlayerManager.h"
#include "SNetworkCardMgr.h"
#include "GSharedField.h"
#include "GStandAloneModeGateRouter.h"
#include "GStandAloneModeGuildRouter.h"
#include "GMasterServerGuildRouter.h"
#include "MLocale.h"
#include "GServer.h"
#include "GStandAloneModeMoveServerRouter.h"
#include "GMasterServerMoveServerRouter.h"


GMasterServerFacade::GMasterServerFacade()
: m_pMasterNetClient(NULL)
, m_pConnectionKeeper(NULL)
, m_pManagedFieldInfoMgr(new GManagedFieldInfoMgr())
, m_pProxyFieldManager(new GProxyFieldManager())
, m_pProxyPartyManager(new GProxyPartyManager())
, m_pProxyPlayerManager(NULL)
, m_pPartyRouter(NULL)
, m_pGateRouter(NULL)
, m_pPlayerRouter(NULL)
, m_pFieldRouter(NULL)
, m_pGuildRouter(NULL)
, m_pMoveServerRouter(NULL)
{
	m_nGameServerID = GConfig::m_nMyServerID;
}

GMasterServerFacade::GMasterServerFacade( MNetClient* pNetClient )
: m_pMasterNetClient(pNetClient)
, m_pConnectionKeeper(NULL)
, m_pManagedFieldInfoMgr(new GManagedFieldInfoMgr())
, m_pProxyFieldManager(new GProxyFieldManager())
, m_pProxyPartyManager(new GProxyPartyManager())
, m_pProxyPlayerManager(NULL)
, m_pPartyRouter(NULL)
, m_pGateRouter(NULL)
, m_pPlayerRouter(NULL)
, m_pFieldRouter(NULL)
, m_pGuildRouter(NULL)
, m_pMoveServerRouter(NULL)
{
	m_nGameServerID = GConfig::m_nMyServerID;
}

GMasterServerFacade::~GMasterServerFacade()
{
	DestroyNetwork();

	SAFE_DELETE(m_pProxyPartyManager);	
	SAFE_DELETE(m_pProxyFieldManager);
	SAFE_DELETE(m_pManagedFieldInfoMgr);	
}

void GMasterServerFacade::Init()
{
	if (IsStandAloneMode())
	{
		InitForStandAlone();
	}
}

void GMasterServerFacade::InitForStandAlone()
{
	m_pManagedFieldInfoMgr->AssignAllForStandAlone(gmgr.pFieldInfoMgr);

	// standalone이면 공용 필드 모두 미리 생성한다.
	gmgr.pFieldMgr->GetFactory()->CreateAllSharedField(gmgr.pFieldInfoMgr, gsys.pTestSystem, gsys.pServer);
}

bool GMasterServerFacade::CreateManagedSharedFields( TDMG_MANAGED_SHARED_FIELD* pFIelds, int nFieldCount )
{
	m_pManagedFieldInfoMgr->AssignSharedField(pFIelds, nFieldCount);

	if (false == gmgr.pFieldMgr->GetFactory()->CreateAllSharedField(gmgr.pFieldInfoMgr, gsys.pTestSystem, gsys.pServer))
		return false;

	return true;
}

void GMasterServerFacade::AddManagedSharedFields(TDMG_SHARED_FIELD_INFO* pTDFieldInfo, int nFieldCount)
{
	_ASSERT(pTDFieldInfo != NULL);
	_ASSERT(0 <= nFieldCount);

	for(int i = 0; i < nFieldCount; i++)
	{
		// 서버 ID가 자신이면 추가.
		if (pTDFieldInfo[i].nGameServerID != GConfig::m_nMyServerID)
			continue;		

		// 필드를 생성하고, ManagedFieldMgr에도 추가한다.
		MUID uidField = pTDFieldInfo[i].uidField;
		int nFieldID = pTDFieldInfo[i].nFieldID;
		int nChannelID = pTDFieldInfo[i].nChannelID;
		
		m_pManagedFieldInfoMgr->AddChannel(uidField, nFieldID, nChannelID);
		gmgr.pFieldMgr->AddSharedField(uidField, nFieldID, nChannelID);
	}
}

void GMasterServerFacade::DelManagedSharedField(MUID uidField)
{
	_ASSERT(uidField.IsValid());

	GProxyField* pField = m_pProxyFieldManager->Find(uidField);
	if (NULL == pField)
	{
		dlog("Error! GMasterServerFacade::DelManagedSharedField(), Not Found!\n");
		return;
	}

	if (pField->GetGameServerID() != GConfig::m_nMyServerID)
		return;

	m_pManagedFieldInfoMgr->DelChannel(uidField);

	GSharedField* pSharedField = gmgr.pFieldMgr->GetSharedField(pField->GetFieldInfo()->m_nFieldID, pField->GetChannelID());
	if (NULL == pSharedField)
	{
		dlog("Error! GMasterServerFacade::DelManagedSharedField(), (FieldID=%d, ChannelID=%d)\n", pField->GetFieldInfo()->m_nFieldID, pField->GetChannelID());
		return;
	}

	pSharedField->DeleteMe();
}

bool GMasterServerFacade::IsStandAloneMode()
{	
	return GConfig::IsStandAloneMode();
}

void GMasterServerFacade::RunNetwork()
{
	if (m_pMasterNetClient) m_pMasterNetClient->Run();
	if (m_pConnectionKeeper) m_pConnectionKeeper->Run();
}

bool GMasterServerFacade::CreateNetwork(const wchar_t* szMasterServerIP, int nMasterServerPort)
{
	if (m_pMasterNetClient == NULL)
	{
		minet::MNetClientDesc master_net_client_desc;	// heartbeat 체크를 하지 않습니다.

		m_pMasterNetClient = new GMasterNetClient(master_net_client_desc);
		m_pProxyPlayerManager = new SProxyPlayerManager(m_pMasterNetClient);
	}

	/// 네트워크 모듈 생성
	if (IsStandAloneMode() == false)
	{
		wstring strNetworkCardIP = GetNetworkCardMgr()->GetIP(GConfig::m_nPrivateNetworkCardID);
		m_pConnectionKeeper = new GClientConnectionKeeper(m_pMasterNetClient, szMasterServerIP, nMasterServerPort, gsys.pSystem, 10000, strNetworkCardIP.c_str());
		m_pConnectionKeeper->SetConnectionType(GClientConnectionKeeper::MASTER_SERVER);

		if (ConnectToMasterServer(szMasterServerIP, nMasterServerPort, strNetworkCardIP.c_str()) == false)
		{
			return false;
		}
	}


	// Router 생성	
	if (IsStandAloneMode())
	{
		SetPartyRouter(new GStandAloneModePartyRouter());
		SetPlayerRouter(new GStandAloneModePlayerRouter());
		SetGateRouter(new GStandAloneModeGateRouter());	
		SetGuildRouter(new GStandAloneModeGuildRouter());
		SetMoveServerRouter(new GStandAloneModeMoveServerRouter());
	}
	else
	{
		SetPartyRouter(new GMasterServerPartyRouter());
		SetPlayerRouter(new GMasterServerPlayerRouter());
		SetGateRouter(new GServerGateRouter());	
		SetGuildRouter(new GMasterServerGuildRouter());
		SetMoveServerRouter(new GMasterServerMoveServerRouter());
	}
	SetFieldRouter(new GServerFieldRouter());
	
	return true;
}

void GMasterServerFacade::DestroyNetwork()
{
	SAFE_DELETE(m_pPartyRouter);
	SAFE_DELETE(m_pGateRouter);
	SAFE_DELETE(m_pPlayerRouter);
	SAFE_DELETE(m_pFieldRouter);
	SAFE_DELETE(m_pGuildRouter);
	SAFE_DELETE(m_pMoveServerRouter);
	SAFE_DELETE(m_pProxyPlayerManager);

	if (m_pMasterNetClient == NULL) return;

	if (IsStandAloneMode() == false)
	{	
		if (m_pMasterNetClient->IsConnected())
		{
			m_pMasterNetClient->Disconnect();
		}
	}

	SAFE_DELETE(m_pConnectionKeeper);
	SAFE_DELETE(m_pMasterNetClient);
}

bool GMasterServerFacade::ConnectToMasterServer( const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP )
{
	if (m_pMasterNetClient == NULL) return false;

	MCommand* pNewCommand = m_pMasterNetClient->NewLocalCommand(MC_LOCAL_NET_CONNECT_WITH_NIC);
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(nPort));
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szNetworkCardIP).c_str()));
	return m_pMasterNetClient->Post(pNewCommand);
}

void GMasterServerFacade::AddProxySharedFields( TDMG_SHARED_FIELD_INFO* pFieldInfos, int nFieldInfoCount, uint32 nChecksum )
{
	for (int i = 0; i < nFieldInfoCount; i++)
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(pFieldInfos[i].nFieldID);
		if (pFieldInfo == NULL) continue;

		m_pProxyFieldManager->AddSharedField(pFieldInfos[i].uidField, pFieldInfo, pFieldInfos[i].nGameServerID, pFieldInfos[i].nChannelID);
	}

	if (nChecksum != m_pProxyFieldManager->CalcChecksumSharedFields())
	{
		m_pFieldRouter->RequestAllFieldInfo();
	}
}

void GMasterServerFacade::SetNetClient( minet::MNetClient* pNetClient )
{
	if (m_pMasterNetClient == NULL && pNetClient != NULL)
	{
		m_pProxyPlayerManager = new SProxyPlayerManager(pNetClient);
	}

	if (m_pMasterNetClient)
	{
		delete m_pMasterNetClient;
	}

	m_pMasterNetClient = pNetClient;
}

void GMasterServerFacade::DelProxySharedField( MUID uidField, uint32 nChecksum )
{
	m_pProxyFieldManager->DeleteSharedField(uidField);

	if (nChecksum != m_pProxyFieldManager->CalcChecksumSharedFields())
	{
		m_pFieldRouter->RequestAllFieldInfo();
	}
}

void GMasterServerFacade::ClearProxySharedFields()
{
	m_pProxyFieldManager->ClearProxySharedFields();
}

void GMasterServerFacade::AddProxyFieldGroups(int nFieldGroupCount, TDMG_DYNAMIC_FIELD_GROUP_INFO* pTDFieldGroupList, int nFieldCount, TDMG_DYNAMIC_FIELD_NODE* pTDDynamicFieldList, uint32 nChecksum)
{
	TDMG_DYNAMIC_FIELD_GROUP_INFO* pTDFieldGroupInfo = NULL;
	for (int i = 0; i < nFieldGroupCount; ++i)
	{
		pTDFieldGroupInfo = &pTDFieldGroupList[i];

		FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(pTDFieldGroupInfo->nFieldGroupID);
		if (NULL == pFieldGroupInfo) return;

		m_pProxyFieldManager->AddFieldGroup(pTDFieldGroupInfo->uidDynamicFieldGroup, pFieldGroupInfo, pTDFieldGroupInfo->nGameServerID, (DYNAMIC_FIELD_TYPE)pTDFieldGroupInfo->nType);
	}

	TDMG_DYNAMIC_FIELD_NODE* pTDDynamicFieldInfo = NULL;
	for (int i = 0; i < nFieldCount; ++i)
	{
		pTDDynamicFieldInfo = &pTDDynamicFieldList[i];

		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(pTDDynamicFieldInfo->nFieldID);
		if (NULL == pFieldInfo) return;

		if (!m_pProxyFieldManager->AddDynamicField(pTDDynamicFieldInfo->uidDynamicField, pFieldInfo, pTDDynamicFieldInfo->uidFieldGroup))
			continue;
	}

	if (nChecksum != m_pProxyFieldManager->CalcChecksumFieldGroups())
	{
		m_pFieldRouter->RequestAllFieldGroupInfo();
	}
}

void GMasterServerFacade::DelProxyFieldGroup(MUID uidFieldGroup, uint32 nChecksum)
{
	m_pProxyFieldManager->DeleteFieldGroup(uidFieldGroup);

	if (nChecksum != m_pProxyFieldManager->CalcChecksumFieldGroups())
	{
		m_pFieldRouter->RequestAllFieldGroupInfo();
	}
}

void GMasterServerFacade::ClearProxyFieldGroups()
{
	m_pProxyFieldManager->ClearProxyFieldGroups();
}

void GMasterServerFacade::UpdateProxyFieldInfo(MUID uidField, const TDMG_PROXY_FIELD_UPDATE_INFO* pTDProxyFieldUpdateInfo)
{
	VALID(NULL != pTDProxyFieldUpdateInfo);

	GProxyField* pProxyField = m_pProxyFieldManager->Find(uidField);
	if (NULL == pProxyField) return;

	pProxyField->SetPlayerCount(pTDProxyFieldUpdateInfo->nPlayerCount);
}

void GMasterServerFacade::AddProxyParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
{
	m_pProxyPartyManager->AddParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
}

void GMasterServerFacade::RemoveProxyParty(MUID uidParty)
{
	m_pProxyPartyManager->RemoveParty(uidParty);
}

void GMasterServerFacade::AddProxyPartyMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID)
{
	m_pProxyPartyManager->AddMember(uidParty, uidMember, strMemberName, nMemberCID);
}

void GMasterServerFacade::RemoveProxyPartyMember(MUID uidParty, MUID uidMember)
{
	m_pProxyPartyManager->RemoveMember(uidParty, uidMember);
}

void GMasterServerFacade::ChangeProxyPartyLeader(MUID uidParty, MUID uidNewLeader)
{
	m_pProxyPartyManager->ChangeLeader(uidParty, uidNewLeader);
}

void GMasterServerFacade::ClearProxyParty(void)
{
	m_pProxyPartyManager->Clear();
}

GProxyParty* GMasterServerFacade::FindProxyParty(MUID uidParty) const
{
	return m_pProxyPartyManager->Find(uidParty);
}

void GMasterServerFacade::RestoreProxyParty(TD_PARTY_INFO* pPartyInfo, int nCount)
{
	// 기존 정보 제거
	m_pProxyPartyManager->Clear();

	for(int idxMember = 0; idxMember < nCount; idxMember++)
	{
		// 파티장 정보로 파티 추가
		TD_PARTY_INFO& rPartyInfo = pPartyInfo[idxMember];

		MUID uidParty = rPartyInfo.uidParty;
		int nLeaderIndex = rPartyInfo.nLeaderIndex;
		MUID uidLeader = rPartyInfo.members[nLeaderIndex].uidMember;
		wstring strLeaderName = rPartyInfo.members[nLeaderIndex].szMemberName;
		int nLeaderCID = rPartyInfo.members[nLeaderIndex].nMemberCID;
		
		m_pProxyPartyManager->AddParty(uidParty, uidLeader, strLeaderName, nLeaderCID);		


		// 나머지 파티원 추가
		int nMemberCount = rPartyInfo.nMemberCount;
		for(int i = 0; i < nMemberCount; i++)
		{
			if (i == nLeaderIndex)	continue;

			MUID uidMember = rPartyInfo.members[i].uidMember;
			wstring strMemberName = rPartyInfo.members[i].szMemberName;
			int nMemberCID = rPartyInfo.members[i].nMemberCID;
			m_pProxyPartyManager->AddMember(uidParty, uidMember, strMemberName, nMemberCID);
		}
	}
}

void GMasterServerFacade::RouteRequestAppServerList(void)
{
	if (m_pMasterNetClient == NULL)	return;

	MCommand* pCmd = MakeNewCommand(MMC_COMM_REQUEST_APP_SERVER_LIST);
	Route(pCmd);
}

void GMasterServerFacade::OnConnect()
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->OnConnect();
	}

	mlog("Connected To MasterServer\n");
}

void GMasterServerFacade::OnDisconnect()
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->OnDisconnect();
	}

	gsys.pServer->CloseServer();
	
	mlog("Disconnected From MasterServer\n");
}

void GMasterServerFacade::OnError( int nErrorCode )
{
	if (nErrorCode == WSAECONNREFUSED)
	{
		// 연결 실패인 경우
		if (m_pConnectionKeeper)
		{
			m_pConnectionKeeper->OnNetError(nErrorCode);
		}
	}
	else
	{
		// 네트워크 오류
		gsys.pServer->CloseServer();
		mlog("Client NetError (ErrorCode=%d)\n", nErrorCode);		
	}
}

MCommand* GMasterServerFacade::MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand* pNewCommand = m_pMasterNetClient->NewCommand(nCmdID, 0);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}

MCommand* GMasterServerFacade::MakeNewCommand(int nCmdID)
{
	return m_pMasterNetClient->NewCommand(nCmdID, 0);
}

bool GMasterServerFacade::Route(MCommand* pCommand)
{		
	if (pCommand == NULL)				return false;
	if (m_pMasterNetClient == NULL)		
	{
		SAFE_DELETE(pCommand);
		return false;
	}

	MUID uidReceiver = m_pMasterNetClient->GetServerUID();
	pCommand->SetReceiverUID(uidReceiver);

	return m_pMasterNetClient->Post(pCommand);
}

void GMasterServerFacade::SetPartyRouter(GServerPartyRouter* pPartyRouter)
{
	_ASSERT(pPartyRouter != NULL);

	SAFE_DELETE(m_pPartyRouter);
	m_pPartyRouter = pPartyRouter;
}

void GMasterServerFacade::SetGateRouter(GServerGateRouter* pGateRouter)
{
	_ASSERT(pGateRouter != NULL);

	SAFE_DELETE(m_pGateRouter);
	m_pGateRouter = pGateRouter;
}

void GMasterServerFacade::SetPlayerRouter(GServerPlayerRouter* pPlayerRouter)
{
	_ASSERT(pPlayerRouter != NULL);
	
	SAFE_DELETE(m_pPlayerRouter);
	m_pPlayerRouter = pPlayerRouter;
}

void GMasterServerFacade::SetFieldRouter(GServerFieldRouter* pFieldRouter)
{
	_ASSERT(pFieldRouter != NULL);

	SAFE_DELETE(m_pFieldRouter);
	m_pFieldRouter = pFieldRouter;
}

void GMasterServerFacade::SetGuildRouter( GServerGuildRouter* pGuildRouter )
{
	_ASSERT(pGuildRouter != NULL);

	SAFE_DELETE(m_pGuildRouter);
	m_pGuildRouter = pGuildRouter;
}

void GMasterServerFacade::SetMoveServerRouter(GMoveServerRouter* pMoveServerRouter)
{
	_ASSERT(pMoveServerRouter != NULL);

	SAFE_DELETE(m_pMoveServerRouter);
	m_pMoveServerRouter = pMoveServerRouter;
}