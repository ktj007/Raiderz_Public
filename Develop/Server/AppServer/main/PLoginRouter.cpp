#include "stdafx.h"
#include "PLoginRouter.h"
#include "PCommandCenter.h"
#include "PGlobal.h"
#include "PCommandTable.h"

void PLoginRouter::RouteLoginResponse(MUID uidClient, CCommandResultTable_MasterServer nResult) const
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MPC_COMM_RESPONSE_LOGIN,
										uidClient,
										2,
										NEW_INT(nResult),
										NEW_UID(uidClient)
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}
