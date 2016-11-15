#include "stdafx.h"
#include "XEffect.h"
#include "REffectInstance.h"
#include "XCharacter.h"
#include "XGameState.h"
#include "XWorld.h"
#include "RSceneManager.h"

#include "XResourceManager.h"

XEffect::XEffect()
:XModelEffect()
{
	m_bFollowTerrainAlign	= false;
	m_bFollowTerrainPos		= false;
	m_bFollowWaterPos		= false;

	m_uidOwer				= MUID::ZERO;

	m_matLocal.MakeIdentity();
}

XEffect::~XEffect()
{
}

bool XEffect::Create(MUID& uid, const wstring& str)
{
	m_UID = uid;

	if (!global.res)
		return false;

	REffectInstanceSceneNode* pEffectInstance = global.res->CreateEffectWithBGCache(str);
	if (NULL == pEffectInstance)
		return false;

	if (global.res->IsUsingEffectCacheLog())
		global.res->LogEffectCache(str);

	m_pEffect = pEffectInstance;
	m_strEffectFileName = str;

	return true;

//	REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
//	if (global.res && global.res->IsUsingEffectCacheLog())
//		global.res->LogEffectCache(str);
//	
// 	if( !pEffectInstance->Create( MLocale::ConvUTF16ToAnsi(str.c_str()) ) )
// 	{
//#ifdef _DEBUG
// 		mlog("CreateEffect 실패(%s)\n", MLocale::ConvUTF16ToAnsi(str.c_str()));
//#endif
//		SAFE_DELETE(pEffectInstance);
// 		return false;
// 	}
//
//	m_pEffect = pEffectInstance;
//	m_strEffectFileName = str;
//
//	return true;
}

void XEffect::UpdateXEffect( float fDelta )
{
	UpdateActorNode();

	if(m_bFollowTerrainAlign || m_bFollowTerrainPos || m_bFollowWaterPos)
		UpdateFollowTerrainAndWater();

	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
}

void XEffect::UpdateFollowTerrainAndWater()
{
	vec3 vWorldPosition = m_pEffect->GetPosition();
	vec3 vWorldDirection = m_pEffect->GetDirection();
	vec3 vWorldUp = m_pEffect->GetUp();

	if(m_uidOwer != MUID::ZERO)
	{
		XObject* pObject = gg.omgr->FindObject(m_uidOwer);
		if(pObject == NULL)
		{
			// 없어졌다.
			m_bDelMe = true;
			return;
		}

		vWorldPosition = pObject->GetPosition();
		vWorldDirection = pObject->GetDirection();
		vWorldUp = pObject->GetUpvector();

		pObject->GetActor()->GetActorNodePos(m_strBoneName.c_str(), vWorldPosition, E_TS_WORLD);

		// 로컬 셋팅
		MMatrix matTemp;
		matTemp.SetLocalMatrix(vWorldPosition, vWorldDirection, vWorldUp);
		matTemp = m_matLocal * matTemp;
		vWorldPosition	= vec3(matTemp._41, matTemp._42, matTemp._43);
		vWorldDirection = vec3(matTemp._21, matTemp._22, matTemp._23);
		vWorldUp		= vec3(matTemp._31, matTemp._32, matTemp._33);
	}

	// 물 검사
	if(m_bFollowWaterPos)
	{
		vec3 vOutWater;
		vec3 vWaterTo	= vWorldPosition;
		vWaterTo.z		+= 10000.0f;
		if(gg.currentgamestate->GetWorld()->PickCollision(vWorldPosition, vWaterTo, &vOutWater, NULL, CollisionAttribute::WATER))
		{
			float fHeight = vOutWater.z - vWorldPosition.z;
			if((m_fFollowWaterHeightMin == 0.0f && m_fFollowWaterHeightMax == 0.0f) ||
				(fHeight >= m_fFollowWaterHeightMin && fHeight <= m_fFollowWaterHeightMax))
			{
				RMatrix matWater;
				matWater.SetLocalMatrix(vOutWater, vWorldDirection, vWorldUp);
				m_pEffect->SetTransform(matWater);
			}
			else
			{
				// 높이가 해당 조건에 안맞으면 삭제...
				m_bDelMe = true;
			}

			return;
		}
	}

	// 지형 검사
	RMatrix matTerrain = gg.currentgamestate->GetWorld()->GetTerrainMatrix(vWorldPosition, vWorldDirection, vWorldUp, m_bFollowTerrainAlign);
	m_pEffect->SetTransform(matTerrain);
	m_pEffect->SetScale(m_vScale);
}

void XEffect::SetOwner( MUID uidOwner )
{
	m_uidOwer = uidOwner;
}

void XEffect::SetFollowTerrainAlign( bool bTerrainAlign, bool bTerrainPos )
{
	m_bFollowTerrainAlign	= bTerrainAlign;
	m_bFollowTerrainPos		= bTerrainPos;
}

void XEffect::UpdateActorNode()
{
	XObject* pObject = gg.omgr->FindActor(m_uidOwer);
	if(pObject == NULL)
		return;

	if(pObject->GetActor())
		pObject->GetActor()->AddActiveActorNode(m_strBoneName);
}

void XEffect::SetFollowWaterPos( bool bWaterPos )
{
	m_bFollowWaterPos = bWaterPos;
}

void XEffect::CheckOwerChangeMotion( MUID uidOwner )
{
	if(m_bDelMe)
		return;

	if(uidOwner != m_uidOwer)
		return;

	if(m_bCancelEnabled)
		m_bDelMe = true;
}