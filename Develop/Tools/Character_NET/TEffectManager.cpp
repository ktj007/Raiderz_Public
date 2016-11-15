#include "stdafx.h"
#include "TEffectManager.h"
#include "REffectSceneNode.h"

TEffectManager::TEffectManager()
{
	SetEffectFileList();
}

TEffectManager::~TEffectManager()
{
	Destroy();
}

void TEffectManager::Destroy()
{
	Clear();

	StopWepaonEffect();
}

void TEffectManager::SetEffectFileList()
{
	m_vecEffectFileList.clear();

	vector<string> vecAddString;
	vecAddString.push_back("effect.scene.xml");
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	TFileNameSampling::GetFileList(PATH_SFX_TOOL, m_vecEffectFileList, &vecAddString, NULL, FA_FILE);
}

void TEffectManager::GetEffectPureFileNameList( vector<string>& vecFileList )
{
	for(vector<FILE_LIST>::iterator it = m_vecEffectFileList.begin(); it != m_vecEffectFileList.end(); ++it)
	{
		vecFileList.push_back(it->strPureFileName);
	}
}

void TEffectManager::Clear()
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin(); itor != m_vEffectList.end(); ++itor)
		SAFE_DELETE( *itor );

	m_vEffectList.clear();
}

void TEffectManager::OnUpdate(float fDelta)
{
	Update(m_vEffectList, fDelta);
	Update(m_vWeaponEffectList, fDelta);

	effectPostEffect.UpdatePostEffect(fDelta);
}

void TEffectManager::Update( vector<XEffectBase*>& vecUpdateEffect, float fDelta )
{
	// 이펙트 업데이트
	vector<XEffectBase*>::iterator itor = vecUpdateEffect.begin();
	while(itor != vecUpdateEffect.end())
	{
		XEffectBase* pEffect = (*itor);

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
			itor = vecUpdateEffect.erase(itor);
			continue;
		}

		itor++;
	}
}

TEffect* TEffectManager::CreateTEffect(const string& szEffectName)
{
	if(g_pMainApp->m_bShowEffect == false)
		return NULL;

	if (szEffectName.empty())	return NULL;
	//if (gg.game == NULL)	return NULL;

	TEffect* pNewObject = new TEffect();
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

void TEffectManager::AttachTEffect( TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bIgnoreDir )
{
	g_pMainApp->GetSceneMgr()->AttachEffect( pEffect->GetEffect(), pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );
}

MUID TEffectManager::AddTEffect( const string& szEffectName, TCharacter* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, bool bWaterPos /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	TEffect* pEffect = CreateTEffect(szEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	AddTEffect(pEffect, pSceneNode, pActorNode, _rBaseFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, bWaterPos, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);

	return pEffect->GetMUID();
}

MUID TEffectManager::AddTEffect( const string& szEffectName, TCharacter* pSceneNode, RActorNode* pActorNode, const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bTerrainAlign /*= false*/, bool bTerrainPos /*= false*/, bool bWaterPos /*= false*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	RMatrix matFrame = RMatrix::IDENTITY;
	if(bTerrainAlign)
	{
		matFrame = GetTerrainMatrix(vPos, vDir, true);
	}
	else if(bTerrainPos)
	{
		matFrame = GetTerrainMatrix(vPos, vDir, false);
	}
	else
	{
		matFrame.SetLocalMatrix(vPos, vDir, vUp);
	}

	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugSphere(matFrame, 10, 5.0f, D3DCOLOR_XRGB(0, 255, 255));
	return AddTEffect(szEffectName, pSceneNode, pActorNode, matFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, bWaterPos, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
}

MUID TEffectManager::AddTEffect( const string& szEffectName, const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bTerrainAlign /*= false*/, bool bTerrainPos /*= false*/, bool bWaterPos /*= false*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	RMatrix matFrame = RMatrix::IDENTITY;
	if(bTerrainAlign)
	{
		matFrame = GetTerrainMatrix(vPos, vDir, true);
	}
	else if(bTerrainPos)
	{
		matFrame = GetTerrainMatrix(vPos, vDir, false);
	}
	else
	{
		matFrame.SetLocalMatrix(vPos, vDir, vUp);
	}

	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugSphere(matFrame, 10, 5.0f, D3DCOLOR_XRGB(0, 255, 255));
	return AddTEffect(szEffectName, NULL, NULL, matFrame, bLastFramePause, bIgnoreDir, bImmediatlyDelete, bWaterPos, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
}

void TEffectManager::AddTEffect( TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, bool bWaterPos /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	if(pEffect == NULL)
		return;

	pEffect->SetImmediatlyDelete(bImmediatlyDelete);		// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	pEffect->SetDurationTime(fDurationTime);
	pEffect->SetLastFramePause(bLastFramePause);
	//	pEffect->SetIgnoreParentActorNodeDir(bIgnoreDir);		// 부모의 방향을 무시를 하기 위한 셋팅 false는 무시 안함, true 무시 함
	pEffect->SetUseVisibility(fStartVisibilityTime, fEndVisibilityTime);
	
	if(pSceneNode || pActorNode)
	{
		if(pSceneNode)
			pEffect->SetActor(pSceneNode);

		pEffect->SetFollowWaterPos(bWaterPos);

		if(pActorNode)
			pEffect->SetBoneName((tstring)pActorNode->GetNodeName());
	}

	//_ASSERT( global.smgr );

	// 이펙트의 스케일 값을 자유롭게 하기 위한 설정 셋팅
	SetScaleInit(pEffect, pSceneNode, pActorNode);
	AttachTEffect( pEffect, pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );

	if(pSceneNode && pActorNode)
	{
		pSceneNode->AddActiveActorNode(pActorNode->m_strName);
	}
}

//자동으로 지워지지 않는(loop, bLastFramePause) 이펙트 삭제용.
bool TEffectManager::DeleteTEffect( MUID & uid, bool bImmediatlyDelete /*= false*/ )
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffect = (*itor);
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

TEffect* TEffectManager::GetTEffect(MUID& uid)
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			return static_cast<TEffect*>(pEffect);
		}
	}
	return NULL;	
}

bool TEffectManager::SetScaleTEffect( MUID& uid, bool bAutoScale, vec3 scale )
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		TEffect* pEffect = static_cast<TEffect*>(*itor);
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

rs3::RMatrix TEffectManager::GetTerrainMatrix( const vec3& vPos, vec3 vDir, bool bAlign )
{
	vec3 vNewPos = vPos;
	vec3 vAlign = vec3::AXISZ;

	vNewPos.z = 0.0f;

	vDir.z = 0.0f;
	vDir.Normalize();

	// 매트릭스 계산
	RMatrix matWorld;
	matWorld.SetLocalMatrix(vNewPos, vDir, vAlign);

	return matWorld;
}

bool TEffectManager::SetTransform( MUID& uid, vec3& vPos, vec3& vDir, RMatrix& matResult, bool bTerrainAlign /*= false*/, bool bTerrainPos /*= false*/ )
{
	TEffect * pEffect = GetTEffect(uid);
	if(pEffect)
	{
		RMatrix matTemp = RMatrix::IDENTITY;
		matResult = RMatrix::IDENTITY;

		if(bTerrainAlign)
		{
			matTemp = GetTerrainMatrix(vPos, vDir, true);
		}
		else if(bTerrainPos)
		{
			matTemp = GetTerrainMatrix(vPos, vDir, false);
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

void TEffectManager::SetScaleInit( TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode )
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

MUID TEffectManager::GenerateLocalUID()
{
	static MUID localUID(100, 0);
	
	localUID.High++;
	localUID.Low = 0;

	return localUID;
}

void TEffectManager::SetPostEffect( BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType )
{
	effectPostEffect.StartPostEffect(byPostEffectType, pActor, fPlayTime, fScatTime, fSpeed, nScarFactorType, nBlurType);
}

MUID TEffectManager::SetSelfIlluminationEffect( string& strActorNodeName, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fDurationTime, float fMin, float fMax, bool bLoop )
{
	MUID uidEmpty;
	if(CheckDuplicationSelfIlluminationEffect(uidEmpty, strActorNodeName))
		return MUID::ZERO;

	TSelfIlluminationEffect* pEffect = CreateXSelfIlluminationEffect(fStartTime, fStartDurationTime, fEndTime, fEndDurationTime, fMin, fMax);
	if (pEffect == NULL )
		return MUID::ZERO;

	pEffect->SetDurationTime(fDurationTime);
	pEffect->SetBoneName(strActorNodeName);
	pEffect->SetLoop(bLoop);

	return pEffect->GetMUID();
}

TSelfIlluminationEffect* TEffectManager::CreateXSelfIlluminationEffect( float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax )
{
	TSelfIlluminationEffect* pNewObject = new TSelfIlluminationEffect();
	if (pNewObject)
	{
		if(!pNewObject->Create(fStartTime, fStartDurationTime, fEndTime, fEndDurationTime, fMin, fMax ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}
	return NULL;
}

bool TEffectManager::CheckDuplicationSelfIlluminationEffect( MUID& uidOwner, string& strActorNodeName )
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		if((*itor)->GetType() == ET_GLOW)
		{
			XGlowEffect* pEffect = static_cast<XGlowEffect*>(*itor);
			return pEffect->IsDuplicationEffect(uidOwner, strActorNodeName);
		}
	}

	return false;
}

void TEffectManager::StopAnimationEffect()
{
	effectPostEffect.Clear();

	// 이펙트 업데이트
	vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
	while(itor != m_vEffectList.end())
	{
		XEffectBase* pEffect = (*itor);

		// 이펙트 업데이트
		if(pEffect->IsEffectLoop() && pEffect->GetType() != ET_WEAPON)
			pEffect->Stop();

		itor++;
	}
}

MUID TEffectManager::AddXDecalEffect( string strEffectName, string strBoneName, vec3 vPos, bool bFollow, BYTE byDeclaEffectType, const DWORD dwColor, const float fSize )
{
	TDecalEffect* pNewDecal = new TDecalEffect();
	if(pNewDecal)
	{
		MUID uidNew = GenerateLocalUID();
		pNewDecal->Create(uidNew, strEffectName, DET_PROJECTION, vPos, fSize);
		pNewDecal->SetBoneName(strBoneName);
		pNewDecal->SetFollow(bFollow);

		m_vEffectList.push_back(pNewDecal);
		return uidNew;
	}

	return MUID::ZERO;
}

TDecalEffect* TEffectManager::GetTDecalEffect( MUID& uid )
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			return static_cast<TDecalEffect*>(pEffect);
		}
	}

	return NULL;	
}

XEffectBase* TEffectManager::GetEffect( MUID& uid )
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffect = (*itor);
		if( pEffect->GetMUID() == uid)
		{
			return pEffect;
		}
	}

	return NULL;
}

TNoiseFadeEffect* TEffectManager::CreateNoiseFadeEffect( float fStartDurationTime, float fEndDurationTime )
{
	TNoiseFadeEffect* pNewObject = new TNoiseFadeEffect();
	if (pNewObject)
	{
		MUID uid = MUID::ZERO;
		if(!pNewObject->Create(uid, fStartDurationTime, fEndDurationTime ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}
	return NULL;
}

void TEffectManager::SetNoiseEffect( vector<string>& vecMeshNodeName, float fStartDurationTime, float fEndDurationTime, DWORD dwColor1, DWORD dwColor2, float fSize1, float fSize2, bool bInverse )
{
	TNoiseFadeEffect* pEffect = CreateNoiseFadeEffect(fStartDurationTime, fEndDurationTime);

	pEffect->SetMeshNodeName(vecMeshNodeName);
	pEffect->SetFadeEffectColor(dwColor1, dwColor2, fSize1, fSize2);
	pEffect->SetInverse(bInverse);

	return;
}

void TEffectManager::SetWeaponEffect( TCharacter* pSceneNode, const string& szEffectName, int nIndex )
{
	if(CheckWeaponEffectDummy(pSceneNode, nIndex) == false)
		return;

	TWeaponEffect* pEffect = CreateWeaponEffect(szEffectName, nIndex);
	if(pEffect == NULL)
		return;

	pEffect->SetActor(pSceneNode);

	RMatrix _rBaseFrame = RMatrix::IDENTITY;
	g_pMainApp->GetSceneMgr()->AttachEffect( pEffect->GetEffect(), NULL, NULL, _rBaseFrame, false );
}

void TEffectManager::SetWeaponEffect( TCharacter* pSceneNode, const string& szEffectName, const string& szBoneName )
{
	if(CheckWeaponEffectDummy(pSceneNode, szBoneName) == false)
		return;

	TWeaponEffect* pEffect = CreateWeaponEffect(szEffectName, 0);
	if(pEffect == NULL)
		return;

	pEffect->SetActor(pSceneNode);
	pEffect->SetBoneName((string)szBoneName);

	RMatrix _rBaseFrame = RMatrix::IDENTITY;
	g_pMainApp->GetSceneMgr()->AttachEffect( pEffect->GetEffect(), NULL, NULL, _rBaseFrame, false );
}

TWeaponEffect* TEffectManager::CreateWeaponEffect( const string& szEffectName, int nIndex )
{
	TWeaponEffect* pNewObject = new TWeaponEffect();
	if (pNewObject)
	{
		MUID uidNew = GenerateLocalUID();
		if(!pNewObject->Create(uidNew, szEffectName, nIndex ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vWeaponEffectList.push_back(pNewObject);

		return pNewObject;
	}
	return NULL;
}

void TEffectManager::StopWepaonEffect()
{
	for( vector<XEffectBase*>::iterator itor = m_vWeaponEffectList.begin(); itor != m_vWeaponEffectList.end(); ++itor)
		SAFE_DELETE( *itor );

	m_vWeaponEffectList.clear();
}

bool TEffectManager::CheckWeaponEffectDummy( TCharacter* pSceneNode, int nIndex )
{
	char chEffectName[256] = {0,};
	sprintf_s(chEffectName, "%s%02d", WEAPON_EFFECT_DUMMY_NAME_TOOL, nIndex);

	return CheckWeaponEffectDummy(pSceneNode, chEffectName);
}

bool TEffectManager::CheckWeaponEffectDummy( TCharacter* pSceneNode, string strBoneName )
{
	MMatrix mat;
	RActorNode* pActorNode = pSceneNode->GetActorNodeTransform(strBoneName.c_str(), mat, E_TS_LOCAL);
	if(pActorNode == NULL)
		return false;

	return true;
}