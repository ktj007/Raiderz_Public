/************************************************************
MODULE NAME : XModuleMotion

HISTORY :
Date		Name		Description
2006-12-10	남기룡		새롭게 애니메이션 처리 클래스 수정

DESCRIPTION
 오브젝트의 애니메이션을 관리하는 모듈
*************************************************************/

#include "StdAfx.h"
#include "XModuleMotion.h"
#include "XObject.h"
#include "XModuleEntity.h"
#include "XEventID.h"
#include "XPlayer.h"
#include "XNonPlayer.h"

#include "RMesh.h"
#include "XGameTransformControllerManager.h"
#include "XMotionLogic.h"
#include "XUpperMotionLogic.h"
#include "XUpperMotionTimeModifier.h"

////////////////////////////////////////////////////////////////////////////////////////////
XModuleMotion::XModuleMotion(XObject* pOwner) : XModule(pOwner), m_pModuleEntity(NULL), m_pMotionLogic(NULL), m_pUpperMotionLogic(NULL)
{
	
}

XModuleMotion::~XModuleMotion(void)
{
	SAFE_DELETE(m_pMotionLogic);
	SAFE_DELETE(m_pUpperMotionLogic);
}

void XModuleMotion::OnInitialized()
{
	XModule::OnInitialized();

	m_pModuleEntity = m_pOwner->GetModuleEntity();
	_ASSERT(m_pModuleEntity);

	XGameTransformControllerManager* pAnimationController = m_pModuleEntity->GetAnimationController();

	_ASSERT(m_pOwner->IsNPC() || m_pOwner->IsPlayer());
	m_pMotionLogic = new XMotionLogic(static_cast<XActor*>(m_pOwner), 
									  pAnimationController,
									  new XMotionTimeModifier(m_pModuleEntity->GetActor(), pAnimationController),
									  &m_MotionIdleMemory);
	m_pUpperMotionLogic = new XUpperMotionLogic(static_cast<XActor*>(m_pOwner), 
												pAnimationController,
												new XUpperMotionTimeModifier(m_pModuleEntity->GetActor(), pAnimationController),
												NULL);
}

XEventResult XModuleMotion::OnEvent(XEvent& msg)
{
	XModule::OnEvent(msg);

	switch (msg.m_nID)
	{
	case XEVTL_ON_MOTION_EVENT:
		{
			wchar_t* szEvent = static_cast<wchar_t*>(msg.m_pData);

			m_pMotionLogic->OnEvent(szEvent);

			return MR_TRUE;
		}
		break;
	case XEVTD_MESH_LOADING_COMPLETE:
		{
			// 죽은 애니메이션 처리
			XActor * pOwnerActor = static_cast<XActor*>(m_pOwner);
			if(pOwnerActor)
			{
				pOwnerActor->DeadProcEnterField();
			}
		}
		break;
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			m_MotionIdleMemory.Init();
		}
		break;
	};

	return MR_FALSE;
}


void XModuleMotion::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();

	Subscribe(XEVTL_ON_MOTION_EVENT);
	Subscribe(XEVTD_MESH_LOADING_COMPLETE);
	Subscribe(XEVTL_ON_CHANGED_MOTION_TO_IDLE);
}

XMotionTypes XModuleMotion::GetCurrMotionType()
{
	return m_pMotionLogic->GetCurrMotionType();
}

bool XModuleMotion::ChangeMotion( const wchar_t* szMotionName, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false*/, bool bForceIdle/*=false */ )
{
	// [2007/7/19 isnara]
	// _ChangeMotion 결과값에 의해서 true일때만 모션큐를 지운다.
	if(m_MotionIdleMemory.CheckChangeMotion(szMotionName, m_pMotionLogic, bForceIdle) == false)
		return m_pMotionLogic->ChangeMotion(szMotionName, nType, bForced);	

	return true;
}

bool XModuleMotion::ChangeMotion(const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType, bool bForced)
{
	return m_pMotionLogic->ChangeMotion(szMotionName, szNextMotionName, nType, bForced);
}

// 커스텀 모션은 XMotionMgr::InitCustomMotions() 에서 추가해줘야 한다.
bool XModuleMotion::ChangeToCustomMotion(const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced)
{
	return m_pMotionLogic->ChangeToCustomMotion(szMotionName, szAnimationName, bForced);
}

void XModuleMotion::PushCustomMotion( const wchar_t* szMotionName, const wchar_t* szAnimationName, float fMotionSpeed /*=1.0f*/, float fMotionSppedRatio /*= 1.0f*/ )
{
	m_pMotionLogic->PushCustomMotion(szMotionName, szAnimationName, fMotionSpeed, fMotionSppedRatio);
}

void XModuleMotion::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	PFI_BLOCK_THISFUNC(302);

	m_pMotionLogic->Update(fDelta);
	m_pUpperMotionLogic->Update(fDelta);
}

wstring XModuleMotion::GetCurrMotion()
{
	return m_pMotionLogic->GetCurrMotion();
}

wstring XModuleMotion::GetCurrSequence()
{
	return m_pMotionLogic->GetCurrSequence();
}


bool XModuleMotion::IsCurrentMotion(wstring szMotionName)
{
	return m_pMotionLogic->IsCurrentMotion(szMotionName);
}

bool XModuleMotion::IsCameraLook()
{
	return m_pMotionLogic->IsCameraLook();
}

void XModuleMotion::PushMotion( const wchar_t* szMotionName, float fMotionSpeed /*=1.0f*/, float fMotionSppedRatio /*= 1.0f*/, bool bForceIdle/*=false*/ )
{
	if(m_MotionIdleMemory.CheckPushMotion(szMotionName, m_pMotionLogic, bForceIdle) == false)
		m_pMotionLogic->PushMotion(szMotionName, fMotionSpeed, fMotionSppedRatio);
}

// 일단 하나의 애니메이션 인덱스만 속도 조절이 가능하면 되므로 하나만으로 제약함.
void XModuleMotion::SetAnimationTime(int nAnimationIndex, unsigned int nPlayTime)
{
	m_pMotionLogic->SetAnimationTime(nAnimationIndex, nPlayTime);
}

void XModuleMotion::SetSpeed(float fSpeed)
{
	m_pMotionLogic->SetSpeed(fSpeed);
}

float XModuleMotion::GetSpeed()
{
	return m_pMotionLogic->GetSpeed();
}

void XModuleMotion::SetMotionTime(unsigned int nPlayTime)
{
	m_pMotionLogic->SetMotionTime(nPlayTime);
}

float XModuleMotion::GetMotionTime()
{
	return m_pMotionLogic->GetMotionTime();
}

void XModuleMotion::SetPause(bool bPause)
{
	if (bPause)
	{
		m_pModuleEntity->GetAnimationController()->PauseAnimation();
	}
	else
	{
		m_pModuleEntity->GetAnimationController()->PlayAnimation();
	}
}

bool XModuleMotion::IsCurrMotionLoop()
{
	return m_pMotionLogic->IsCurrMotionLoop();
}

bool XModuleMotion::IsCurrMotionPlayDone()
{
	return m_pMotionLogic->IsCurrMotionPlayDone();
}

void XModuleMotion::ChangeWeaponAniType( WEAPON_TYPE nWeaponType )
{
	m_pMotionLogic->ChangeWeaponAniType(nWeaponType);
	m_pUpperMotionLogic->ChangeWeaponAniType(nWeaponType);
}

void XModuleMotion::HoldSequence( bool bHold)
{
	if ( bHold == true)
		m_pMotionLogic->HoldSequence();
	else
		m_pMotionLogic->HoldRelease();
}

bool XModuleMotion::IsHoldingSequence()
{
	return m_pMotionLogic->IsHoldingSequence();
}

void XModuleMotion::SetAnimationNamePostfix( const wchar_t* szPostfix )
{
	m_pMotionLogic->SetAnimationNamePostfix(szPostfix);
	m_pUpperMotionLogic->SetAnimationNamePostfix(szPostfix);
}

void XModuleMotion::ReleaseAnimationNamePostfix()
{
	m_pMotionLogic->ReleaseAnimationNamePostfix();
	m_pUpperMotionLogic->ReleaseAnimationNamePostfix();
}

void XModuleMotion::RefreshThisMotion()
{
	m_pMotionLogic->RefreshThisMotion();
	m_pUpperMotionLogic->RefreshThisMotion();
}

bool XModuleMotion::ChangeUpperMotion( const wchar_t* szMotionName, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false */ )
{
	return m_pUpperMotionLogic->ChangeMotion(szMotionName, nType, bForced);
}

bool XModuleMotion::ChangeUpperMotion( const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false */ )
{
	return m_pUpperMotionLogic->ChangeMotion(szMotionName, szNextMotionName, nType, bForced);
}

void XModuleMotion::ChangeStanceType()
{
	// m_pUpperMotionLogic는 현재 필요가 없어요.
	m_pMotionLogic->ChangeStanceType();
}

void XModuleMotion::ReserveNextMotionSpeed( float fSpeed )
{
	m_pMotionLogic->ReserveNextMotionSpeed(fSpeed);
}

void XModuleMotion::SetUpperSpeed( float fSpeed )
{
	m_pMotionLogic->SetSpeed(fSpeed);
}

void XModuleMotion::Reload()
{
	m_pMotionLogic->Reload();
	m_pUpperMotionLogic->Reload();
}

wstring XModuleMotion::GetCurrUpperMotion()
{
	return m_pUpperMotionLogic->GetCurrMotion();

}

bool XModuleMotion::IsCurrentUpperMotion( wstring szMotionName )
{
	return m_pUpperMotionLogic->IsCurrentMotion(szMotionName);
}

void XModuleMotion::StopUpperMotion()
{
	m_pUpperMotionLogic->StopUpperMotion();
}

bool XModuleMotion::ChangeToCustomUpperMotion( const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced/*=false*/ )
{
	return m_pUpperMotionLogic->ChangeToCustomMotion(szMotionName, szAnimationName, bForced);
}

void XModuleMotion::SetUpperAnimationTime( int nAnimationIndex, unsigned int nPlayTime )
{
	m_pUpperMotionLogic->SetAnimationTime(nAnimationIndex, nPlayTime);
}

float XModuleMotion::GetOrgSpeed()
{
	return m_pMotionLogic->GetOrgSpeed();
}

std::wstring XModuleMotion::GetCustomMotionAniName( const wstring& szMotionName )
{
	return m_pMotionLogic->GetCustomMotionAniName(szMotionName);
}

void XModuleMotion::SetIdleMemoryMotion( int nBuffID, wstring strMotion, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false*/ )
{
	m_MotionIdleMemory.SetIdleMemoryMotion(nBuffID, strMotion, nType, bForced);
}

void XModuleMotion::SetIdleMemoryCustomMotion( int nBuffID, wstring strMotion, wstring strAnimationName, bool bForced/*=false*/ )
{
	m_MotionIdleMemory.SetIdleMemoryCustomMotion(nBuffID, strMotion, strAnimationName, bForced);
}

void XModuleMotion::RemoveIdleMemory(int nBuffID)
{
	m_MotionIdleMemory.End(nBuffID, m_pMotionLogic, (m_pOwner->GetEntityType() == ETID_NPC));
}

void XModuleMotion::RemoveAllIdleMemory()
{
	m_MotionIdleMemory.AllDel();
}

bool XModuleMotion::IsPastDeadPostTrigger()
{
	return m_pMotionLogic->IsPastDeadPostTrigger();
}
