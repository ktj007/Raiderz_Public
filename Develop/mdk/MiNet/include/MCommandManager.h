#ifndef MCOMMANDMANAGER_H
#define MCOMMANDMANAGER_H

#include "MiNetLib.h"
#include "MThread2.h"
#include "MCommand.h"
#include "MCommandParameter.h"
#include "MCommandHandler.h"
#include <map>
#include <string>
using namespace std;

namespace minet {

typedef list<MCommand*>			MCommandList;		///< Command List

#define DEFAULT_COMMAND_QUEUE_MAX_SIZE			500000		// 50만개 이상 큐에 쌓이면 커맨드 추가하지 않음

/// 커맨드 매니져
/// - 커맨드 디스크립션 관리
/// - 커맨드 큐 관리
/// - 쓰레드에 안전함.
class MINET_API MCommandManager
{
protected:
	cml2::MCriticalSection		m_CommandQueueLock;
	MCommandList				m_CommandQueue;			///< Posted Command List
	size_t						m_nMaxQueueSize;
public:
	MCommandManager(void);
	virtual ~MCommandManager(void);
	void Clear();

	/// 커맨드 Posting
	bool Post(MCommand* pNew);

	/// 가장 먼저 추가된 커맨드를 읽어들임
	MCommand* GetCommand(void);

	/// 커맨드 큐 최대 크기 수정
	void SetMaxQueueSize(size_t nSize)	{ m_nMaxQueueSize = nSize; }

	/// 커맨드 큐 최대 크기
	size_t GetMaxQueueSize()			{ return m_nMaxQueueSize; }

	/// 핸들러 연결
	void SetCommandHandler(int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc);

	size_t GetCommandQueueCount(void);

	/// Command Description의 문법 설명 스트링 얻어내기
	void GetSyntax(char* szSyntax, int nSyntaxBufferSize, const MCommandDesc* pCD);
};


} // namespace minet

#endif