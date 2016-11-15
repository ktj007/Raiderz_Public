#include "stdafx.h"
#include "XActionDie.h"
#include "XModuleEntity.h"
#include "XModuleCollision.h"
#include "XModuleBuff.h"
#include "XModuleMotion.h"
#include "XModuleAction.h"
#include "XModuleActorControl.h"
#include "XModuleTalent.h"
#include "XGameTransformControllerManager.h"

XActionDie::XActionDie()
{
	m_bStartDie		= false;
	m_bMFDie		= false;
	m_bDieAniEnd	= false;

	m_vDieStartUp	= vec3(0, 0, 0);
	m_vDieStartDir	= vec3(0, 0, 0);
	m_vDieTargetUp	= vec3(0, 0, 0);
	m_vDieTargetDir	= vec3(0, 0, 0);
}

XActionDie::~XActionDie()
{

}

void XActionDie::Init( XObject* pObject )
{
	m_pOwner		= pObject;
}

void XActionDie::StartDie(bool bAni)
{
	// 애니메이션이 없으면 애니끝나고 이후 처리를 실행한다.
	if(bAni == false)
	{
		DieAniEnd();
		return;
	}

	// 이미 시작했다면...
	if(m_bStartDie)
		return;

	StartTerrainUpDir();
}

void XActionDie::OnStartDie( ACTION_DIE_STATE eState )
{
	switch(eState)
	{
	case ADS_NORMAL:
	case ADS_MF_NO_ANIMATION:
	case ADS_KNOCKBACK:
		{
			// 다이 애니메이션 시작
			const wchar_t* szDieMotionName = MOTION_NAME_DIE;
			if (m_pOwner->GetEntityType() == ETID_NPC) szDieMotionName = MOTION_NAME_NPC_DIE;

			// 애니메이션 및 탤런트... 기타등등 모두 멈춘다.
			XModuleAction* pModuleAction = m_pOwner->GetModuleAction();
			if(pModuleAction)
			{
				pModuleAction->StopUpperAni();		// 상체 애니메이션 취소
			}
			
			XModuleTalent* pModuleTalent = m_pOwner->GetModuleTalent();
			if(pModuleTalent)
			{
				pModuleTalent->Cancel();
			}

			// 다이 애니메이션 셋팅
			bool bSetAni = true;
			XModuleMotion * pModuleMotion = m_pOwner->GetModuleMotion();
			if(pModuleMotion && pModuleMotion->GetCurrMotion() != szDieMotionName)
			{
				//mlog("다이 애니 셋팅\n");
				bSetAni = pModuleMotion->ChangeMotion(szDieMotionName, MT_DEFAULT, true);
			}
			
			StartDie(bSetAni);
			//mlog("노멀 다이어요\n");

			break;
		}
		
	case ADS_KNOCKDOWN:
	case ADS_THROWUP:
	case ADS_GRAPPLED:
	case ADS_UPPERED:
		{
			// 모션다이면... 모션을 수시로 체크해야함...
			m_bMFDie = true;

			//mlog("모션다이에요\n");
			StartTerrainUpDir();
			//mlog("모션 다이어요\n");
			break;
		}
	}
}

void XActionDie::OnStartDie( MF_STATE eMFState, bool bAniSet )
{
	// 애니메이션 셋팅이 된것만 다음에 해당한다.
	// 애니메이션이 없으면... 그냥 다이 애니로 셋팅한다.
	if(bAniSet)
	{
		switch(eMFState)
		{
		case MF_KNOCKBACK:
			{
				OnStartDie(ADS_KNOCKBACK);
				return;
			}

		case MF_THROWUP:
			{
				OnStartDie(ADS_THROWUP);
				return;
			}

		case MF_KNOCKDOWN:
			{
				OnStartDie(ADS_KNOCKDOWN);
				return;
			}

		case MF_UPPERED:
			{
				OnStartDie(ADS_UPPERED);
				return;
			}

		case MF_GRAPPLED:
		case MF_SWALLOWED:
			{
				OnStartDie(ADS_GRAPPLED);
				return;
			}
		}
	}

	OnStartDie(ADS_NORMAL);
}

void XActionDie::OnEndDie()
{
	m_bStartDie		= false;
	m_bMFDie		= false;
	m_bDieAniEnd	= false;

	XModuleEntity * pModuleEntity		= m_pOwner->GetModuleEntity();
	if(pModuleEntity)
	{
		pModuleEntity->_SetUpVector(UP_VECTOR);
		pModuleEntity->_SetRenderDirection( vec3::ZERO);
	}
}

void XActionDie::Update( float fDelta )
{
	if(m_bStartDie && m_bMFDie == false)
	{
		XModuleEntity * pModuleEntity		= m_pOwner->GetModuleEntity();
		if(pModuleEntity == NULL)
		{
			return;
		}

		//mlog("1 %s\n", MLocale::ConvTCHARToAnsi(m_pOwner->GetModuleMotion()->GetCurrMotion().c_str()).c_str());
		//XActor* pThisActor = static_cast<XActor*>(m_pOwner);

		//if(pThisActor == NULL)
		//	return;
		if(m_pOwner == NULL)
			return;

		// 마지막 프레임에 도달하면 처리하는 함수
		if(pModuleEntity->GetAnimationController()->IsOncePlayDone())
		{
			if (m_bMFDie == false)
			{
				//mlog("그냥인데... 마지막 프레임?");
				DieAniEnd();
			}

			m_bStartDie = false;
			return;
		}

		//------------------------------------------------------------------------
		// 죽었을때에 지형 경사에 의한 회전
		// 플레이어나 NPC에만 해당
		//if(pThisActor->IsDead())								// NPC나 플레이어가 죽었는가?
		{
			UpdateTerrainUpDir(fDelta);
		}
	}
	else if(m_bMFDie)
	{
		XModuleMotion * pModuleMotion = m_pOwner->GetModuleMotion();
		XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();

		if(pModuleMotion == NULL ||
			pModuleEntity == NULL)
			return;

		wstring strDieMotionName = MOTION_NAME_DIE;
		if (m_pOwner->GetEntityType() == ETID_NPC) strDieMotionName = MOTION_NAME_NPC_DIE;

		wstring strIdleMotionName = MOTION_NAME_IDLE;
		if (m_pOwner->GetEntityType() == ETID_NPC) strIdleMotionName = MOTION_NAME_NPC_IDLE;

		//mlog("2 %s\n", MLocale::ConvTCHARToAnsi(pModuleMotion->GetCurrMotion().c_str()).c_str());
		if(pModuleMotion->IsHoldingSequence())
		{
			DoneDie();
			m_bMFDie = false;
		}
		else if(pModuleMotion->GetCurrMotion() == strDieMotionName)
		{
			if(pModuleEntity->GetAnimationController()->IsOncePlayDone())
			{
				//mlog("다이 모션이었나요?");
				DieAniEnd();
				m_bMFDie = false;
			}
			else
				UpdateTerrainUpDir(fDelta);
		}
		else if(pModuleMotion->GetCurrMotion() == strIdleMotionName ||
			pModuleMotion->IsPastDeadPostTrigger())
		{
			m_bMFDie = false;
			//mlog("노멀로 다시 죽어요\n");
			OnStartDie(ADS_NORMAL);
		}

		UpdateTerrainUpDir(fDelta);
	}
}

bool XActionDie::CalTerrainUpDir( vec3& pos, vec3& dir, vec3& outUp, vec3& outDir )
{
	XModuleCollision * pModuleCollision = m_pOwner->GetModuleCollision();
	if(pModuleCollision)
	{
		vec3 vStart	= pos + RVector(0,0,100);
		vec3 vEnd	= pos + RVector(0,0,-1000);
		vec3 vOutPos;
		vec3 vOutNor;

		if(pModuleCollision->PickRayToTerrain(vStart, vEnd, vOutPos, vOutNor))
		{
			// 업벡터 셋팅
			vOutNor.Normalize();

			// 실제 방향 구하기
			vec3 vDir		= dir;
			vec3 vCross		= vDir.CrossProduct(vOutNor);
			vec3 vRealDir	= vOutNor.CrossProduct(vCross);

			outUp			= vOutNor;
			outDir			= vRealDir;
		}
		else
		{
			// 지형이 없다면...
			outUp.Set(0.0f, 0.0f, 1.0f);
			outDir.Set(dir);
		}

		return true;
	}

	return false;
}

void XActionDie::DoneDie()
{
	// 이미 죽어 있었다.
	XModuleEntity * pModuleEntity		= m_pOwner->GetModuleEntity();
	if(pModuleEntity)
	{
		vec3 vTerrainUp;
		vec3 vTerrainDir;

		if(CalTerrainUpDir(pModuleEntity->GetPosition(), pModuleEntity->GetDirection(), vTerrainUp, vTerrainDir))
		{
			pModuleEntity->_SetRenderUpDir(vTerrainUp, vTerrainDir);
		}
	}

	DieAniEnd();
}

void XActionDie::DieAniEnd()
{
	XModuleActorControl* pActorControl = m_pOwner->GetModuleActorControl();
	if(pActorControl)
	{
		pActorControl->OnDieAniEnd();
	}

	m_bDieAniEnd = true;
}

bool XActionDie::IsDoingActionDie()
{
	// 시작도 안했다. 현재 종료상태이다.
	if(m_bStartDie == false &&
		m_bDieAniEnd == false)
		return false;

	// 작동중이다.
	return true;
}

void XActionDie::StartTerrainUpDir()
{
	XModuleEntity * pModuleEntity		= m_pOwner->GetModuleEntity();
	if(pModuleEntity)
	{
		vec3 vTerrainUp;
		vec3 vTerrainDir;

		pModuleEntity->InitRenderPosDirUp();

		if(CalTerrainUpDir(pModuleEntity->GetPosition(), pModuleEntity->GetDirection(), vTerrainUp, vTerrainDir))
		{
			// 처음시작에 대한 정보 셋팅					
			m_vDieStartUp = vec3::AXISZ;
			if(pModuleEntity->GetActor())
				m_vDieStartUp = pModuleEntity->GetActor()->GetWorldUp();

			m_vDieStartDir = pModuleEntity->GetDirection();
			m_vDieTargetUp = vTerrainUp;
			m_vDieTargetDir = vTerrainDir;

			m_bStartDie		= true;
			m_bDieAniEnd	= false;
		}
	}
}

void XActionDie::UpdateTerrainUpDir( float fDelat )
{
	XModuleEntity * pModuleEntity		= m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	if(m_pOwner->GetEntityType() == ETID_PLAYER ||
		m_pOwner->GetEntityType() == ETID_NPC)
	{
		// 위조건이 맞다면 지형에 붙인다.
		// 보간을 한다.
		// 시간 비율을 계산한다.
		// 총 애니메이션 시간의 2/3까지 회전을 끝낸다.
		if(pModuleEntity->GetAnimationController()->GetAnimation())
		{
			float fCurrentFrameRatio = 1.0f - (pModuleEntity->GetAnimationController()->GetCurFrameToSecond() / (pModuleEntity->GetAnimationController()->GetMaxFrameToSecond() * 2.0f / 3.0f));
			if(fCurrentFrameRatio < 0.0f)
				fCurrentFrameRatio = 0.0f;

			vec3 vCurrentUp = m_vDieStartUp.GetInterpolated(m_vDieTargetUp, fCurrentFrameRatio);
			vec3 vCurrentDir = m_vDieStartDir.GetInterpolated(m_vDieTargetDir, fCurrentFrameRatio);

			// 업, 방향 셋팅
			pModuleEntity->SetRenderUpDir(vCurrentUp, vCurrentDir);
		}
	}
}
