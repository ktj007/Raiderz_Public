#pragma once

#define DECL_WL_CMD_HANDLER(_func)			static bool _func(DWORD dwIP, WORD wRawPort, minet::MCommand* pCommand, minet::MUDPNetServer* pHandler);
#define IMPL_WL_CMD_HANDLER(_class, _func)	bool _class::_func(DWORD dwIP, WORD wRawPort, minet::MCommand* pCommand, minet::MUDPNetServer* pHandler)


namespace minet {

class MUDPReactor;

template <typename Delegation>
class MUDPCommandHandler;

class MUDPNetServer
{
public:
	MUDPNetServer();
	virtual ~MUDPNetServer();
	
	virtual bool Create(int nPort);
	virtual void Destroy(void);

	bool PostCommand(int nIP, int nPort, MCommand* pCommand);

protected:
	MUDPReactor* m_pReactor;
	MUDPCommandHandler<MUDPNetServer>* m_pCommandHandler;
};

}
