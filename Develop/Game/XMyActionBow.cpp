#include "StdAfx.h"
#include "XMyActionBow.h"
#include "XModuleMyControl.h"
#include "XMyPlayer.h"
#include "XEventID.h"
#include "XModuleTalent.h"
#include "XTargetSticky.h"
#include "XModuleEntity.h"
#include "XModuleCollision.h"
#include "XModuleAction.h"
#include "XModuleBuff.h"
#include "XCameraManager.h"
#include "XGameTransformControllerManager.h"
#include "XModulePost.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "XPost_Action.h"

XMyActionBow::XMyActionBow(XModuleMyControl* pOwner)
: XMyActionState(pOwner)
{
	m_bShooted			= false;
	m_nAimingTime		= 0;
	m_nPreparingTime	= 0;
	m_pTarget			= new XTargetSticky(pOwner->GetOwner());
}

XMyActionBow::~XMyActionBow(void)
{
	SAFE_DELETE(m_pTarget);
}

bool XMyActionBow::CheckEnterable(void* pParam)
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable())
		return false;


	if (m_pOwner->GetMyPlayerOwner()->IsDead()) return false;
	if (!m_pOwner->GetMyPlayerOwner()->IsCurrStanceBattle()) return false;

	if (AmIGod()) return true;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(PLAYER_NORMAL_ATTACK_ARROW_TALENT_ID);
	if (pTalentInfo == NULL || pTalentInfo->m_nSTACost > gvar.MyInfo.ChrStatus.nSTA) return false;

	return true;
}

void XMyActionBow::Exit()
{
	m_pOwner->RemoveSpineController();
	m_pTarget->ExitTarget();

	XMyActionState::Exit();
}

void XMyActionBow::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	if( !m_pOwner->GetOwner()->IsModuleEntity()) return;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(PLAYER_NORMAL_ATTACK_ARROW_TALENT_ID);
	if (pTalentInfo == NULL) return;

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (!pModuleAction) return;

	XModuleEntity* pModuleEntity = m_pOwner->GetOwner()->GetModuleEntity();

	pModuleAction->StopMove(false);

	XModulePost* pModulePost = m_pOwner->GetOwner()->GetModulePost();

	pModulePost->PostMoveStop(m_pOwner->GetOwner()->GetPosition());

	XPostAttackPrepare();
	// 공격 준비 메세지 송신
	//vec3 vCameraDir = gg.controller->GetBackViewCameraDir();
	//XPostUseTalent(PLAYER_NORMAL_ATTACK_ARROW_TALENT_ID, vCameraDir);

	m_bShooted		= false;
	m_nAimingTime	= XGetNowTime();
	m_nPreparingTime= (int)(1000.0f * pModuleEntity->GetAnimationController()->GetAniMaxFrame() / float(ANIMATION_FRAME_PER_SEC));

	m_pTarget->InitTarget(true);

	m_pTarget->SetTargetInfo(pTalentInfo->m_fRange, pTalentInfo->m_nENCost, pTalentInfo->m_EffectInfo.m_nRelation);

	m_pOwner->UseTalent(PLAYER_NORMAL_ATTACK_ARROW_TALENT_ID);
}

XEventResult XMyActionBow::Event(XEvent &msg)
{
	if (msg.m_nID == XEVTL_VIRTUAL_KEY_RELEASED)
	{
		XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
		if( nVirtualKey == VKEY_ACTION)
		{
			if(m_bShooted == false)
			{
				if (IsDonePreparing())
				{
					if(global.ui)
					{
						global.ui->TargetUIEnd();
					}

					doShoot();
				}
				else
				{
					doCancel();
				}

				return MR_TRUE;
			}
		}
	}

	return XMyActionState::Event(msg);
}

void XMyActionBow::Update(float fDelta)
{
	// 준비
	if( !m_bShooted )
		doReady();


}

void XMyActionBow::doReady()
{
	//이부분을 좀....
	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
	if (pCameraNode && m_pOwner->GetOwner()->IsModuleEntity())
	{
		vec3 dir = pCameraNode->GetDirection();
		vec3 pos = pCameraNode->GetPosition();

		vec3 vdir = dir;
		dir.z = 0.0f;
		dir.Normalize();

		// TODO: 여기안을 정리해 봅시다.
		//if (!m_bShooted)
		m_pOwner->ActorRotation(vdir);
	}

	// 타겟 체크
	if (m_pOwner->GetOwner()->IsModuleEntity())
	{
		m_pTarget->UpdateTarget();
		//m_pOwner->GetTalentSpellTarget();
	}
}


void XMyActionBow::doShoot()
{
	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
	if (pCameraNode == NULL) return;

	XModuleTalent* pModuleTalent = m_pOwner->GetOwner()->GetModuleTalent();

	if(pModuleTalent == NULL)
		return;

	if (pModuleTalent->GetTalent() == NULL) return;
	MUID uidTarget	= 0;
	int nGroupID	= -1;
	int nDummyID	= -1;

	// 타겟 가져오기
	stTargetID* pTargetInfo = m_pTarget->GetTargetInfo();
	if (pTargetInfo)
	{
		XObject* pTargetObject = gg.omgr->FindObject(pTargetInfo->uidTarget);
		if(pTargetObject && pTargetObject->IsModuleEntity())
		{
			// 타겟 정보 셋팅
			uidTarget	= pTargetInfo->uidTarget;
			nGroupID	= pTargetInfo->nTargetGroupID;
			nDummyID	= pTargetInfo->nTargetCapsuleID;

			if (pCameraNode && m_pOwner->GetOwner()->IsModuleEntity())
			{
				XModuleCollision* pTargetCollision = pTargetObject->GetModuleCollision();
				if(pTargetCollision == NULL)
					return;

				vec3 campos			= pCameraNode->GetPosition();
				vec3 tar_center		= pTargetCollision->GetColObjectCenter();

				// 그룹과 더미 ID가 있다면... 타겟 위치가 오브젝트가 아니라 캡슐이다.
				if(nGroupID >= 0 &&nDummyID >= 0)
				{
					vec3 vCollCpasuleCenter = pTargetCollision->GetHitEffectProjectileTargetPos(nGroupID, nDummyID);
					if(vCollCpasuleCenter != vec3::ZERO)
					{
						tar_center = vCollCpasuleCenter;
					}
				}
				vec3 dir			= tar_center - pCameraNode->GetPosition();
				dir.Normalize();

				float fAngleDiff = pCameraNode->GetDirection().AngleBetween(dir);
				if (fAngleDiff > 0.02f)
				{
					pCameraNode->SetDirection(dir);

					dir		= tar_center - m_pOwner->GetOwner()->GetPosition();
					dir.z	= 0;
					dir.Normalize();
					m_pOwner->GetOwner()->SetDirection(dir);
				}
			}
		}
	}
	else
	{
		return;
	}

	vec3 vArrowStartPos;
	vec3 vDir;

	vArrowStartPos	= m_pOwner->GetOwner()->GetPosition();
	vDir			= m_pOwner->GetOwner()->GetDirection();

	// 타겟 입력
	pModuleTalent->SetTarget(uidTarget, nGroupID, nDummyID);

	// 부분 판정 타겟이면...
	if(nGroupID >= 0 &&
		nDummyID >= 0)
	{
		XPostAttackArcheryDetail(vArrowStartPos, vDir, uidTarget, nGroupID, nDummyID);
	}
	else
	{
		XPostAttackArchery(vArrowStartPos, vDir, uidTarget);
	}

	m_bShooted = true;
}

void XMyActionBow::doCancel()
{
	if( !m_bShooted )
		XPostAttackCancel();

	m_pOwner->DoActionIdle();
}

bool XMyActionBow::IsDonePreparing()
{
	uint32 nNowTime = XGetNowTime();
	if (nNowTime - m_nAimingTime > m_nPreparingTime) return true;
	return false;
}
