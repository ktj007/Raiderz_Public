#pragma once

class PChatChannel;

class PChatModuleBase
{
public:
	PChatModuleBase(PChatChannel* pOwnerChannel) : m_pOwnerChannel(pOwnerChannel)
	{
		assert(NULL != m_pOwnerChannel);
	}
	virtual ~PChatModuleBase()	{}

	void			Run(minet::MCommand* pCmd)	{ OnRun(pCmd); }

protected:
	PChatChannel*	m_pOwnerChannel;

	virtual void	OnRun(minet::MCommand* pCmd) = 0;

};
