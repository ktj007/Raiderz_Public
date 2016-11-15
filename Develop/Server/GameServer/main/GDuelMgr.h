#pragma once

class GEntityPlayer;
class GDuel;
class GField;

//
// GDuelMgr
//
class GDuelMgr : public MTestMemPool<GDuelMgr>
{
public:
	GDuelMgr(GField* pField);
	~GDuelMgr(void);

	// 매니저가 소멸될때 호출
	void Fini();

	// 결투 요청.	결투가 불가능한 경우에는 RouteFailed() 함수로 플레이어에게 이유를 알려준다.
	//				반환값은 요청이 가능한지 여부
	bool DuelRequest(GEntityPlayer* pRequester, MUID uidTargetPlayer);
	// 결투 요청.	결투가 불가능한 경우에는 RouteFailed() 함수로 플레이어에게 이유를 알려준다.
	//				반환값은 요청이 가능한지 여부
	bool DuelRequest(const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2);
	// 결투 요청에 대한 응답.  요청 이후에만 사용될 수 있다.
	void DuelQuestionRespond(GDuel* pDuel, bool bAnswer);

	// 결투중에 죽을 경우 호출하는 함수, 반환값은 결투상대에게 죽었는지 여부
	bool DuelKilled(GEntityPlayer* pDeadMan, MUID uidKiller);

	// 결투중인 결투자가 접속을 종료했을때 호출되는 함수
	void Disconnect(GEntityPlayer* pDisconnector);

	// 결투 객체의 갯수를 반환
	int	 GetCount();

	// 틱마다 호출됨
	void Update(float fDelta);

	// 결투 상대자인지 여부
	bool IsDuelOpponent(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const;

	void InsertDuel(const MUID& uidDuel, GDuel* pDuel);

	// 결투 객체를 찾을떄 호출
	GDuel* FindDuel(MUID uidDuel);
private:
	
	// 결투 객체를 생성하고 매니저 목록에 등록시킴
	GDuel* CreateDuel(GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer);
	GDuel* CreateDuel(const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2);
	// 결투요청을 수락할때 호출
	void DuelAccept( GDuel* pDuel );
	
	// 결투가 이루어질 수 없는 경우에 호출되는 함수 (결투요청거절등)
	void DuelCancel(GDuel* pDuel);

	// 결투 객체를 목록에서 삭제시킴
	void EraseDuel(GDuel* pDuel);

	// 결투 요청이 가능한지 확인하고, 요청이 불가능하면 RouteFailed()를 호출
	bool CheckDuelRequest( GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer );
	bool CheckPartyDuelRequest( GEntityPlayer* pRequester, const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2 );
	// 결투 요청 커맨드라우터
	void RouteQuestion(GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer);	
	
protected:
	typedef std::map<MUID, GDuel*>	MAP_DUEL;
	MAP_DUEL		m_mapDuels;	
	GField*			m_pField;
};
