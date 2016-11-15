#pragma once

#define DECL_WL_CMD_HANDLER(_func)			static bool _func(DWORD dwIP, WORD wRawPort, minet::MCommand* pCommand, minet::MUDPNetClient* pHandler);
#define IMPL_WL_CMD_HANDLER(_class, _func)	bool _class::_func(DWORD dwIP, WORD wRawPort, minet::MCommand* pCommand, minet::MUDPNetClient* pHandler)


namespace minet {

class MUDPReactor;

template <typename Delegation>
class MUDPCommandHandler;

class MUDPNetClient
{
public:
	MUDPNetClient();
	virtual ~MUDPNetClient();

	virtual bool Create(int nPort, int nServerIP, int nServerPort);
	virtual void Destroy(void);

	void SetServerAddress(int nServerIP, int nServerPort);

	bool PostCommand(MCommand* pCommand);

protected:
	MUDPReactor* m_pReactor;
	MUDPCommandHandler<MUDPNetClient>* m_pCommandHandler;
	int m_nServerIP;
	int m_nServerPort;
};

}
