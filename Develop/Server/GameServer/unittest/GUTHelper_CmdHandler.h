#pragma once

namespace minet
{
class MTestNetAgent;
}
struct TD_GATE_INFO;

class GUTHelper_CmdHandler
{
public:
	static TD_GATE_INFO MakeParam_TD_GATE_INFO(int nFieldID, int nChannelID, MUID uidFieldGrouop=MUID::ZERO, vec3 vPos=vec3(1000, 1000, 0), vec3 vDir=vec3(0, 0, 1));
	static MCommandResult OnRecv_MMC_GATE_RES(minet::MTestNetAgent* pNetAgent, MUID uidRequestPlayer, int nGateType, TD_GATE_INFO* pGateInfo);
};
