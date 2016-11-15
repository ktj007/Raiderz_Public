#pragma once
#include "CSdef.h"
#include "GTalentObserver.h"
#include "GActorObserver.h"

class GEntityActor;

class GActorCooltimeChecker: public GTalentObserver, public GActorObserver, public MTestMemPool<GActorCooltimeChecker>
{
private:
	typedef map<int, pair<float, float> > MAP_COOLTIME;

	MAP_COOLTIME m_mapTalentID;
	GEntityActor* m_pOwner;

private:
	void RouteEndTalentCooldown(GEntityActor* pOwner, int nTalentLine);
	void UpdateCoolTime(MAP_COOLTIME& mapCoolTime, float fDelta);
private:
	// 탤런트 옵저버 이벤트 ------------------------------------
	//====================================================
	virtual void	OnStart(GTalent* pTalent)				override	{}
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	virtual void	OnExit(GTalent* pTalent)				override	{}
	virtual void	OnEnterPhasePrepare(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseAct(GTalent* pTalent)		override;
	virtual void	OnEnterPhaseCanceled(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseFinish(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseExtra(GTalent* pTalent)		override	{}
	virtual void	OnEnterPhaseExtra2(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseAct(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseCanceled(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhasePrepare(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseFinish(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseExtra(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseExtra2(GTalent* pTalent)		override	{}
	virtual void	OnUpdatePhasePrepare(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhasePreAct(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhaseAct(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnUpdatePhaseExtra(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnUpdatePhaseExtra2(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnEnterPhaseExtra3(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseExtra3(GTalent* pTalent)	override	{}
	virtual void	OnUpdatePhaseExtra3(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnDummyCall(GTalent* pTalent, float fDelta) override {}

public:
	GActorCooltimeChecker(GEntityActor* pOwner);
	virtual ~GActorCooltimeChecker();

	void InsertTalentCooltime(int nTalentLine, float fLimitTime);
	void Update(float fDelta);
	void Clear();
	bool IsCooldown( int nTalentID );
	void GetTalentRemainCoolTime(vector<pair<int, float>>& outvecTalentRemainCoolTime);
	void InsertTalentRemainCoolTime(const vector<pair<int, float>>& vecTalentRemainCoolTime);

	static vector<int> GetRelativeTalentsID(GTalentInfo* pTalentInfo);
	vector<string> GetDescription() const;

};
