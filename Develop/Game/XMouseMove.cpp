#include "stdafx.h"
#include "XMouseMove.h"
#include "XModuleCollision.h"
#include "XEffectManager.h"

#define MOUSE_MOVE_CLICK_EFFECT_NAME L"move_click"

bool XMouseMove::OnClick( int x, int y, vec3& vMove )
{
	if (global.device == NULL) return false;

	vec3 vDir, vPos;
	vec3 vTempNor;

	if(GetTrans3DCoordinates(x, y, vPos, vDir) == false)
		return false;

	if(CheckMoveTerrain(vPos, vDir, vMove, vTempNor))
	{
		float fDot = vTempNor.DotProduct(vec3(0, 0, 1));
		if(fDot > 0.75f)
		{
			DrawMovePoint(vMove);
			return true;
		}
	}

	return false;
}

bool XMouseMove::CheckMoveTerrain( vec3 vPos, vec3 vDir, vec3& vOut, vec3& vNor )
{
	if(m_pOwner == NULL)
		return false;

	if(m_pOwner->GetModuleCollision())
	{
		const float PICK_MIN_Z = TERRAIN_HEIGHT_COLLISION_VALUE;
		const float PICK_MAX_Z = TERRAIN_HEIGHT_COLLISION_VALUE * -1.0f;

		vec3 vMovePos = vPos + (vDir * 5000.0f);

		if(m_pOwner->GetModuleCollision()->PickRayToTerrain(vPos, vMovePos, vOut, vNor, CollisionAttribute::ALL))
		{
			return true;
		}

		vec3 vStartPos = vMovePos + vec3(0, 0, PICK_MAX_Z);
		vec3 vEndPos = vMovePos + vec3(0, 0, PICK_MIN_Z);

		if(m_pOwner->GetModuleCollision()->PickRayToTerrain(vStartPos, vEndPos, vOut, vNor, CollisionAttribute::ALL))
		{
			return true;
		}

	}

	return false;
}

void XMouseMove::DrawMovePoint( vec3 vPos )
{
	if(m_vecEffectData.empty() == false)
		DelMovePoint();

	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(wstring(MOUSE_MOVE_CLICK_EFFECT_NAME), vPos, vec3(vec3::AXISY), vec3(vec3::AXISZ), &m_vecEffectData);
}

void XMouseMove::DelMovePoint()
{
	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecEffectData.begin(); it != m_vecEffectData.end(); ++it)
	{
		if(it->type == CET_MODEL)
		{
			global.emgr->DeleteXEffect(it->uidEffect);
		}
		else if(it->type == CET_SOUND)
		{
			if ( global.sound)
				global.sound->StopDynamic3D( it->uidEffect);
		}
	}

	m_vecEffectData.clear();
}
