#pragma once

class GTalent;

__interface GTalentObserver
{
	virtual void	OnStart(GTalent* pTalent);
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	virtual void	OnExit(GTalent* pTalent);
	virtual void	OnEnterPhasePrepare(GTalent* pTalent);
	virtual void	OnEnterPhaseAct(GTalent* pTalent);
	virtual void	OnEnterPhaseCanceled(GTalent* pTalent);
	virtual void	OnEnterPhaseFinish(GTalent* pTalent);
	virtual void	OnEnterPhaseExtra(GTalent* pTalent);
	virtual void	OnEnterPhaseExtra2(GTalent* pTalent);
	virtual void	OnEnterPhaseExtra3(GTalent* pTalent);
	virtual void	OnLeavePhaseAct(GTalent* pTalent);
	virtual void	OnLeavePhaseCanceled(GTalent* pTalent);
	virtual void	OnLeavePhasePrepare(GTalent* pTalent);
	virtual void	OnLeavePhaseFinish(GTalent* pTalent);
	virtual void	OnLeavePhaseExtra(GTalent* pTalent);
	virtual void	OnLeavePhaseExtra2(GTalent* pTalent);
	virtual void	OnLeavePhaseExtra3(GTalent* pTalent);
	virtual void	OnUpdatePhasePrepare(GTalent* pTalent, float fDelta);
	virtual void	OnUpdatePhasePreAct(GTalent* pTalent, float fDelta);
	virtual void	OnUpdatePhaseAct(GTalent* pTalent, float fDelta);
	virtual void	OnUpdatePhaseExtra(GTalent* pTalent, float fDelta);
	virtual void	OnUpdatePhaseExtra2(GTalent* pTalent, float fDelta);
	virtual void	OnUpdatePhaseExtra3(GTalent* pTalent, float fDelta);


	virtual void	OnDummyCall(GTalent* pTalent, float fDelta);
};
