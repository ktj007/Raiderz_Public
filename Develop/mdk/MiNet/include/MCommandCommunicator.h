#ifndef MCOMMANDCOMMUNICATOR_H
#define MCOMMANDCOMMUNICATOR_H

#include "MiNetLib.h"
#include "MiNetCommon.h"
#include "MiNetCommonDef.h"
#include "MCommandManager.h"
#include "MUID.h"
#include "MPacketCrypter.h"
#include "MCommandHandler.h"
#include "MCommandHistory.h"
#include "MThread2.h"

namespace minet {

class MCommandStream;
class MHandlerBase;
class MCommandProfiler;


/// 커맨드 기반의 통신을 목적으로한 커뮤니케이터
class MINET_API MCommandCommunicator
{
private:
	enum CrowdState					
	{
		MCS_COMFORTABLE = 0,
		MCS_CROWDED,
		MCS_OVERFULL
	};

private:
	cml2::MCriticalSection		m_csSafeCmdQueue;
	MCommandList				m_SafeCmdQueue;

	cml2::MCriticalSection		m_csNewUID;
protected:
	MUIDGenerator				m_uidGenerator;

	MCommandProfiler*			m_pCommandProfiler;
	MCommandHistory				m_CommandHistory;

	MCommandManager				m_CommandManager;					///< 커맨드 매니저
	MUID						m_This;								///< 자기 커뮤니케이터 UID
		
	CrowdState					m_nCrowdState;

protected:
	virtual MCommand* GetCommand();

	/// Low-Level Command Transfer Function. 나중에 모아두었다가 블럭 전송등이 가능하게 해줄 수 있다.
	virtual void SendCommand(MCommand* pCommand) {}
	/// Low-Level Command Transfer Function. 나중에 모아두었다가 블럭 전송등이 가능하게 해줄 수 있다.
	virtual void ReceiveCommand(MCommand* pCommand);

	/// 커뮤니케이터가 커맨드를 처리하기 위해 불리는 함수
	virtual MCommandResult OnCommand(MCommand* pCommand);
	/// 커뮤니케이터 루프 전 준비
	virtual void OnPrepareRun(void);
	/// 커맨드를 처리하기 전에
	virtual void OnPrepareCommand(MCommand* pCommand);
	/// 커뮤니케이터 루프
	virtual void OnRun(void) {}
	void Clear();

	/// 복잡도
	virtual void OnCheckCrowded()	{}

	void SetCrowded();
	void SetOverfull();
	bool IsOverfull();
	bool IsCrowded();
	bool IsComfortable();

public:
	MCommandCommunicator(void);
	virtual ~MCommandCommunicator(void);

	/// 프로파일러 활성화
	void InitProfiler();
	void ProfileAnalysis(const char* szFolder, const char* szFileName, bool bSaveCSV);

	/// This UID 설정
	void SetUID(MUID& uidThis)		{ m_This = uidThis; }

	/// 커맨드 입력 - 쓰레드에 안전함.
	virtual bool Post(MCommand* pCommand);

	/// 실행 ( 커맨드 전송및 처리 )
	void Run(void);

	/// 커맨드 매니져 얻기
	MCommandManager* GetCommandManager(void)	{ return &m_CommandManager; }

	/// 커맨드 생성
	MCommand* NewCommand(int nCmdID, MUID TargetUID);
	MCommand* NewLocalCommand(int nCmdID);

	/// 커맨드 핸들러 연결
	virtual void SetCommandHandler(int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc);

	/// 자신의 UID를 얻어낸다.
	MUID GetUID(void){ return m_This; }

	MUID NewUID();

	/// 커맨드 프로파일러 연결. delete도 여기서 해준다.
	void AttachCommandProfiler(MCommandProfiler* pCommandProfiler)
	{
		m_pCommandProfiler = pCommandProfiler;		
	}
};


} // namespace minet

#endif
