#pragma once

#include "PChatModule_EnterChannel.h"


class PChatModule_EnterPrivateChannel : public PChatModule_EnterChannel
{
public:
	PChatModule_EnterPrivateChannel(PChatChannel* pOwnerChannel) : PChatModule_EnterChannel(pOwnerChannel) {}
	virtual ~PChatModule_EnterPrivateChannel() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};