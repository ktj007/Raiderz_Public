#pragma once

#include "MUID.h"
#include "GTickRegulator.h"
#include "MTime.h"
#include "MMemPool.h"
#include "GHateTableDef.h"
#include "GHateTableSwapper.h"
#include "GHateTableLocker.h"
#include "GActorObserver.h"

class GEntityNPC;
class GEntityPlayer;
class GEntityActor;

#define HATE_ABSOLUTE_AMOUNT	999999

TEST_FORWARD_FT(HateTable, Fixture, EntryTimeout);
TEST_FORWARD_FT(HateTable, Fixture, EntryTimeout_NPCSpecificSetting);

class GHateTable : private GActorObserver, public MMemPool<GHateTable>
{
	TEST_FRIEND_FT(HateTable, Fixture, EntryTimeout);
	TEST_FRIEND_FT(HateTable, Fixture, EntryTimeout_NPCSpecificSetting);
private:
	typedef multimap<HatePoint, MUID> HatePoint_MAP_REVERSE;
	typedef map<MUID, float> HateTimeoutMap;
public:
	GHateTable(GEntityNPC* pOwner);
	~GHateTable();
		
	// 초기화
	void Init(float fExpiredTime);
	// 업데이트
	void	Update(float fDelta);
	// 내부 변수 초기화
	void	Reset();

	// 헤이트 테이블 목표 액터UID 반환
	MUID	GetTarget() const;
	// 헤이트테이블 최상위 액터UID들 반환
	void	GetHighestTargetsUID(int nQty, vector<MUID>& vecTargets) const;
	// 헤이트테이블 최상위 액터UID들 반환
	void	GetHighestTargets(int nQty, vector<pair<MUID,HatePoint>>& vecTargets) const;
	// 헤이트테이블 엔트리 갯수
	size_t	GetQty() const;
	// 헤이트테이블 비었는지 여부
	bool	IsEmpty() const;
	// 해당 UID가 헤이트테이블에 등록되어있는지 여부
	bool	IsExist(const MUID& id) const;
	// 해당 UID의 헤이트 수치 반환 
	HatePoint	GetHate(const MUID& id) const;
	// 해당 UID의 엔트리 타임아웃 시간
	float	GetTimeoutTime(const MUID& id) const;
	// 만료 시간 반환
	float	GetExpiredTime() const;

	// 헤이트 엔트리 제거
	void	DropPoint( MUID id );
	// 헤이트 감소 여부
	void	EnableTimeout(bool bSwitch);
	// 헤이트 스왑 이벤트 적용
	bool	ApplyEvent(HATE_TARGET_SWAP_EVENT nEventType, float fDuration);
	// 대상 액터를 상대로 고정함
	void	Lock(MUID uidTarget, float fDurationTime);
	// 타운트가 걸릴지 여부
	void	SetTauntable(bool bEnable);
	// 타운트가 걸릴 수 있는지 여부
	bool	IsTauntable();
	
	// 헤이트 추가 함수들 ----------
	void	AddPoint_FoundEnemy(GEntityActor* pActor);	
	void	AddPoint_GainDamage(GEntityActor* pActor, int nDamage, float fHateFactor = 1.0f);
	void	AddPoint_EnemyApplyEffect(GEntityActor* pActor, int nHateAdj);
	void	AddPoint_EnemyRecoverHP(GEntityActor* pActor, int nRecoveredHP, float fHateFactor = 1.0f);
	void	AddPoint_Custom(GEntityActor* pActor, int nAmount);	

	// 타겟 UID들 반환
	void	CollectTarget(vector<MUID>& vecTargets) const;

private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;
	// 자신이 죽었을때 호출되는 이벤트
	virtual void OnDie() override;
	// 자신이 디스폰 되었을때 (NPC 전용)
	virtual void OnDespawn() override;
	// 자신의 객체가 소멸될때
	virtual void OnDestroy() override;
	// 자신의 헤이트테이블에 타켓이 추가되었을때
	virtual void OnAddHateEntry( GEntityActor* pEnemy ) override;
private:
	// 헤이트 엔트리 추가
	void InsertEntry(MUID uid, HatePoint point);
	// 헤이트 엔트리 제거
	void DeleteEntity(const MUID& uid);
	// 헤이트 엔트리 제거 및 옵저버에 알림
	void Drop(const MUID& uid);
	// 헤이트테이블 엔트리들 수치감소
	void UpdateTimeout(float fElapsedTime);
	// 헤이트테이블 랭킹 갱신
	void	UpdateHateRanking();
	// 헤이트테이블 수치 적용
	void	AdjHatePoint(GEntityActor* pActor, HatePoint point);
	// 해당 UID로 액터가 유효한지 검사
	bool	IsValidTarget(MUID id) const;
	// 해당 UID로 액터 객체 반환
	GEntityActor* FindActor(const MUID& id);
	// 해당 타켓이 허용 높이 범위에 있는지 검사
	bool CheckHeight(GEntityPlayer* pEnemy);

private:
	GEntityNPC*			m_pOwner;
	HatePointMap		m_mapHateTable;
	MUID				m_uidLastTarget;
	GHateTableSwapper	m_Swapper;
	GHateTableLocker	m_TargetLocker;
	MRegulator			m_rgrTick;
	HateTimeoutMap		m_mapTimeout;
	bool				m_bEnableTimeout;
	float				m_fExpiredTime;
};