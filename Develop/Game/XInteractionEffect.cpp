#include "StdAfx.h"
#include "XInteractionEffect.h"
#include "XModuleCollision.h"
#include "XMyPlayer.h"
#include "XGame.h"
#include "XCharacter.h"
#include "RMesh.h"
#include "RDebugPassRenderer.h"
#include "XWorld.h"
#include "RSceneManager.h"
#include "XInteractionHelper.h"
#include "XEffectInvoker.h"
#include "XEffectManager.h"

#ifdef _MADDUCK2
	#include "RDebugPassRenderer.h"
#endif

unsigned long int TargetEffectColor[] = {
	0xFFFF0000,		//	INTERACTION_ENEMY_NPC = 0,
	0xFFFF0000,		//	INTERACTION_ENEMY_NETPLAYER,
	0xFF800080,		//	INTERACTION_PARTY_MEMBER,
	0xFF0000FF,		//	INTERACTION_INTERACTIVE_NPC,
	0xFF800080,		//	INTERACTION_INTERACTIVE_OBJECT,
	0xFF808080,		//	INTERACTION_INTERACTIVE_DISABLE,
	0xFFFFFFFF		//	INTERACTION_NONE
};					

XInteractionEffect::XInteractionEffect( XObject* pObject, RRENDERPASS eRenderPass /*= RRP_NORMAL*/ )
: RSceneNode(eRenderPass)
, m_pOwner(pObject)
, m_eInteractionEffectType(INTERACTION_NONE)
{
	m_aabb.SetInfinity();
	SetAutoUpdate(true);

	m_vecDecalEffectData.clear();
}

XInteractionEffect::~XInteractionEffect(void)
{
	m_pOwner = NULL;

	Destroy();
}

void XInteractionEffect::Init( wstring strEffectName, vec3 vPos, vec3 vDir, float fSize )
{
	if(m_vecDecalEffectData.empty())
	{
		EFFECT_POST_TRANSFORM postData;
		postData.vSize = vec3(fSize, fSize, XCONST::DECALEFFECT_DEPTH_VALUE);

		XEffectInvoker decalInvoker;
		decalInvoker.Invoke(m_pOwner, strEffectName, wstring(), wstring(), &m_vecDecalEffectData, NULL, &postData);
	}
}

void XInteractionEffect::Destroy()
{
	m_eInteractionEffectType = INTERACTION_NONE;

	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecDecalEffectData.begin();
		it != m_vecDecalEffectData.end(); ++it)
	{
		if(it->type == CET_MODEL ||
			it->type == CET_DECALEFFECT)
			global.emgr->DeleteXEffect(it->uidEffect);
	}

	m_vecDecalEffectData.clear();

	RemoveFromParent();
}

RBoundingBox XInteractionEffect::GetObjectAABB()
{
	return XInteractionHelper::GetObjectAABB(m_pOwner);
}

void XInteractionEffect::CreateInteractionEffect(INTERACTION_EFFECT_TYPE eType )
{
	SetInteractionEffectType(eType);

	RBoundingBox objAABB = GetObjectAABB();
	vec3 vCenter = GetInteractionCenter(objAABB);
	Init(L"decal_Interaction", vCenter, m_pOwner->GetDirection(), GetInteractionSize(objAABB));

	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecDecalEffectData.begin();
		it != m_vecDecalEffectData.end(); ++it)
	{
		if(it->type == CET_DECALEFFECT)
		{
			XDecalEffect* pDecal = global.emgr->GetXDecalEffect(it->uidEffect);
			if(pDecal)
				SetInteractionColor(pDecal);
		}
	}

	global.smgr->AddSceneNode(this);
}

void XInteractionEffect::OnUpdate()
{
	PFC_THISFUNC;

	if(INTERACTION_NONE == m_eInteractionEffectType) return;

	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecDecalEffectData.begin();
		it != m_vecDecalEffectData.end(); ++it)
	{
		if(it->type == CET_DECALEFFECT)
		{
			XDecalEffect* pDecal = global.emgr->GetXDecalEffect(it->uidEffect);
			if(pDecal)
			{
				// 위치 변경
				SetInteractionPos(pDecal);

				// 색깔 변경
				SetInteractionColor(pDecal);
			}
		}
	}
}

bool XInteractionEffect::IsEffect()
{
	if( m_eInteractionEffectType == INTERACTION_NONE ) return false;

	return true;
}

void XInteractionEffect::SetInteractionEffectType(INTERACTION_EFFECT_TYPE eType )
{
	m_eInteractionEffectType	= eType;
}

float XInteractionEffect::GetInteractionSize( RBoundingBox& rbb )
{
	return XInteractionHelper::GetInteractionSize(rbb);
}

vec3 XInteractionEffect::GetInteractionCenter( RBoundingBox& rbb )
{
	return XInteractionHelper::GetInteractionCenter(m_pOwner, rbb);
}

void XInteractionEffect::SetInteractionColor( XDecalEffect* pDecal )
{
	if(pDecal == NULL)
		return;

	DWORD dwFactor = TargetEffectColor[m_eInteractionEffectType];

	pDecal->SetDecalEffectColor(dwFactor);
}

void XInteractionEffect::SetInteractionPos( XDecalEffect* pDecal )
{
	if(pDecal == NULL)
		return;

	RBoundingBox objAABB = GetObjectAABB();
	vec3 vCenter = GetInteractionCenter(objAABB);

	if(MMath::Equals(pDecal->GetPosition(), vCenter, 5.0f) == false)
	{
		// 위치가 변경이 되었네...
		float fSize = GetInteractionSize(objAABB);

		pDecal->SetDecalEffectData(vCenter, fSize, fSize, XCONST::DECALEFFECT_DEPTH_VALUE);
	}
}