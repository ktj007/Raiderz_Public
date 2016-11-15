#include "stdafx.h"
#include "XTargetCollisionBox.h"
#include "XNonPlayer.h"
#include "XCameraManager.h"

XTargetCollisionBox::XTargetCollisionBox(XObject* pObject)
: XTargetLine(pObject)
{

}

XTargetCollisionBox::~XTargetCollisionBox()
{

}

void XTargetCollisionBox::GetCameraInfo( vec3& vPos, vec3& vDir )
{
	// 카메라 위치 정보
	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
	if(pCameraNode)
	{
		vDir = pCameraNode->GetDirection();
		vPos = m_pOwner->GetPosition();
		vPos.z += 100.0f;
	}
}

bool XTargetCollisionBox::CheckTargetAllowed( XObject* pObject )
{
	PFC_THISFUNC;

	// 타겟이 NPC나 PLAYER가 아니라면 타겟에서 제외한다.
	if(pObject->GetEntityType() != ETID_NPC && pObject->GetEntityType() != ETID_PLAYER)
	{
		return false;
	}

	if(CheckMe(pObject))	return false;

	if (IsActorDead(pObject))
	{
		if(pObject->GetEntityType() == ETID_NPC)
		{
			XNonPlayer* pNPC = pObject->AsNPC();
			if(pNPC->IsLootable())	return true;
		}

		return false;
	}

	if(pObject->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = pObject->AsNPC();
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		if(pNPCInfo->nNpcType == NPC_TYPE_OBJECT &&
			pNPC->GetIConState() == NIS_NONE)
			return false;
	}


	return true;
}

// TODO : 인터랙션 체크하는 부분에 XTarget을 적용시킬수 있을까?
bool XTargetCollisionBox::CheckLineTargetObj( XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist )
{
	PFC_THISFUNC;

	XModuleCollision* pModuleCollision = pObject->GetModuleCollision();
	if(pModuleCollision)
	{
		vec3 vDir = dir;
		vDir.z = 0.f;

		float fCheckX = CHECK_INTERACTION_COL_Z - 50.f;
		RBoundingBox CollisionBox;
		CollisionBox.vmax.Set( fCheckX, 9999.f, CHECK_INTERACTION_COL_Z*2.f);
		CollisionBox.vmin.Set(-fCheckX,    0.f,-CHECK_INTERACTION_COL_Z*2.f);

		if (pModuleCollision->GetObjectColBox(FIND_ALL, pos, vDir, CollisionBox, fOutDist))
		{
			outTargetObject.uidTarget			= pObject->GetUID();
			outTargetObject.vTargetPos			= pos + (vDir * fOutDist);

			return true;
		}
	}

	return false;
}

bool XTargetCollisionBox::CheckTargetObj( XObject* pObject)
{
	PFC_THISFUNC;

	if(CheckTargetAllowed(pObject) == false)
		return false;

	if( m_pTarget &&
		m_pTarget->uidTarget == pObject->GetUID())
	{
		return false;
	}

	DelTargetAll();

	m_pTarget = new stTargetID();
	m_pTarget->uidTarget		= pObject->GetUID();
	m_pTarget->time				= timeGetTime();

	return true;
}

void XTargetCollisionBox::SetIndicatorEffect()
{
	if (XGETCFG_GAME_INDICATOR == false) return;
	if (m_pTarget == NULL) return;

	XObject* pTargetObject = gg.omgr->FindObject(m_pTarget->uidTarget);

	if (!pTargetObject) return;

	if(pTargetObject && pTargetObject->GetType() == XOBJ_NPC)
	{
		// npc이면
		XNonPlayer* pNPC = pTargetObject->AsNPC();
		XNPCInfo* pNpcInfo = pNPC->GetInfo(true);
		if(pNpcInfo && pNpcInfo->IsShowIndicator() == false)
			return;
	}

	m_pTarget->uidEffect = m_TargetRenderer.SetIndicatorEffect(pTargetObject);
}

void XTargetCollisionBox::ChangeIndicatorEffectPos()
{
	if (XGETCFG_GAME_INDICATOR == false) return;
	if (m_pTarget == NULL) return;
	if (m_pTarget->uidEffect == MUID::ZERO) return;

	m_TargetRenderer.DelTargetEffect(m_pTarget->uidEffect);

	SetIndicatorEffect();
}

void XTargetCollisionBox::DeleteIndicatorEffect()
{
	if (m_pTarget == NULL) return;

	m_TargetRenderer.DelTargetEffect(m_pTarget->uidEffect);
	m_pTarget->uidEffect = MUID::ZERO;
}

bool XTargetCollisionBox::IsIndicatorEffect()
{
	if (m_pTarget == NULL) return false;
	if (m_pTarget->uidEffect == MUID::ZERO) return false;

	return true;
}