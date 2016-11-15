#pragma once

#include "PChatModuleBase.h"
#include "STypes.h"

class PChatModule_EnterChannel : public PChatModuleBase
{
public:
	PChatModule_EnterChannel(PChatChannel* pOwnerChannel);
	virtual ~PChatModule_EnterChannel(void);

	bool Run(MUID uidChannel, CID cidPlayer, MUID uidGameServer);

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;
};
