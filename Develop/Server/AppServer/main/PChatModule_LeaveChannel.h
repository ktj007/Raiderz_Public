#pragma once

#include "PChatModuleBase.h"
#include "STypes.h"

class PChatModule_LeaveChannel : public PChatModuleBase
{
public:
	PChatModule_LeaveChannel(PChatChannel* pOwnerChannel);
	virtual ~PChatModule_LeaveChannel(void);

	bool Run(MUID uidChannel, CID cidPlayer, MUID uidGameServer);

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};
