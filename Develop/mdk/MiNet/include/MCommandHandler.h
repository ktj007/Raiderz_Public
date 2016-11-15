#ifndef _MCOMMAND_HANDLER_H
#define _MCOMMAND_HANDLER_H

#include "MiNetLib.h"
#include "MiNetCommon.h"

#include <vector>
using namespace std;

namespace minet {

class MCommand;
class MCommandCommunicator;
class MCommandHandler;

typedef MCommandResult (MCommandHanderFunc) (MCommand* pCommand, MCommandHandler* pHandler);

/// 커맨드 핸들러 기반 클래스
class MINET_API MCommandHandler
{
protected:
	vector<int>					m_vecCmdID;
	MCommandCommunicator*		m_pCC;
protected:
	void SetCmdHandler(int nID, MCommandHanderFunc* fnFunc);
public:
	MCommandHandler(MCommandCommunicator* pCC) : m_pCC(pCC) {}
	virtual ~MCommandHandler();
	MCommandCommunicator* GetCommandCommunicator() const { return m_pCC; }
};

#define DECL_CMD_HANDLER(_func)			static MCommandResult _func(MCommand* pCommand, MCommandHandler* pHandler);
#define IMPL_CMD_HANDLER(_class, _func) MCommandResult _class::_func(MCommand* pCommand, MCommandHandler* pHandler)


} // namespace minet

#endif