#include "stdafx.h"
#include "XCutScenePlayerAligner.h"
#include "XCutScenePlayer.h"
#include "XModuleEntity.h"

XCutScenePlayerAligner::XCutScenePlayerAligner() : m_bEnabled(false), m_nActionState(CUTSCENEPLAYER_ACTION_STATE_NONE)
{
}

XCutScenePlayerAligner::~XCutScenePlayerAligner()
{
}

void XCutScenePlayerAligner::SetEnabled( XModuleEntity* pModuleEntity, bool bAlign )
{
	if (m_bEnabled == bAlign) return;

	m_bEnabled = bAlign;

	if (bAlign == false)
	{
		pModuleEntity->StopDirectionInterpolation();
		pModuleEntity->SetRenderDirection(vec3::ZERO);
	}
}

void XCutScenePlayerAligner::OnEnterState( XCutScenePlayer* pOwner, CUTSCENEPLAYER_ACTION_STATE nActionState )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false ||
		XAlignChecker::CheckEnemyList(pOwner) == false)
	{
		return;
	}

	m_nActionState = nActionState;

	switch (nActionState)
	{
	/*case CUTSCENEPLAYER_ACTION_STATE_NA:
	case CUTSCENEPLAYER_ACTION_STATE_MOVE:
		{
			if (pOwner->GetInfo(true)->bAlignEnemy)
			{
				m_bEnabled = true;
			}
		}
		break;*/
	case CUTSCENEPLAYER_ACTION_STATE_TALENT:
		{
			m_bEnabled = true;
		}
		break;
	}


}

void XCutScenePlayerAligner::OnExitState( XCutScenePlayer* pOwner )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false ||
		XAlignChecker::CheckEnemyList(pOwner) == false)
	{
		return;
	}

	switch (m_nActionState)
	{
	/*case CUTSCENEPLAYER_ACTION_STATE_NA:
	case CUTSCENEPLAYER_ACTION_STATE_MOVE:
		{
			if (pOwner->GetInfo(true)->bAlignEnemy)
			{
				SetEnabled(pOwner->GetModuleEntity(), false);
			}
		}
		break;*/
	case CUTSCENEPLAYER_ACTION_STATE_TALENT:
		{
			SetEnabled(pOwner->GetModuleEntity(), false);
		}
		break;
	}

	m_nActionState = CUTSCENEPLAYER_ACTION_STATE_NONE;
}



///////////////////////////////////////////////////////////////////////////////////

bool XCutScenePlayerAligner::XAlignChecker::CheckNPCInfo( XCutScenePlayer* pOwner )
{
	if (pOwner == NULL) return false;
	if (pOwner->GetInfo(true) == NULL) return false;

	return true;
}

bool XCutScenePlayerAligner::XAlignChecker::CheckEnemyList( XCutScenePlayer* pOwner )
{
	vector<TD_ENEMY_INFO>& enemylist = gvar.Game.EnemyInfo.GetEnemyList();
	for(vector<TD_ENEMY_INFO>::iterator it = enemylist.begin(); it != enemylist.end(); ++it)
	{
		if(it->nUIID == pOwner->GetUIID())
			return true;
	}

	return false;
}

