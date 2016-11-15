#pragma once

class LTimeoutHandler;
class LTimeoutWatcher;
class LMoveServerTimeoutHandler;

class LTimeoutManager
{
public:
	LTimeoutManager();
	virtual ~LTimeoutManager();

	void InitWatcher(void);
	void Update(void);	
	void StopAllWatching(MUID uidPlayer);

	void StartWatchingSelMyCharTimeout(MUID uidPlayer, LTimeoutHandler* pHandler);
	void StopWatchingSelMyCharTimeout(MUID uidPlayer);
	void StartWatchingMoveServerTimeout(MUID uidPlayer);
	void StopWatchingMoveServerTimeout(MUID uidPlayer);

private:
	LTimeoutWatcher*			m_pSelMyCharWatcher;		///< 플레이어 캐릭터 선택 타임아웃 처리자
	LTimeoutWatcher*			m_pMoveServerWatcher;		///< 플레이어 서버이동 접속종료 타임아웃 감시자
	LMoveServerTimeoutHandler*	m_pMoveServerTimeoutHandler;///< 다른 서버로 릴레이되는 요청/응답 커맨드 타임아웃 감시자
};
