#include "StdAfx.h"
#include "GUTHelper_CmdHandler.h"
#include "MMockNetClient.h"
#include "GCommandTable.h"
#include "STransData_M2G.h"


TD_GATE_INFO GUTHelper_CmdHandler::MakeParam_TD_GATE_INFO(int nFieldID, int nChannelID, MUID uidFieldGrouop, vec3 vPos, vec3 vDir)
{
	TD_GATE_INFO tdGateInfo;
	tdGateInfo.nFieldID = nFieldID;
	tdGateInfo.nChannelID = nChannelID;
	tdGateInfo.uidDynamicFieldGroup = uidFieldGrouop;
	if (uidFieldGrouop == MUID::ZERO)
		tdGateInfo.bDynamicField = false;
	else
		tdGateInfo.bDynamicField = true;
	tdGateInfo.vPosition = vPos;
	tdGateInfo.vDirection = vDir;
	return tdGateInfo;
}

MCommandResult GUTHelper_CmdHandler::OnRecv_MMC_GATE_RES(minet::MTestNetAgent* pNetAgent, MUID uidRequestPlayer, int nGateType, TD_GATE_INFO* pGateInfo)
{
	return pNetAgent->OnRecv(MMC_GATE_RES
		, 3
		, NEW_UID(uidRequestPlayer)
		, NEW_INT(nGateType)
		, NEW_BLOB(pGateInfo, sizeof(TD_GATE_INFO), 1));
}