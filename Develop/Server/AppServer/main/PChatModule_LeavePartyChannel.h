#pragma once

#include "PChatModule_LeaveChannel.h"


class PChatModule_LeavePartyChannel : public PChatModule_LeaveChannel
{
public:
	PChatModule_LeavePartyChannel(PChatChannel* pOwnerChannel);
	virtual ~PChatModule_LeavePartyChannel(void);

	void Run(MUID uidChannel, CID cidPlayer, MUID uidGameServer);

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};
