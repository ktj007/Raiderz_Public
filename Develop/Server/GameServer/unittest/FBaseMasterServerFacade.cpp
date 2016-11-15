#include "stdafx.h"
#include "FBaseMasterServerFacade.h"
#include "MockMasterNetClient.h"
#include "GConfig.h"
#include "GMasterServerPartyRouter.h"
#include "GServerGateRouter.h"
#include "GServerFieldRouter.h"
#include "GMasterServerPlayerRouter.h"
#include "GStandAloneModePlayerRouter.h"
#include "GMasterServerGuildRouter.h"
#include "GMasterServerMoveServerRouter.h"

FBaseMasterServerFacade::FBaseMasterServerFacade()
{
	GConfig::m_nMyServerID = 1;

	m_bOldStandAlone = GConfig::m_bStandAlone;
	GConfig::m_bStandAlone = false;

	minet::MNetClientDesc master_net_client_desc;
	master_net_client_desc.bKeepAlive = false;

	MockMasterNetClient* pMasterMockNetClient = new MockMasterNetClient(master_net_client_desc);

	gsys.pMasterServerFacade->SetNetClient(pMasterMockNetClient);

	m_pMasterServerFacade = gsys.pMasterServerFacade;
	
	// StandAloneMode가 구분되어 있는 Router
	m_pMasterServerFacade->SetPartyRouter(new GMasterServerPartyRouter());
	m_pMasterServerFacade->SetPlayerRouter(new GMasterServerPlayerRouter());
	m_pMasterServerFacade->SetGuildRouter(new GMasterServerGuildRouter());
	m_pMasterServerFacade->SetMoveServerRouter(new GMasterServerMoveServerRouter());
	
	// StandAloneMode 구분없는 Router
	m_pMasterServerFacade->SetGateRouter(new GServerGateRouter());	
	m_pMasterServerFacade->SetFieldRouter(new GServerFieldRouter());
}

FBaseMasterServerFacade::~FBaseMasterServerFacade()
{
	GConfig::m_bStandAlone = m_bOldStandAlone;
}
