#pragma once

#include "XInterpolation.h"
#include "XBuffAttribute.h"
#include "XNPCActionID.h"

class XNonPlayer;
class XModuleMotion;
class XModuleEntity;


class XNPCAligner
{
public:
	class XTerrainAligner
	{
	public:
		void Update(XNonPlayer* pOwner, bool bAligningEnemyNow);
	};

	class XAlignCalculator
	{
	public:
		static vec3		CalcEnemyDir(vec3& vOwnerPosition);
		static vec3		CalcMoveDir(vec3& vOwnerPosition, vec3& vTarPos);
		static float	CalcDistance(vec3& vOwnerPosition);
		static vec3		CalcThisNPCDir(XNonPlayer* pOwner);
	};

	class XAlignChecker
	{
	public:
		static bool	IsEnemy(XNonPlayer* pOwner);
		static bool	CheckMotion(XModuleMotion* pModuleMotion);
		static bool	CheckAngleEnabled(vec3 vDir1, vec3 vDir2, float fEnemyDistance, bool bMoving);
		static bool	CheckNPCInfo( XNonPlayer* pOwner );
		static bool	CheckStunAndRooted(bool bIsStun, XBuffAttribute& buffAttribute);
		static bool CheckEnemyList( XNonPlayer* pOwner );
	};
protected:
	XTerrainAligner				m_TerrainAligner;

	XDirectionInterpolation		m_DirInterpo;

	NPC_ACTION_STATE			m_nActionState;

	bool	m_bInterpolation;
	bool	m_bEnabled;
	bool	m_bOnTalentState;

	void	SetEnabled( XModuleEntity* pModuleEntity, bool bAlign );

	//bool UpdateInterpolation(XNonPlayer* pOwner, float fDelta);
	void StartInterpolation(XNonPlayer* pOwner, vec3 vToDir);
	void DoAlignEnemy( XNonPlayer* pOwner );

	virtual void SetRenderDirection( XNonPlayer* pOwner, vec3 vDir );

	void UpdateTerrainAlign(XNonPlayer* pOwner);
	void UpdateEnemyAlign( XNonPlayer* pOwner, float fDelta );
	void _UpdateEnemyAlignOnMoveState( XNonPlayer* pOwner, float fDelta );
	void _UpdateEnemyAlignOnTalentState( XNonPlayer* pOwner, float fDelta );
public:
	XNPCAligner();
	virtual ~XNPCAligner();

	void Update(XNonPlayer* pOwner, float fDelta);

	void OnEnterMoveState(XNonPlayer* pOwner, vec3& vTarPos);
	void OnEnterState(XNonPlayer* pOwner, NPC_ACTION_STATE nActionState);
	void OnExitState(XNonPlayer* pOwner);

	void OnMoveUpdate(XNonPlayer* pOwner, vec3& vTarPos);

	bool IsEnabled() { return m_bEnabled; }
};


