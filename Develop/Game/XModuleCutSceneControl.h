#pragma once
#include "XModuleActorControl.h"
#include "XCutScenePlayerActionState.h"

class XCutScenePlayerAligner;

class XModuleCutSceneControl : public XModuleActorControl, public MMemPool<XModuleCutSceneControl>
{
	friend class XCutScenePlayerActionTalent;

protected:

	//////////////////////////////////////////////////////////////////////////
	// Module 초기화
	//
	virtual void				OnInitialized();

	//////////////////////////////////////////////////////////////////////////
	// FSM
	//
	XCutScenePlayerActionFSM*			m_pActionFSM;
	void					InitStates();
	void					DoAction(CUTSCENEPLAYER_ACTION_STATE nState, void* pParam=NULL);			///< 액션 변경

	XCutScenePlayerAligner*				m_pAligner;

public:

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸 / Owner
	//
	XModuleCutSceneControl(XObject* pOwner=NULL);
	virtual ~XModuleCutSceneControl(void);

	XCutScenePlayer*					GetOwner();

public:
	virtual void	OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir);
	XCutScenePlayerAligner*				GetAligner()	{ return m_pAligner; }

protected:
	bool UsableTalent(int nTalentID);
};