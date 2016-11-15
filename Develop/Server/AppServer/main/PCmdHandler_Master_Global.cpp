#include "stdafx.h"
#include "PCmdHandler_Master_Global.h"
#include "MCommand.h"
#include "PCommandTable.h"
#include "STransData_M2G.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PMasterServerFacade.h"
#include "PConfig.h"

PCmdHandler_Master_Global::PCmdHandler_Master_Global(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_ONCONNECT,		OnConnect);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,	OnDisconnect);
	SetCmdHandler(MC_LOCAL_NET_ONERROR,			OnError);

	SetCmdHandler(MAC_COMM_RESPONSE_LOGIN,		OnResponseLogin);
}

MCommandResult PCmdHandler_Master_Global::OnResponseLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult = 0;
	MUID uidAlloc;
	uint8 nServerMode;

	if( !pCommand->GetParameter(&nResult,		0, MPT_INT) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidAlloc,		1, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nServerMode,	2, MPT_UCHAR) )	return CR_ERROR;

	if (nResult != CR_SUCCESS)
	{
		mlog("MasterServer Login Failed\n");
		
		gsys.pMasterServerFacade->OnDisconnect();

		return CR_ERROR;
	}

	///< MasterServer 의 서버 모드로 셋팅
	PConfig::m_nServerMode = nServerMode;

	mlog("MasterServer Login Success : Alloc UID = (%u:%u), ServerMode = %s\n", uidAlloc.High, uidAlloc.Low, PConfig::GetServerModeANSIString().c_str());

	return CR_TRUE;
}

MCommandResult PCmdHandler_Master_Global::OnConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pMasterServerFacade->OnConnect();	
	return CR_TRUE;
}

MCommandResult PCmdHandler_Master_Global::OnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pMasterServerFacade->OnDisconnect();
	return CR_TRUE;
}

MCommandResult PCmdHandler_Master_Global::OnError(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	int nErrorCode;
	if (!pCommand->GetParameter(&nErrorCode, 0, MPT_INT))		return CR_ERROR;

	// 커멘드 처리
	gsys.pMasterServerFacade->OnError(nErrorCode);

	return CR_TRUE;
}
