#pragma once

#include "XCutScenePlayerActionID.h"

class XCutScenePlayer;


class XCutScenePlayerAligner
{
public:
	class XAlignChecker
	{
	public:
		//static bool	IsEnemy(XNonPlayer* pOwner);
		//static bool	CheckMotion(XModuleMotion* pModuleMotion);
		//static bool	CheckAngleEnabled(vec3 vDir1, vec3 vDir2, float fEnemyDistance, bool bMoving);
		static bool	CheckNPCInfo( XCutScenePlayer* pOwner );
		//static bool	CheckStunAndRooted(bool bIsStun, XBuffAttribute& buffAttribute);
		static bool CheckEnemyList( XCutScenePlayer* pOwner );
	};

protected:
	CUTSCENEPLAYER_ACTION_STATE			m_nActionState;

	bool	m_bEnabled;

	void	SetEnabled( XModuleEntity* pModuleEntity, bool bAlign );

public:
	XCutScenePlayerAligner();
	virtual ~XCutScenePlayerAligner();

	void OnEnterState(XCutScenePlayer* pOwner, CUTSCENEPLAYER_ACTION_STATE nActionState);
	void OnExitState(XCutScenePlayer* pOwner);
};