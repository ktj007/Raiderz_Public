#pragma once
#include "XDummyBot_Roam.h"

class XDummySpawnMgr;
class XDummyLazyCommandPoster;

class XDummyBot_Loot : public XDummyBot_Roam
{
	enum PHASE
	{
		PHASE_INIT_REPEAT,
		PHASE_INIT_DONE,
		PHASE_KILL,
		PHASE_BEGIN,
		PHASE_SHOW,
		PHASE_LOOT,
		PHASE_REFRESHITEM,
		PHASE_END,
	};

public:
	XDummyBot_Loot(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Loot(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Loot"; }

protected:
	virtual void OnRun(float fDelta);

private:
	vector<MUID> GetRandomLootItem();
	
	void ChangePhase(PHASE nPhase);
	bool CheckPostTime();

private:
	XDummySpawnMgr* m_pSpawnMgr;
	UIID			m_nTargetUIID;
	MUID			m_nTargetUID;
	PHASE			m_nPhase;

	vector<TD_ITEM_DROP>		m_vecTDDropItem;
	MRegulator*		m_pPhaseRegulator;
	
};

//////////////////////////////////////////////////////////////////////////
///
/// 동작원리 :	spawn_id 와 동일한 NPC 를 찾아 죽인 뒤 루팅합니다.
///				이미 필드에 max_spawn 값 이상의 spawn_id 와 동일한 NPC 가 있을 경우 스폰하지 않습니다.
///				스폰은 매 spawn_tick 초마다 합니다.
///	
///	주의사항 :	선공몹이 있는 곳을 피하십시오.
///				spawn_id 에 기술되는 NPC 가 선공몹이 아니게 하십시오. 진행상황을 예측할 수 없습니다.
///				실제 spawn_id 의 몹이 소환되는 지역을 피하십시오. 서버의 스폰 정책에 종속되어 버립니다.
///
//////////////////////////////////////////////////////////////////////////