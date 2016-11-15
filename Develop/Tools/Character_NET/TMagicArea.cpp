#include "stdafx.h"
#include "TMagicArea.h"
#include "TEffectCharacterToolSinglyInvoker.h"

// 마법진에 쓰이는 애니메이션은 정적입니다.
#define MAGICAREA_IDLE		"idle"
#define MAGICAREA_DESPAWN	"die"
#define MAGICAREA_SPAWN		"MF_KD3"
#define MAGICAREA_ATTACK	"spell_fire"

TMagicArea::TMagicArea()
: m_pNPCInfo(NULL)
, m_bModelLoadingComplete(false)
, m_bDespawn(false)
, m_pActor(NULL)
, m_pNPCActor(NULL)
, m_bStart(false)
, m_fExistTime(0.0f)
{
}

TMagicArea::~TMagicArea()
{
	OnDestroy();
}

void TMagicArea::OnUpdate( float fDelta )
{
	if(m_bStart == false)
		return;

	// 모델 업데이트
	if(m_pNPCActor)
	{
		RSceneNodeUpdateVisitor visitor;
		if (visitor.OnPreOrderVisit(m_pNPCActor))
			visitor.OnPostOrderVisit(m_pNPCActor);
	}
	// 모델 로딩 이후에 해야하는 것들...
	if(CheckModelLoadingComplete())
		DoModelLoadingComplete();

	// 애니메이션 상태에 따른 설정
	CheckAnimationState(fDelta);
}

bool TMagicArea::OnCreate()
{
	return true;
}

void TMagicArea::OnDestroy()
{
	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecEffectData.begin(); it != m_vecEffectData.end(); it++)
	{
		if((*it).type == CET_MODEL ||
			(*it).type == CET_SOUND ||
			(*it).type == CET_DECALEFFECT)
		{
			g_pMainApp->m_pEffectManager->DeleteTEffect((*it).uidEffect);
		}
	}

	m_vecEffectData.clear();

	if(m_pNPCActor)
	{
		m_pNPCActor->RemoveFromParent();
		m_pNPCActor->Destroy();
		SAFE_DELETE(m_pNPCActor);
	}

	m_bStart = false;
}

void TMagicArea::OnRender()
{
	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecEffectData.begin(); it != m_vecEffectData.end(); it++)
	{
		if(it->type == CET_DECALEFFECT)
		{
			TDecalEffect* pDecalEffect = static_cast<TDecalEffect*>(g_pMainApp->m_pEffectManager->GetEffect(it->uidEffect));
			pDecalEffect->RenderXEffect();
		}
	}
}

void TMagicArea::Setup( TCharacter* pActor, CSTalentInfo* pInfo, CSTalentEventInfo * pEvent /*= NULL*/ )
{
	if(pInfo == NULL ||
		pActor == NULL)
		return;

	m_pActor = pActor;

	m_matWorld.MakeIdentity();

	// 버프 ID
	int nBuffID = -1;
	if(pInfo->m_nExtraActiveParam1 > 0)
		nBuffID = pInfo->m_nExtraActiveParam1;
	else if(pInfo->m_nExtraActiveParam2 > 0)
		nBuffID = pInfo->m_nExtraActiveParam2;

	XBuffInfo* pBuffInfo = g_pMainApp->m_BuffInfo.Get(nBuffID);
	if(pBuffInfo == NULL)
		return;

	vec3 vLocal(0, 0, 0);

	if(pEvent)
	{
		vLocal = vec3(pEvent->m_vLocal.x, pEvent->m_vLocal.y, 0.0f);
	}

	// 이펙트
	EffectCreate(pBuffInfo, vLocal);

	// 모델
	SetupModel((TTalentInfo*)pInfo);

	m_bStart = true;
	m_fExistTime = 5.f;
}

void TMagicArea::SetupModel( TTalentInfo* pTalentInfo )
{
	int nNPCID = 0;
	if (nNPCID > 0)
	{
		TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(nNPCID);
		if(pNPCInfo == NULL)
			return;

		// npc
		m_pNPCInfo = pNPCInfo;

		char szBuf[_MAX_DIR] = {0,};
		sprintf_s(szBuf, "%s%s%s.elu", PATH_MODEL_TOOL
			, pNPCInfo->strMeshPath.c_str(), pNPCInfo->strMeshName.c_str()
			, pNPCInfo->strMeshName.c_str());

		m_pNPCActor = new TCharacter;
		if(!m_pNPCActor->RActor::Create(szBuf))
		{
			SAFE_DELETE(m_pNPCActor);

			mlog("에러 : Mesh파일(%s) 로드 실패 \r", szBuf);

			return;
		}

		AniSpawn();

		g_pMainApp->GetSceneMgr()->AddSceneNode(m_pNPCActor);
	}
}

void TMagicArea::AniSpawn()
{
	m_pNPCActor->SetAnimation(string(MAGICAREA_SPAWN), RAT_NONE);
}

void TMagicArea::AniIdle()
{
	m_pNPCActor->SetAnimation(string(MAGICAREA_IDLE), RAT_NONE);
}

void TMagicArea::AniAttack()
{
	if(m_pNPCActor->SetAnimation(string(MAGICAREA_ATTACK), RAT_NONE) == false)
	{
		OnDestroy();
	}
}

void TMagicArea::AniDespawn()
{
	if(m_pNPCActor->SetAnimation(string(MAGICAREA_DESPAWN), RAT_NONE) == false)
	{
		OnDestroy();
	}

	m_bDespawn = true;
}

void TMagicArea::TrapAction()
{
	AniAttack();
}

void TMagicArea::Despawn()
{
	AniDespawn();
}

bool TMagicArea::CheckModelLoadingComplete()
{
	if(m_pNPCActor &&
		m_bModelLoadingComplete == false &&
		m_pNPCActor->IsLoadingCompleted())
	{
		m_bModelLoadingComplete = true;

		return true;
	}

	return false;
}

void TMagicArea::CheckAnimationState(float fDelta)
{
	// 애니메이션 상태에 따른 설정
	if(m_pNPCActor &&
		m_pNPCActor->GetCurAnimation())
	{
		string strAliasName = (string)m_pNPCActor->GetCurAnimation()->GetAliasName();
		// 1. 스폰이후에는 Idle 애니메이션으로 변경한다.
		if(strAliasName == MAGICAREA_SPAWN &&
			m_pNPCActor->IsPlayDone())
			AniIdle();
		else if(strAliasName == MAGICAREA_IDLE &&
			m_pNPCActor->IsOncePlayDone())
			Despawn();
		// 2. 공격이나 사라지는 경우 이후에는 자동으로 지우게 한다.
		if((strAliasName == MAGICAREA_DESPAWN ||
			strAliasName == MAGICAREA_ATTACK) &&
			m_pNPCActor->IsPlayDone())
			OnDestroy();
	}
	else
	{
		m_fExistTime -= fDelta;

		if(m_fExistTime <= 0.0f)
			OnDestroy();
	}
}

void TMagicArea::DoModelLoadingComplete()
{
	// 로딩 완료 후 행하는 것들...
	// 죽었나?
	if(m_bDespawn)
		return;

	// 스폰
	AniSpawn();
}

void TMagicArea::EffectCreate(XBuffInfo* pBuffInfo, vec3 vCenter)
{
	// 위치
	m_matWorld.SetLocalMatrix(vCenter, vec3::AXISY, vec3::AXISZ);

	RMatrix matResult = m_pActor->GetWorldTransform();
	matResult *= m_matWorld;

	vec3 vPos = vec3(matResult._41, matResult._42, matResult._43);
	vec3 vDir = vec3(matResult._21, matResult._22, matResult._23);
	vec3 vUp = vec3(matResult._31, matResult._32, matResult._33);

	// 버프 이펙트 정보 가져오기
	vector<XBuffEffectInfo *> vecEffectInfoByType;
	pBuffInfo->m_BuffEffect.GetEffectInfoList(BUFF_GAIN_EFFECT, vecEffectInfoByType);

	for(vector<XBuffEffectInfo *>::iterator itBuffEffect = vecEffectInfoByType.begin(); itBuffEffect != vecEffectInfoByType.end(); ++itBuffEffect)
	{
		vector<EFFECT_RESULT_DATA> vecResult;
		TEffectCharacterToolSinglyInvoker effectInvoker;
		effectInvoker.Invoke((*itBuffEffect)->m_strEffectName, vPos, vDir, vUp, pBuffInfo->m_TriggeredBuff.infoEffect.m_fRadius, &vecResult);
		//effectInvoker.Invoke(m_pActor, (*itBuffEffect)->m_strEffectName, string(""), string(""), &vecResult, NULL, pBuffInfo->m_TriggeredBuff.infoEffect.m_fRadius, m_matWorld);

		for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecResult.begin(); itUID != vecResult.end(); itUID++)
		{
			XEffectBase* pEffect = g_pMainApp->m_pEffectManager->GetEffect((*itUID).uidEffect);
			if(pEffect/* && pEffect->IsEffectLoop()*/)
			{
				m_vecEffectData.push_back(*itUID);
			}
		}
	}
}
