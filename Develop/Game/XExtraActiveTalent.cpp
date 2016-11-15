#include "StdAfx.h"
#include "XExtraActiveTalent.h"
#include "XTalentEvent.h"
#include "XModuleEntity.h"
#include "XModuleActorControl.h"
#include "XGameTransformControllerManager.h"

#ifdef _DEBUG
//#define GRAPLLED_DEBUG_TALENT
//#include "RDebugPassRenderer.h"
#endif

void XExtraActiveTalent::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str(), L"", m_pTalentInfo->m_nMovableType);
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);
}

/////////////////////////////////////////////////////////////////////////
void XExtraActiveTalent_Grapple::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
}

void XExtraActiveTalent_Grapple::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);

	StartAttach();
}

void XExtraActiveTalent_Grapple::OnEnterPhaseExtraAct()
{
	// 정보 빼오기... 
	GetGrappledHitInfo();

	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_EXTRA, m_pTalentInfo->m_szExtraActAnimation.c_str());

	m_pTalentEvent->SetActStart(true);
	StartAttach();
}

void XExtraActiveTalent_Grapple::OnEnterPhaseCanceled()
{
	EndAttach();
}

void XExtraActiveTalent_Grapple::OnEnterPhaseFinish()
{
	EndAttach();
}

void XExtraActiveTalent_Grapple::EndAttach()
{
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	//XEntityTransformControlAttacher* pAttacher = pModuleEntity->GetTransformControl<XEntityTransformControlAttacher>();
	//if(pAttacher)
	//{
	//	// 상대방이 스스로 풀지 못했다면... 여기서
	//	// 상대방에게 풀렸다고 알린다.
	//	XObject * pTarget = gg.omgr->Find(pAttacher->GetTarget());
	//	if(pTarget)
	//	{
	//		XModuleActorControl * pModuleActorControl = pTarget->GetModuleActorControl();
	//		if(pModuleActorControl)
	//			pModuleActorControl->Stop();
	//	}
	//}
}

void XExtraActiveTalent_Grapple::StartAttach()
{
	// 사용할 더미 노드 추가(필요 할때에만 등록을 합니다.)
	// 애니메이션이 셋팅이 되면 초기화가 되므로 애니메이션이 변경이 될때마다 셋팅을 해준다.
	XModuleEntity* pEntity = m_pOwner->GetModuleEntity();
	if(pEntity)
		pEntity->GetAnimationController()->UseDummyGrip();
}

void XExtraActiveTalent_Grapple::GetGrappledHitInfo()
{
	XTalentInfo* pTalent = GetInfo();
	if(pTalent == NULL ||
		pTalent->m_HitInfo.m_vSegments.size() == 0)
		return;

	int nCurFrame = m_pOwner->GetModuleEntity()->GetAnimationController()->GetAniFrameWithUpdateTime();

	// 아직 Throw 모션이 아니인데... 히트 시간이 지났다면... 붙어라...
	m_nGrappledHitFrame = m_pOwner->GetActor()->GetSecToFrame(pTalent->m_HitInfo.m_vSegments[0].m_fCheckTime);
	if(nCurFrame >= m_nGrappledHitFrame)
	{
		return;
	}

	// 어쩔수 없이 쓰는거다. 
	// 자주 부르는것이 아니므로 괜찮다.
	m_pOwner->GetModuleEntity()->GetAnimationController()->GetActorNodePosByFrame(wstring(GRAPPLED_NPC_GRIP_BONE_NAME), m_nGrappledHitFrame, m_vGrappledPos);

	int nRemainFrame = m_nGrappledHitFrame - nCurFrame;
	m_fGrappledRemainTime = m_pOwner->GetActor()->GetFrameToSec(nRemainFrame);

#ifdef GRAPLLED_DEBUG_TALENT
	vec3 vTest;
	m_pOwner->GetActor()->GetActorNodePos(GRAPPLED_NPC_GRIP_BONE_NAME, vTest, E_TS_WORLD, true);
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation( vTest);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 5, 20, D3DCOLOR_XRGB(255, 255, 0));
#endif
}

//////////////////////////////////////////////////////////////////////////
void XExtraActiveTalent_Eat::OnEnterPhaseExtraAct2()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_EXTRA2, m_pTalentInfo->m_szExtraActAnimation2.c_str());
	StartAttach();

}

void XExtraActiveTalent_Eat::OnEnterPhaseExtraAct3()
{
	ChangeTalentMotion(TALENT_PHASE_EXTRA3, m_pTalentInfo->m_szExtraActAnimation3.c_str());
	StartAttach();
}