#pragma once

#include "PChatModule_LeaveChannel.h"


class PChatModule_LeavePrivateChannel : public PChatModule_LeaveChannel
{
public:
	PChatModule_LeavePrivateChannel(PChatChannel* pOwnerChannel) : PChatModule_LeaveChannel(pOwnerChannel) {}
	virtual ~PChatModule_LeavePrivateChannel() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};