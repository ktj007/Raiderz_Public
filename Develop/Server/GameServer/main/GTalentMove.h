#pragma once

#include "GTalentObserver.h"

class GTalent;
class GEntityActor;
class GTalentInfo;
class GField;

//////////////////////////////////////////////////////////////////////////
//
//	GTalentMove
//
//////////////////////////////////////////////////////////////////////////

class GTalentMove : public GTalentObserver
{
public:
	struct MOVEINFO
	{
		vec3 vPos;
		vec3 vDir;
	};

	GTalentMove();

	// 옵저버 이벤트 ------------------------------------
	//====================================================
	virtual void	OnStart(GTalent* pTalent)				override;
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	virtual void	OnExit(GTalent* pTalent)				override	{}
	virtual void	OnEnterPhasePrepare(GTalent* pTalent)	override;
	virtual void	OnEnterPhaseAct(GTalent* pTalent)		override;
	virtual void	OnEnterPhaseCanceled(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseFinish(GTalent* pTalent)	override	{}
	virtual void	OnEnterPhaseExtra(GTalent* pTalent)		override	{}
	virtual void	OnEnterPhaseExtra2(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseAct(GTalent* pTalent)		override	{}
	virtual void	OnLeavePhaseCanceled(GTalent* pTalent)	override;
	virtual void	OnLeavePhasePrepare(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseFinish(GTalent* pTalent)	override;
	virtual void	OnLeavePhaseExtra(GTalent* pTalent)		override;
	virtual void	OnLeavePhaseExtra2(GTalent* pTalent)		override;
	virtual void	OnUpdatePhasePrepare(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhasePreAct(GTalent* pTalent, float fDelta) override;
	virtual void	OnUpdatePhaseAct(GTalent* pTalent, float fDelta) override ;
	virtual void	OnUpdatePhaseExtra(GTalent* pTalent, float fDelta) override {}
	virtual void	OnUpdatePhaseExtra2(GTalent* pTalent, float fDelta) {}
	virtual void	OnEnterPhaseExtra3(GTalent* pTalent)	override	{}
	virtual void	OnLeavePhaseExtra3(GTalent* pTalent)	override;
	virtual void	OnUpdatePhaseExtra3(GTalent* pTalent, float fDelta) override 	{}

	virtual void	OnDummyCall(GTalent* pTalent, float fDelta) override {}

	// 이동 탤런트 기능 마무리
	void Finish( float fElapsedTime );
	// 이동 탤런트의 이동을 적용
	void Update( float fElapsedTime);
	/// 예상되는 위치값
	GTalentMove::MOVEINFO GetExpectedMoveInfo(float fActElapsedTime);
	
private:
	// 이동탤런트가 유효한지 여부, 유효하다면 true를 반환
	bool IsValid();
	// 이동해야될 상대좌표를 저장해둠
	void BuildMoveInfo();
	// 탤런트 진행시간에 맞는 이동정보 인덱스를 반환
	int GetMoveIndex( float fElapsedTime);
	// 인덱스에 맞는 이동정보 구조체를 반환
	MOVEINFO GetMoveInfo(int nIndex);
	
private:
	vector<MOVEINFO>	m_vecMoveInfo;
	int					m_nLastMoveIndex;
	bool				m_bBuiltOK;
	vec3				m_vInitPos;
	vec3				m_vInitDir;
	GEntityActor*		m_pActor;
	GField*				m_pField;
	GTalentInfo*		m_pMoveTalentInfo;
};