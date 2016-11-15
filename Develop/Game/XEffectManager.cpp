#include "stdafx.h"
#include "XEffectManager.h"
#include "XEffect.h"
#include "XBaseApplication.h"
#include "ReffectActor.h"
#include "REffectSceneNode.h"
#include "RSceneManager.h"
#include "XSystem.h"
#include "XGame.h"
#include "XWorld.h"
#include "XCharacter.h"
#include "RActorNodeConstraintController.h"

//#include "RDebugPassRenderer.h"
//#include "RCameraSceneNode.h"
XEffectManager::XEffectManager()
{
	effectWait.Init(this);

	m_pEffectDecalPolygon = NULL;
}

XEffectManager::~XEffectManager()
{
	Destroy();
}

void XEffectManager::Destroy()
{
	Clear();
}

void XEffectManager::Clear()
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin(); itor != m_vEffectList.end(); ++itor)
		SAFE_DELETE( *itor );

	m_vEffectList.clear();
	effectPreLoader.AllReleaseXEffectPreLoader();
	effectWait.Destroy();

	m_vecEffectReserve.clear();
	SAFE_DELETE(m_pEffectDecalPolygon);
}

void XEffectManager::OnUpdate(float fDelta)
{
	// 예약된 이펙트 처리
	UpdataXEffectReserve();

	// 이펙트 업데이트
	UpdateXEffect(fDelta);

	effectPostEffect.UpdatePostEffect(fDelta);

	// 탤런트 히트 업데이트
	m_HitEffectSelecter.Update(fDelta);

	// 외곽선 업데이트
	m_PEEdgeMgr.Update();
}

XEffect* XEffectManager::CreateXEffect(const wstring& szEffectName)
{
	if (szEffectName.empty())	return NULL;
	//if (gg.game == NULL)	return NULL;

	XEffect* pNewObject = new XEffect();
	if (pNewObject)
	{
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew, szEffectName ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		effectPreLoader.ReleaseXEffectPreLoader(szEffectName);
		return pNewObject;
	}
	return NULL;
}

void XEffectManager::AttachXEffect( XModelEffect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bIgnoreDir )
{
	_ASSERT( global.smgr );
	global.smgr->AttachEffect( pEffect->GetEffect(), pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );
}

MUID XEffectManager::AddXEffect( const wstring& szEffectName, XCharacter* pActor, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bLastFramePause /*= false*/, bool bIgnoreDir /*= false*/, bool bImmediatlyDelete /*= false*/, float fDurationTime /*= 0.0f*/, float fStartVisibilityTime /*= 0.0f*/, float fEndVisibilityTime /*= 0.0f*/ )
{
	//////////////////////////////////////////////////////////////////////////
	// 생성
	XEffect* pEffect = CreateXEffect(szEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	if(pActor && pActorNode)
		pActor->AddActiveActorNode(pActorNode);

	// 데이터 셋팅
	pEffect->SetImmediatlyDelete(bImmediatlyDelete);		// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	pEffect->SetDurationTime(fDurationTime);
	pEffect->SetLastFramePause(bLastFramePause);
	pEffect->SetUseVisibility(fStartVisibilityTime, fEndVisibilityTime);
	
	if(pActorNode)
		pEffect->SetBoneName(MLocale::ConvAnsiToUTF16(pActorNode->GetNodeName().c_str()));

	SetScaleInit(pEffect, pActor, pActorNode);			// 이펙트의 스케일 값을 자유롭게 하기 위한 설정 셋팅
	
	AttachXEffect( pEffect, pActor, pActorNode, _rBaseFrame, bIgnoreDir );
	return pEffect->GetMUID();
}

MUID XEffectManager::AddXEffect( EFFECT_CREATE_DATA& effectCreateData )
{
	//////////////////////////////////////////////////////////////////////////
	// 생성
	XEffect* pEffect = CreateXEffect(effectCreateData.strEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	// follow이 아니라면... 예약 한다. Update때 처리
	if(effectCreateData.bFollow == false)
	{
		AddXEffectReserve(pEffect, effectCreateData);
		return pEffect->GetMUID();
	}

	return AddXEffect(pEffect, effectCreateData);
}

MUID XEffectManager::AddXEffect( XEffect* pEffect, EFFECT_CREATE_DATA& effectCreateData )
{
	//////////////////////////////////////////////////////////////////////////
	// 자동 처리
	XCharacter * pActor = NULL;
	RActorNode * pActorNode = NULL;

	XObject* pObject = gg.omgr->FindObject(effectCreateData.uidActor);
	if(pObject)
		pActor = pObject->GetActor();
	if(pActor && effectCreateData.strBoneName.empty() == false)
		pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(effectCreateData.strBoneName.c_str()).c_str());

	//////////////////////////////////////////////////////////////////////////
	// 기본 셋팅
	pEffect->SetImmediatlyDelete(effectCreateData.bImmediatlyDelete);		// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	pEffect->SetDurationTime(effectCreateData.fDurationTime);
	pEffect->SetLastFramePause(effectCreateData.bLastFramePause);
	pEffect->SetUseVisibility(effectCreateData.fStartVisibilityTime, effectCreateData.fEndVisibilityTime);
	pEffect->SetOwner(effectCreateData.uidActor);
	pEffect->SetBoneName(effectCreateData.strBoneName);
	pEffect->SetCancelEnabled(effectCreateData.bCancelEnabled);

	//////////////////////////////////////////////////////////////////////////
	// 이펙트 위치 설정
	RMatrix matFrame = RMatrix::IDENTITY;

	if(effectCreateData.bFollow)
	{
		pEffect->SetFollowTerrainAlign(effectCreateData.bTerrainAlign, effectCreateData.bTerrainPos);				// 지형에 붙어 다니기...
		pEffect->SetFollowWaterPos(effectCreateData.bWaterPos);
		pEffect->SetFollowWaterHeight(effectCreateData.fWaterHeightMin, effectCreateData.fWaterHeightMax);

		if(effectCreateData.matPostMatrix != MMatrix::IDENTITY)
		{
			// 이 매트릭스를 local로 바꾸어야 한다.
			MMatrix matLocal = effectCreateData.matPostMatrix;
			MMatrix matActorWorld;
			MMatrix matWorldInverse;
			matActorWorld = pActor->GetWorldTransform();
			pActor->GetActorNodeTransform(effectCreateData.strBoneName.c_str(), matActorWorld, E_TS_WORLD);

			matActorWorld.GetInverse(&matWorldInverse);
			matLocal *= matWorldInverse;

			matFrame = matLocal;
		}

		if(effectCreateData.vLocal != vec3::ZERO)
		{
			MMatrix matLocal;
			matLocal.SetTranslationMatrix(effectCreateData.vLocal);

			matFrame *= matLocal;
		}

		if(pActor && pActorNode)
			pActor->AddActiveActorNode(effectCreateData.strBoneName);

		if(effectCreateData.bTerrainAlign ||
			effectCreateData.bTerrainPos ||
			effectCreateData.bWaterPos)
		{
			pActor = NULL;
			pActorNode = NULL;

			pEffect->SetLocalMatrix(matFrame);
			matFrame.MakeIdentity();
		}
	}
	else
	{
		MMatrix matLocal;
		matLocal.SetTranslationMatrix(effectCreateData.vLocal);

		vec3 vPos(vec3::ZERO);
		vec3 vDir(vec3::AXISY);
		vec3 vUp(vec3::AXISZ);

		if(pActor)
		{
			vPos = pActor->GetWorldPosition();
			vDir = pActor->GetWorldDirection();				// 엔진에서 이부분을 파낸다고 합니다. 이때 전체적으로 작업합시다.

			pActor->GetActorNodePos(effectCreateData.strBoneName.c_str(), vPos, E_TS_WORLD);
		}

		// 매트릭스는 world 이어야 한다.
		if(effectCreateData.matPostMatrix != MMatrix::IDENTITY)
		{
			vPos = vec3(effectCreateData.matPostMatrix._41, effectCreateData.matPostMatrix._42, effectCreateData.matPostMatrix._43);
			vDir = vec3(effectCreateData.matPostMatrix._21, effectCreateData.matPostMatrix._22, effectCreateData.matPostMatrix._23);
			vUp = vec3(effectCreateData.matPostMatrix._31, effectCreateData.matPostMatrix._32, effectCreateData.matPostMatrix._33);
		}

		// 로컬 셋팅
		MMatrix matTemp;
		matTemp.SetLocalMatrix(vPos, vDir, vUp);
		matTemp = matLocal * matTemp;
		vPos = vec3(matTemp._41, matTemp._42, matTemp._43);
		vDir = vec3(matTemp._21, matTemp._22, matTemp._23);
		vUp = vec3(matTemp._31, matTemp._32, matTemp._33);

		vec3 vOutWater;
		vec3 vWaterTo	= vPos;
		vWaterTo.z		+= 10000.0f;
		// 지형
		if(effectCreateData.bTerrainAlign)
		{
			matFrame = gg.currentgamestate->GetWorld()->GetTerrainMatrix(vPos, vDir, vUp, true);
		}
		else if(effectCreateData.bTerrainPos)
		{
			matFrame = gg.currentgamestate->GetWorld()->GetTerrainMatrix(vPos, vDir, vUp, false);
		}
		else if(effectCreateData.bWaterPos && 
			gg.currentgamestate->GetWorld()->PickCollision(vPos, vWaterTo, &vOutWater, NULL, CollisionAttribute::WATER))
		{
			matFrame.SetLocalMatrix(vOutWater, vDir, vUp);
		}
		else
		{
			matFrame.SetLocalMatrix(vPos, vDir, vUp);
		}

		pActor = NULL;
		pActorNode = NULL;
	}

	AttachXEffect( pEffect, pActor, pActorNode, matFrame, effectCreateData.bIgnoreDir );

	//////////////////////////////////////////////////////////////////////////
	// 스케일 적용(마지막에 해야 한다.)
	SetScaleInit(pEffect, pActor, pActorNode);			// 이펙트의 스케일 값을 자유롭게 하기 위한 설정 셋팅
	SetScaleXEffect(pEffect, effectCreateData.bAutoScale, effectCreateData.vScale);

	return pEffect->GetMUID();
}

//자동으로 지워지지 않는(loop, bLastFramePause) 이펙트 삭제용.
bool XEffectManager::DeleteXEffect( MUID & uid, bool bImmediatlyDelete /*= false*/ )
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

			// 예약된것도 지우자.
			DelXEffectReserve(pEffect->GetMUID());

			if (pEffect->IsEffectLoadingComplete() == false || bImmediatly)
			{
				int xcount = m_vEffectList.size();
				MUID uid = pEffect->GetMUID();
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

XEffect* XEffectManager::GetXEffect(MUID& uid)
{
	XEffectBase* pEffectBase = GetEffect(uid);

	if(pEffectBase &&
		pEffectBase->GetType() == ET_MODEL)
		return static_cast<XEffect*>(pEffectBase);

	return NULL;	
}

bool XEffectManager::SetScaleXEffect(MUID& uid, bool bAutoScale, vec3 scale)
{
	for( vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
		itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffect = (*itor);
		if( pEffect->GetMUID() == uid &&
			pEffect->GetType() == ET_MODEL)
		{
			return SetScaleXEffect(static_cast<XEffect*>(pEffect), bAutoScale, scale);
		}
	}

	return false;	
}

bool XEffectManager::SetScaleXEffect( XEffect* pEffect, bool bAutoScale, vec3 scale )
{
	if(pEffect == NULL)
		return false;

	vec3 vResultScale = scale;
	if(bAutoScale == false)
	{
		vResultScale = pEffect->GetInverseScale() * scale;
	}

	pEffect->SetScale(vResultScale);
	return true;
}

bool XEffectManager::SetTransform( MUID& uid, vec3& vPos, vec3& vDir, vec3& vUp, RMatrix& matOutResult, bool bTerrainAlign /*= false*/, bool bTerrainPos /*= false*/ )
{
	XEffect * pEffect = GetXEffect(uid);
	if(pEffect)
	{
		RMatrix matTemp = RMatrix::IDENTITY;
		matOutResult = RMatrix::IDENTITY;

		if(bTerrainAlign)
		{
			matTemp = gg.currentgamestate->GetWorld()->GetTerrainMatrix(vPos, vDir, vUp, true);
		}
		else if(bTerrainPos)
		{
			matTemp = gg.currentgamestate->GetWorld()->GetTerrainMatrix(vPos, vDir, vUp, false);
		}
		else
		{
			matTemp.SetLocalMatrix(vPos, vDir, vec3(0.0f, 0.0f, 1.0f));
		}

		if(pEffect->GetEffect())
		{
			matOutResult.SetScale(pEffect->GetScale());
			matOutResult *= matTemp;
			pEffect->GetEffect()->SetTransform(matOutResult);
		}

		return true;
	}

	return false;
}

void XEffectManager::SetScaleInit( XEffect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode )
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

void XEffectManager::PreLoaderXEffect( const wstring& strEffectName )
{
	effectPreLoader.AddXEffectPreLoader(strEffectName);
}

void XEffectManager::UpdateXEffect( float fDelta )
{
	vector<XEffectBase*>::iterator itor = m_vEffectList.begin();
	while(itor != m_vEffectList.end())
	{
		XEffectBase* pEffect = (*itor);

		// 기다리는 이펙트인가?
		if(effectWait.CheckWaiting(pEffect->GetMUID()))
		{
			itor++;
			continue;
		}

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
			DelXEffectReserve(pEffect->GetMUID());
			SAFE_DELETE(pEffect);
			itor = m_vEffectList.erase(itor);
			continue;
		}

		itor++;
	}
}

MUID XEffectManager::AddEffectWaiting( EFFECT_CREATE_DATA& effectWaitingData )
{
	XEffect* pEffect = CreateXEffect(effectWaitingData.strEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	effectWait.AddEffectWaiting(pEffect->GetMUID(), effectWaitingData, WET_MODEL);

	return pEffect->GetMUID();
}

void XEffectManager::ActorLoadingComplete( MUID uidObject )
{
	effectWait.ActorLoadingComplete(uidObject);
}

void XEffectManager::SetPostEffect( BYTE byPostEffectType, XObject* pObject, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType )
{
	effectPostEffect.StartPostEffect(byPostEffectType, pObject, fPlayTime, fScatTime, fSpeed, nScarFactorType, nBlurType);
}

MUID XEffectManager::AddXDecalEffect( EFFECT_CREATE_DATA& effectCreateData, BYTE byDeclaEffectType, const DWORD dwColor, const float fSizeX, const float fSizeY, const float fSizeZ )
{
	//////////////////////////////////////////////////////////////////////////
	// 생성
	vec3 vPos = effectCreateData.matPostMatrix.GetTranslation();

	XObject* pObject = gg.omgr->FindObject(effectCreateData.uidActor);
	XCharacter* pActor = NULL;
	RActorNode* pActorNode = NULL;

	if(pObject)
	{
		pActor = pObject->GetActor();
		pActorNode = NULL;

		if(pActor)
			pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(effectCreateData.strBoneName.c_str()).c_str());

		if(vPos == vec3::ZERO &&
			pActor)
		{
			vPos = pActor->GetWorldPosition();

			pActor->GetActorNodePos(effectCreateData.strBoneName.c_str(), vPos, E_TS_WORLD);
		}
	}

	XDecalEffect* pDecalEffect = CreateXDecalEffect(effectCreateData.strEffectName, byDeclaEffectType, vPos, fSizeX, fSizeY, fSizeZ);
	if (pDecalEffect == NULL )
		return MUID::ZERO;

	// 데이터 입력
	if(effectCreateData.bFollow)
	{
		pDecalEffect->SetOwner(effectCreateData.uidActor);
		pDecalEffect->SetBoneName(effectCreateData.strBoneName);
	}

	pDecalEffect->SetDurationTime(effectCreateData.fDurationTime);
	pDecalEffect->SetUseVisibility(effectCreateData.fStartVisibilityTime, effectCreateData.fEndVisibilityTime);
	pDecalEffect->SetDecalEffectColor(dwColor);
	pDecalEffect->SetCancelEnabled(effectCreateData.bCancelEnabled);

	AttachXDecalEffect(pDecalEffect, pActor, pActorNode);

	return pDecalEffect->GetMUID();
}

XDecalEffect* XEffectManager::CreateXDecalEffect( const wstring& szEffectName, BYTE byDeclaEffectType, const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ )
{
	if (szEffectName.empty())	return NULL;
	//if (gg.game == NULL)	return NULL;

	//////////////////////////////////////////////////////////////////////////
	// 데칼 폴리곤은 사용 할때 생성한다.
	if(byDeclaEffectType == DET_POLYGON)
	{
		if(m_pEffectDecalPolygon == NULL)
			m_pEffectDecalPolygon = new RDecalPolygonSceneNode;

		m_pEffectDecalPolygon->Add(MLocale::ConvUTF16ToAnsi(szEffectName.c_str()), vPos, fSizeX, fSizeY, fSizeZ);

		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 데칼 프로젝션
	XDecalEffect* pNewObject = new XDecalEffect();
	if (pNewObject)
	{
		// 데칼 이펙트 생성
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew, szEffectName, byDeclaEffectType, vPos, fSizeX, fSizeY, fSizeZ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		//effectPreLoader.ReleaseXEffectPreLoader(szEffectName);
		return pNewObject;
	}
	return NULL;
}

void XEffectManager::AttachXDecalEffect( XDecalEffect* pDecalEffect, RSceneNode* pSceneNode, RActorNode* pActorNode )
{
	_ASSERT( global.smgr );

	if(pActorNode)
		pActorNode->AddChild(pDecalEffect->GetEffect());
	else if(pSceneNode)
		pSceneNode->AddChild(pDecalEffect->GetEffect());
	else
		global.smgr->AddSceneNode(pDecalEffect->GetEffect());
}

XDecalEffect* XEffectManager::GetXDecalEffect( MUID& uid )
{
	XEffectBase* pEffectBase = GetEffect(uid);

	if(pEffectBase &&
		pEffectBase->GetType() == ET_DECAL)
		return static_cast<XDecalEffect*>(pEffectBase);

	return NULL;
}

XEffectBase* XEffectManager::GetEffect( MUID& uid )
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

void XEffectManager::UpdataXEffectReserve()
{
	vector<MUID> vecDelete;

	vector<stEffectReserveData>::iterator it = m_vecEffectReserve.begin();
	while(it != m_vecEffectReserve.end())
	{
		// 오브젝트가 존재하냐?
		if(it->effectData.uidActor != MUID::ZERO)
		{
			XObject* pObject = gg.omgr->FindObject(it->effectData.uidActor);
			if(pObject == NULL)
			{
				vecDelete.push_back(it->pEffect->GetMUID());
				it = m_vecEffectReserve.erase(it);
				continue;
			}

			// 로딩은 되었냐?
			// 메쉬가 있는것만 체크하자.
			// 따라다녀야 하고 본이 있다면 로딩 체크는 필수다.
			XCharacter* pActor = pObject->GetActor();
			if(pActor &&
				pActor->m_pMesh &&
				pActor->CheckLoadingCompletedFastly() == false &&
				it->effectData.bFollow == true &&
				it->effectData.strBoneName.empty() == false)
			{
				++it;
				continue;
			}
		}

		// 이펙트 붙인다.
		EFFECT_TYPE effectType = it->pEffect->GetType();
		if(effectType == ET_MODEL)
			AddXEffect(static_cast<XEffect*>(it->pEffect), it->effectData);
		else if(effectType == ET_WEAPON)
		{
			if(AddXWeaponEffect(static_cast<XWeaponEffect*>(it->pEffect), it->effectData) == MUID::ZERO)
				vecDelete.push_back(it->pEffect->GetMUID());
		}

		it = m_vecEffectReserve.erase(it);
	}

	for(vector<MUID>::iterator itDel = vecDelete.begin(); itDel != vecDelete.end(); ++itDel)
	{
		DeleteXEffect(*itDel, true);
	}

	vecDelete.clear();
}

void XEffectManager::AddXEffectReserve( XEffectBase* pEffect, EFFECT_CREATE_DATA& effectCreaetData )
{
	stEffectReserveData reserveData;
	reserveData.pEffect		= pEffect;
	reserveData.effectData	= effectCreaetData;
	m_vecEffectReserve.push_back(reserveData);
}

void XEffectManager::DelXEffectReserve( MUID& uid )
{
	// 예약된거...
	for(vector<stEffectReserveData>::iterator it = m_vecEffectReserve.begin(); it != m_vecEffectReserve.end(); ++it)
	{
		if(it->pEffect->GetMUID() == uid)
		{
			m_vecEffectReserve.erase(it);
			return;
		}
	}

	// 대기중인거
	effectWait.DelEffectWaiting(uid);
}

void XEffectManager::AddHitInfo( XTalentHitParam* pHitInfo, unsigned int nDelayTime )
{
	m_HitEffectSelecter.AddHitInfo(pHitInfo, nDelayTime);
}

void XEffectManager::StartHitEffectSelect( XTalentHitParam* pHitParam )
{
	m_HitEffectSelecter.StartHitEffectSelect(pHitParam);
}

MUID XEffectManager::AddXSelfIllumination( EFFECT_CREATE_DATA& effectCreateData, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax, bool bLoop )
{
	// 중복 체크
	if(CheckDuplicationSelfIlluminationEffect(effectCreateData.uidActor, effectCreateData.strBoneName))
		return MUID::ZERO;

	// 생성
	XSelfIlluminationEffect* pIlluminationEffect = CreateXSelfIlluminationEffect(fStartTime, fStartDurationTime, fEndTime, fEndDurationTime, fMin, fMax);
	if (pIlluminationEffect == NULL )
		return MUID::ZERO;

	// 데이터 입력
	pIlluminationEffect->SetOwner(effectCreateData.uidActor);
	pIlluminationEffect->SetBoneName(effectCreateData.strBoneName);
	pIlluminationEffect->SetDurationTime(effectCreateData.fDurationTime);
	pIlluminationEffect->SetLoop(bLoop);
	pIlluminationEffect->SetCancelEnabled(effectCreateData.bCancelEnabled);

	// 모델이 아직 로딩이 안되었다면...
	//if(effectCreateData.pActor->IsLoadingCompleted() == false)
	//	effectWait.AddEffectWaiting(pIlluminationEffect->GetMUID(), effectCreateData, WET_SELFILLUMINATION);

	return pIlluminationEffect->GetMUID();
}

XSelfIlluminationEffect* XEffectManager::CreateXSelfIlluminationEffect( float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax )
{
	XSelfIlluminationEffect* pNewObject = new XSelfIlluminationEffect();
	if (pNewObject)
	{
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew, fStartTime, fStartDurationTime, fEndTime, fEndDurationTime, fMin, fMax ) )
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}
	return NULL;
}

bool XEffectManager::CheckDuplicationSelfIlluminationEffect( MUID& uidOwner, wstring& strActorNodeName )
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

MUID XEffectManager::AddXLightEffect( XObject* pObject, wstring& strBoneName, float fRadius, float fAttenuationStart, float fIntensity, const DWORD dwColor, float fDurationTime, bool bFollow, RMatrix _rWorld /*= RMatrix::IDENTITY*/ )
{
	// 생성
	XLightEffect* pLightEffect = CreateXLightEffect();
	if(pLightEffect == NULL)
		return MUID::ZERO;

	XCharacter* pActor = NULL;
	RActorNode* pActorNode = NULL;

	// 데어터 입력
	pLightEffect->SetRadius(fRadius);
	pLightEffect->SetColorRGB(dwColor);
	pLightEffect->SetDurationTime(fDurationTime);
	pLightEffect->SetAttenuationStart(fAttenuationStart);
	pLightEffect->SetIntensity(fIntensity);
	
	if(bFollow && pObject)
	{
		pActor = pObject->GetActor();
		pLightEffect->SetOwner(pObject->GetUID());
		pLightEffect->SetBoneName(strBoneName);

		if(pActor)
			pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(strBoneName.c_str()).c_str());

		if(_rWorld != MMatrix::IDENTITY)
		{
			// 이 매트릭스를 local로 바꾸어야 한다.
			MMatrix matLocal = _rWorld;
			MMatrix matActorWorld;
			MMatrix matWorldInverse;
			matActorWorld = pActor->GetWorldTransform();
			pActor->GetActorNodeTransform(strBoneName.c_str(), matActorWorld, E_TS_WORLD);

			matActorWorld.GetInverse(&matWorldInverse);
			matLocal *= matWorldInverse;

			_rWorld = matLocal;
			pLightEffect->SetLocalMatrix(matLocal);
		}
	}

	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( _rWorld, 3, 30, D3DCOLOR_XRGB(255, 255, 255));

	AttachSceneNode(pLightEffect->GetEffect(), NULL, NULL, _rWorld);

	return pLightEffect->GetMUID();
}

XLightEffect* XEffectManager::CreateXLightEffect()
{
	XLightEffect* pNewObject = new XLightEffect();
	if(pNewObject)
	{
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew))
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}

	return NULL;
}

void XEffectManager::AttachSceneNode( RSceneNode* pEffectInst, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame /*= RMatrix::IDENTITY*/, bool bIgnoreDir /*= false*/ )
{
	if (pSceneNode)
	{
		pSceneNode->AddChild(pEffectInst);
		if (pActorNode)
		{
			RActorNodeConstraintController* pConstraintController = new RActorNodeConstraintController;
			pConstraintController->SetTargetActorNode(pActorNode->GetNodeName());
			pConstraintController->SetIgnoreActorNodeDirection( bIgnoreDir );
			pEffectInst->AddController(pConstraintController);
		}
	}
	else
	{
		// root에 붙이기
		_ASSERT( pSceneNode == NULL );
		_ASSERT( pActorNode == NULL );	//만일 액터노드를 붙이려면 RSceneNode에 RActor를 채워주세요
		if(gg.currentgamestate->GetWorld())
			gg.currentgamestate->GetWorld()->GetWorldSceneNode()->AddChild(pEffectInst);
	}

	pEffectInst->SetTransform( pEffectInst->GetLocalTransform() * _rBaseFrame );
	pEffectInst->UpdateTransform();
}

XLightEffect* XEffectManager::GetXLightEffect( MUID& uid )
{
	XEffectBase* pEffectBase = GetEffect(uid);

	if(pEffectBase &&
		pEffectBase->GetType() == ET_LIGHT)
		return static_cast<XLightEffect*>(pEffectBase);

	return NULL;
}

void XEffectManager::SetXNoiseFadeEffect( XObject* pObject, vector<tstring> vecMeshNodeName, float fStartDurationTime, float fEndDurationTime, DWORD dwColor1, DWORD dwColor2, float fSize1, float fSize2, bool bInverse )
{
	// 생성
	XNoiseFadeEffect* pNoiseFadeEffect = CreateNoiseFadeEffect(fStartDurationTime, fEndDurationTime);
	if(pNoiseFadeEffect == NULL)
		return;

	pNoiseFadeEffect->SetOwner(pObject->GetUID());
	pNoiseFadeEffect->SetFadeEffectColor(dwColor1, dwColor2, fSize1, fSize2);
	pNoiseFadeEffect->SetMeshNodeName(vecMeshNodeName);
	pNoiseFadeEffect->SetInverse(bInverse);

	return;
}

XNoiseFadeEffect* XEffectManager::CreateNoiseFadeEffect( float fStartDurationTime, float fEndDurationTime )
{
	XNoiseFadeEffect* pNewObject = new XNoiseFadeEffect();
	if(pNewObject)
	{
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew, fStartDurationTime, fEndDurationTime))
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}

	return NULL;
}

void XEffectManager::ActorChangeMotion( MUID uidActor )
{
	for(vector<XEffectBase*>::iterator itor = m_vEffectList.begin(); itor != m_vEffectList.end(); ++itor)
	{
		XEffectBase* pEffectBase = (*itor);

		pEffectBase->CheckOwerChangeMotion(uidActor);
	}
}

MUID XEffectManager::AddXWeaponEffect( EFFECT_CREATE_DATA& effectCreateData )
{
	// 생성
	XWeaponEffect* pWeaponEffect = CreateWeaponEffect(effectCreateData.strEffectName);
	if(pWeaponEffect == NULL)
		return MUID::ZERO;

	return AddXWeaponEffect(pWeaponEffect, effectCreateData);
}

MUID XEffectManager::AddXWeaponEffect( XWeaponEffect* pEffect, EFFECT_CREATE_DATA& effectCreateData )
{
	// 체크
	XObject* pObject = gg.omgr->FindObject(effectCreateData.uidActor);
	if(pObject == NULL/* ||
		CheckWeaponEffectDummy(pObject->GetActor(), effectCreateData.strBoneName) == false*/)
	{
		return MUID::ZERO;
	}

	// 데이타 입력
	pEffect->SetBoneName(effectCreateData.strBoneName);
	pEffect->SetOwner(effectCreateData.uidActor);
	pEffect->SetPartsSlotType(effectCreateData.nPartsSlotType);

	RMatrix mat;
	mat.MakeIdentity();

	AttachXEffect( pEffect, NULL, NULL );

	return pEffect->GetMUID();
}

XWeaponEffect* XEffectManager::CreateWeaponEffect( const wstring& szEffectName )
{
	XWeaponEffect* pNewObject = new XWeaponEffect();
	if(pNewObject)
	{
		MUID uidNew = global.system->GenerateLocalUID();
		if(!pNewObject->Create(uidNew, szEffectName))
		{
			SAFE_DELETE(pNewObject);
			return NULL;
		}

		m_vEffectList.push_back(pNewObject);

		return pNewObject;
	}

	return NULL;
}

bool XEffectManager::CheckWeaponEffectDummy( XCharacter* pActor, wstring& strBoneName )
{
	if(pActor == NULL)
		return false;

	MMatrix mat;
	RActorNode* pActorNode = pActor->GetActorNodeTransform(strBoneName.c_str(), mat, E_TS_LOCAL);
	if(pActorNode == NULL)
		return false;

	return true;
}

MUID XEffectManager::AddWeaponEffectWaiting( EFFECT_CREATE_DATA& effectWaitingData )
{
	XWeaponEffect* pEffect = CreateWeaponEffect(effectWaitingData.strEffectName);
	if (pEffect == NULL )
		return MUID::ZERO;

	effectWait.AddEffectWaiting(pEffect->GetMUID(), effectWaitingData, WET_WEAPON);

	return pEffect->GetMUID();
}

XWeaponEffect* XEffectManager::GetXWeaponEffect( MUID& uid )
{
	XEffectBase* pEffectBase = GetEffect(uid);

	if(pEffectBase &&
		pEffectBase->GetType() == ET_WEAPON)
		return static_cast<XWeaponEffect*>(pEffectBase);

	return NULL;
}

XModelEffect* XEffectManager::GetXModelEffect( MUID& uid )
{
	XEffectBase* pEffectBase = GetEffect(uid);

	if(pEffectBase &&
		(pEffectBase->GetType() == ET_WEAPON || pEffectBase->GetType() == ET_MODEL))
		return static_cast<XModelEffect*>(pEffectBase);

	return NULL;
}

void XEffectManager::EnablePEEdge( MUID& uid, const RVector4& _color, float _fTickNess, float _fBlurFactor, bool _bProjection )
{
	m_PEEdgeMgr.EnablePEEdge(uid, _color, _fTickNess, _fBlurFactor, _bProjection);
}

void XEffectManager::DisablePEEdge( MUID& uid )
{
	m_PEEdgeMgr.DiablePEEdge(uid);
}
