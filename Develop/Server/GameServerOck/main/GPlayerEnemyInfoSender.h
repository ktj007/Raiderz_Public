#pragma once

#include "GActorObserver.h"
#include "MTime.h"
#include "CTransData.h"

class GEntityPlayer;
class GEntityActor;

class GPlayerEnemyInfoSender : private GActorObserver, public MTestMemPool<GPlayerEnemyInfoSender>
{
public:
	GPlayerEnemyInfoSender(GEntityPlayer* pOwner);
	~GPlayerEnemyInfoSender();
	void Update(float fDelta);	
	void Clear();

	// 적이 있는지 여부
	bool HasEnemy() const;


	bool IsObserveDuel() const;
	bool AttachObserveDuel(MUID uidDuel);
	bool DetachObserveDuel();
private:
	// 적 정보 갱신
	void UpdateEnemyEntry();

	TD_ENEMY_INFO MakeTD( GEntityActor* pEachActor );
	void RouteInfo();
	void RouteClearInfo();	

	// 해당 UID로 액터 객체 반환
	GEntityActor* FindActor(const MUID& id);
	int			  CalcCheckSum();

	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
	// 상대가 자신을 헤이트테이블에서 추가하였을때
	virtual void OnAddHateTarget( GEntityActor* pEnemy ) override;
	// 상대가 자신을 헤이트테이블에서 제거하였을때
	virtual void OnLostHateTarget( MUID uidEnemy ) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;
	// 결투 준비
	virtual void OnDuelReady( const vector<MUID>& pEnemy ) override;
	// 결투 종료
	virtual void OnDuelFinish( const vector<MUID>& pEnemy ) override;
	
private:
	GEntityPlayer*	m_pOwner;
	MRegulator		m_rgrTick;	
	set<MUID>		m_setEnemies;	

	MUID			m_uidLastHittedEnemy;	///< 마지막으로 플레이어에게 맞은 적
	UIID			m_nLastHitEnemy;		///< 마지막으로 플레이어에게 맞은 적
	int				m_nOldCheckSum;			///< 값이 바뀌어서 라우팅할지 결정하는데 쓰임	
	MUID			m_uidObserveDuel;		///< 관람중인 결투
};
