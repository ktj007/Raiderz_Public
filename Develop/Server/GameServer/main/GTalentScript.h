#pragma once

#include "GTalentObserver.h"
#include "GTalentDef.h"

class GTalent;
class GEntityActor;
class GTalentInfo;

//////////////////////////////////////////////////////////////////////////
//
//	GTalentScript
//
//////////////////////////////////////////////////////////////////////////

class GTalentScript : public GTalentObserver
{
public:
	// 옵저버 이벤트 ------------------------------------
	//====================================================
	virtual void	OnStart(GTalent* pTalent)				override;
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	virtual void	OnExit(GTalent* pTalent)				override	{}
	virtual void	OnEnterPhasePrepare(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseAct(GTalent* pTalent)		override;
	virtual void	OnEnterPhaseCanceled(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseFinish(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseExtra(GTalent* pTalent)		override	{}
	virtual void	OnEnterPhaseExtra2(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseAct(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseCanceled(GTalent* pTalent)	override;
	virtual void	OnLeavePhasePrepare(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseFinish(GTalent* pTalent)	override;
	virtual void	OnLeavePhaseExtra(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseExtra2(GTalent* pTalent)	override	{}
	virtual void	OnUpdatePhasePrepare(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhasePreAct(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhaseAct(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnUpdatePhaseExtra(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnUpdatePhaseExtra2(GTalent* pTalent, float fDelta) override 	{}
	virtual void	OnEnterPhaseExtra3(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseExtra3(GTalent* pTalent)	override	{}
	virtual void	OnUpdatePhaseExtra3(GTalent* pTalent, float fDelta) override 	{}

	virtual void	OnDummyCall(GTalent* pTalent, float fDelta) override {}

	void OnDelayedAct(GTalent* pTalent);

private:
	void CallEventScript( GEntityActor* pActor, cscommon_server::TALENT_TARGET_INFO& infoTarget, const wstring& strText );
};
