#include "stdafx.h"
#include "GCmdHandler_App_Common.h"
#include "MCommandTable.h"
#include "CCommandTable_App.h"
#include "GGlobal.h"
#include "GAppServerFacade.h"
#include "CCommandResultTable.h"

GCmdHandler_App_Common::GCmdHandler_App_Common(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_ONCONNECT,				OnConnect);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,			OnDisconnect);
	SetCmdHandler(MC_LOCAL_NET_ONERROR,					OnError);
	
	SetCmdHandler(MPC_COMM_RESPONSE_LOGIN,				OnResponseLogin);	
}

MCommandResult GCmdHandler_App_Common::OnConnect(MCommand* pCommand, MCommandHandler* pHandler)	
{
	gsys.pAppServerFacade->OnConnect();
	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Common::OnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)	
{
	gsys.pAppServerFacade->OnDisconnect();
	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Common::OnError(MCommand* pCommand, MCommandHandler* pHandler)	
{
	// 커맨드 확인
	int nErrorCode;
	if (!pCommand->GetParameter(&nErrorCode, 0, MPT_INT))	return CR_ERROR;

	// 에러 통보
	gsys.pAppServerFacade->OnError(nErrorCode);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Common::OnResponseLogin(MCommand* pCommand, MCommandHandler* pHandler)	
{
	// 커맨드 확인
	int nResult;
	MUID uidClient;

	if (!pCommand->GetParameter(&nResult, 0, MPT_INT))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidClient, 1, MPT_UID))	return CR_ERROR;


	// 결과 확인
	if (nResult != CR_SUCCESS)
	{
		dlog("AppServer Login Failed(CommandResult = %d)\n", nResult);

		gsys.pAppServerFacade->OnDisconnect();
		return CR_ERROR;
	}

	dlog("AppServer Login Success : Alloc UID = (%u:%u)\n", uidClient.High, uidClient.Low);

	return CR_TRUE;
}
