#include "stdafx.h"
#include "WS_EffectManager.h"
#include "REffectSceneNode.h"

WS_EffectManager::WS_EffectManager()
{

}

WS_EffectManager::~WS_EffectManager()
{
	Destroy();
}

void WS_EffectManager::Destroy()
{
	Clear();
}

void WS_EffectManager::Clear()
{
	for( vector<WS_Effect*>::iterator itor = m_vEffectList.begin(); itor != m_vEffectList.end(); ++itor)
		SAFE_DELETE( *itor );

	m_vEffectList.clear();
}

void WS_EffectManager::OnUpdate(float fDelta)
{
	// 이펙트 업데이트
	vector<WS_Effect*>::iterator itor = m_vEffectList.begin();
	while(itor != m_vEffectList.end())
	{
		WS_Effect* pEffect = (*itor);

		// 이펙트 업데이트
		pEffect->UpdateXEffect(fDelta);

		bool bDelNow = false;
		if(pEffect->IsEffectLiveTime() == false)
		{
			if (pEffect->IsEffectLoadingComplete() == false || pEffect->GetImmediatlyDelete())
			{
				bDelNow = true;
			}
			else
			{
				pEffect->Stop();
			}
		}

		// 지워진 이펙트 검사
		if (pEffect->IsEffectLive() == false || bDelNow)
		{
			SAFE_DELETE(pEffect);
			itor = m_vEffectList.erase(itor);
			continue;
		}

		itor++;
	}

	effectPostEffect.UpdatePostEffect(fDelta);
}

WS_Effect* WS_EffectManager::CreateWSEffect(const string& szEffectName)
{
	if (szEffectName.empty())	return NULL;
	//if (gg.game == NULL)	return NULL;

	WS_Effect* pNewObject = new WS_Effect();
	if (pNewObject)
	{
		MUID uidNew = GenerateLocalUID();
		if(!pNewObject->Create(uidNew, szEffectName ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}
	return NULL;
}

void WS_EffectManager::AttachWSEffect( WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bIgnoreDir )
{
	REngine::GetSceneManager().AttachEffect( pEffect->GetEffect(), pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );
}

MUID WS_EffectManager::AddWSEffect( const string& szEffectName, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	WS_Effect* pEffect = CreateWSEffect(szEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	AddWSEffect(pEffect, pSceneNode, pActorNode, _rBaseFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);

	return pEffect->GetMUID();
}

MUID WS_EffectManager::AddWSEffect( const string& szEffectName, RSceneNode* pSceneNode, RActorNode* pActorNode, const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bTerrainAlign /*= false*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	RMatrix matFrame = RMatrix::IDENTITY;
	if(bTerrainAlign)
	{
		matFrame = GetTerrainAlignMatrix(vPos, vDir);
	}
	else
	{
		matFrame.SetLocalMatrix(vPos, vDir, vUp);
	}

	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugSphere(matFrame, 10, 5.0f, D3DCOLOR_XRGB(0, 255, 255));
	return AddWSEffect(szEffectName, pSceneNode, pActorNode, matFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
}

MUID WS_EffectManager::AddWSEffect( const string& szEffectName, const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bTerrainAlign /*= false*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	RMatrix matFrame = RMatrix::IDENTITY;
	if(bTerrainAlign)
	{
		matFrame = GetTerrainAlignMatrix(vPos, vDir);
	}
	else
	{
		matFrame.SetLocalMatrix(vPos, vDir, vUp);
	}

	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugSphere(matFrame, 10, 5.0f, D3DCOLOR_XRGB(0, 255, 255));
	return AddWSEffect(szEffectName, NULL, NULL, matFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
}

void WS_EffectManager::AddWSEffect( WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	if(pEffect == NULL)
		return;

	pEffect->SetImmediatlyDelete(bImmediatlyDelete);		// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	pEffect->SetDurationTime(fDurationTime);
	pEffect->SetLastFramePause(bLastFramePause);
	//	pEffect->SetIgnoreParentActorNodeDir(bIgnoreDir);		// 부모의 방향을 무시를 하기 위한 셋팅 false는 무시 안함, true 무시 함
	pEffect->SetUseVisibility(fStartVisibilityTime, fEndVisibilityTime);

	//_ASSERT( global.smgr );

	// 이펙트의 스케일 값을 자유롭게 하기 위한 설정 셋팅
	SetScaleInit(pEffect, pSceneNode, pActorNode);
	AttachWSEffect( pEffect, pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );
}

//자동으로 지워지지 않는(loop, bLastFramePause) 이펙트 삭제용.
bool WS_EffectManager::DeleteWSEffect( MUID & uid, bool bImmediatlyDelete /*= false*/ )
{
	for( vector<WS_Effect*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		WS_Effect* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			bool bImmediatly = pEffect->GetImmediatlyDelete();

			// 기본은 false인데... 외부에서 true값이 들어오면 true값으로 처리
			if(bImmediatlyDelete == true)
				bImmediatly = true;

			if (pEffect->IsEffectLoadingComplete() == false || bImmediatly)
			{
				SAFE_DELETE(pEffect);
				m_vEffectList.erase(itor);
			}
			else
			{
				pEffect->Stop();
			}

			return true;
		}
	}
	return false;
}

WS_Effect* WS_EffectManager::GetWSEffect(MUID& uid)
{
	for( vector<WS_Effect*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		WS_Effect* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			return pEffect;
		}
	}
	return NULL;	
}

bool WS_EffectManager::SetScaleWSEffect(MUID& uid, bool bAutoScale, vec3 scale)
{
	for( vector<WS_Effect*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		WS_Effect* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			vec3 vResultScale = scale;
			if(bAutoScale == false)
			{
				vResultScale = pEffect->GetInverseScale() * scale;
			}

			pEffect->SetScale(vResultScale);
			return true;
		}
	}

	return false;	
}

rs3::RMatrix WS_EffectManager::GetTerrainAlignMatrix( const vec3& vPos, const vec3& vDir )
{
	vec3 vNewPos = vPos;
	vec3 vAlign(0.0f, 0.0f, 1.0f);

	vNewPos.z = 0.0f;

	// 매트릭스 계산
	RMatrix matWorld;
	matWorld.SetLocalMatrix(vNewPos, vDir, vAlign);

	return matWorld;
}

bool WS_EffectManager::SetTransform( MUID& uid, vec3& vPos, vec3& vDir, RMatrix& matResult, bool bTerrainAlign /*= false*/ )
{
	WS_Effect * pEffect = GetWSEffect(uid);
	if(pEffect)
	{
		RMatrix matTemp = RMatrix::IDENTITY;
		matResult = RMatrix::IDENTITY;

		if(bTerrainAlign)
		{
			matTemp = GetTerrainAlignMatrix(vPos, vDir);
		}
		else
		{
			matTemp.SetLocalMatrix(vPos, vDir, vec3(0.0f, 0.0f, 1.0f));
		}

		if(pEffect->GetEffect())
		{
			matResult.SetScale(pEffect->GetScale());
			matResult *= matTemp;
			pEffect->GetEffect()->SetTransform(matResult);
		}

		return true;
	}

	return false;
}

void WS_EffectManager::SetScaleInit( WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode )
{
	bool bSet = true;
	RMatrix matWT;
	vec3 vInverseScale = vec3(1.0f, 1.0f, 1.0f);

	if(pActorNode)
	{
		matWT = pActorNode->GetWorldTransform();
	}
	else if(pSceneNode)
	{
		matWT = pSceneNode->GetWorldTransform();
	}
	else
	{
		bSet = false;
	}

	if(bSet)
	{
		vec3 vScalse(1.0f, 1.0f, 1.0f);
		vec3 vTans(0.0f, 0.0f, 0.0f);
		MQuat qRotation;

		matWT.Decompose(vScalse, vTans, qRotation);

		vInverseScale.x = 1.0f / vScalse.x;
		vInverseScale.y = 1.0f / vScalse.y;
		vInverseScale.z = 1.0f / vScalse.z;

		pEffect->SetInverseScale(vInverseScale);
	}
}

MUID WS_EffectManager::GenerateLocalUID()
{
	static MUID localUID(100, 0);
	
	localUID.High++;
	localUID.Low = 0;

	return localUID;
}

void WS_EffectManager::SetPostEffect( BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType )
{
	effectPostEffect.StartPostEffect(byPostEffectType, pActor, fPlayTime, fScatTime, fSpeed, nScarFactorType);
}

void WS_EffectManager::ClearPostEffect()
{
	effectPostEffect.Clear();
}