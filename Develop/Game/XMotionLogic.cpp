#include "stdafx.h"
#include "XMotionLogic.h"
#include "XMotionMgr.h"
#include "XNaming.h"
#include "XGameTransformControllerManager.h"
#include "XNonPlayer.h"
#include "XCutScenePlayer.h"
#include "XPlayer.h"
#include "XMotionTimeModifier.h"
#include "XMotionEventListener.h"
#include "XMotionEventListener__NPC_IDLE.h"
#include "XMotionHelper.h"
#include "XMotionIdleMemory.h"
#include "XModuleEntity.h"

void XMotionLogic::XMotionError::Clear()
{
	m_strAnimationError.clear();
	m_strMotionError.clear();
}

void XMotionLogic::XMotionError::ClearMotionError()
{
	m_strMotionError.clear();
}

void XMotionLogic::XMotionError::ClearAnimationError()
{
	m_strAnimationError.clear();
}

void XMotionLogic::XMotionError::SetMotionError( const wchar_t* szMsg )
{
	m_strMotionError = szMsg;
}

void XMotionLogic::XMotionError::SetMotionError( wstring& strMsg )
{
	m_strMotionError = strMsg;
}

void XMotionLogic::XMotionError::SetAnimationError( const wchar_t* szMsg )
{
	m_strAnimationError = szMsg;
}

void XMotionLogic::XMotionError::SetAnimationError( wstring& strMsg )
{
	m_strAnimationError = strMsg;
}

bool XMotionLogic::XMotionError::IsExistMotionError()
{
	if (m_strMotionError.size() > 0) return true;
	return false;
}

bool XMotionLogic::XMotionError::IsExistAnimationError()
{
	if (m_strAnimationError.size() > 0) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

void XMotionAniNameMaker::MakePlayerAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, WEAPON_TYPE nWeaponType, CHAR_STANCE nStance )
{
	if (pMotion->IsWeaponAni() == false)
	{
		outName = wstring(pMotion->m_vecAniSequence[curr][nAniSequence]->GetAniName());
		return;
	}

	WEAPON_TYPE ani_weapon_type = nWeaponType;

	if (nStance == CS_NORMAL)
	{
		if ((pMotion->GetStanceType() != XMotion::MS_BATTLE) || 
			pMotion->IsWeaponAni() == false)
		{
			ani_weapon_type = WEAPON_NONE;
		}
	}

	if (ani_weapon_type != WEAPON_MAX)
	{
		if (nAniSequence < 0 || nAniSequence >= (int)pMotion->m_vecAniSequence[curr].size()) return;

		outName = wstring(pMotion->m_vecAniSequence[curr][nAniSequence]->GetAniName(ani_weapon_type));
	}
}

void XMotionAniNameMaker::MakeNPCAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, int nCurrNPCMode, XNPCInfo* pNPCInfo )
{
	wstring strPreFix = L"";
	if(pMotion->IsAutoPrefix())
		strPreFix = XNaming::ModeAnimationNameHeader(nCurrNPCMode, pNPCInfo);

	// NPC 전용 idle 애니메이션이 있나?
	if(pMotion->GetName() == MOTION_NAME_NPC_IDLE &&
		pNPCInfo->strIdleAni.empty() == false)
	{
		outName =strPreFix + pNPCInfo->strIdleAni;
		return;
	}

	outName = strPreFix	+ wstring(pMotion->m_vecAniSequence[curr][nAniSequence]->GetAniName());
}

void XMotionAniNameMaker::MakeRideAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, int nCurrNPCMode, XNPCInfo* pNPCInfo )
{
	MakeNPCAniName(outName, pMotion, curr, nAniSequence, nCurrNPCMode, pNPCInfo );
	// ride_ + 
	outName = L"ride_" + outName;
}

void XMotionAniNameMaker::MakeRiderAniName( wstring& outName, XMotion* pMotion, XMotionTypes curr, int nAniSequence, WEAPON_TYPE nWeaponType, CHAR_STANCE nStance )
{
	MakePlayerAniName( outName, pMotion, curr, nAniSequence, nWeaponType, nStance );
	// + "_ride"
	outName	= outName + L"_ride";
}

//////////////////////////////////////////////////////////////////////////
XMotionTrigger_Checker		XMotionLogic::m_TriggerChecker;
XMotionTrigger_Executor		XMotionLogic::m_TriggerExecutor;

XMotionLogic::XMotionLogic( XActor* pOwner, XGameTransformControllerManager* pAnimationController, XMotionTimeModifier* pMotionTimeModifier, XMotionIdleMemory* pMotionIdleMemory )
: m_pOwner(pOwner)
, m_pAnimationController(pAnimationController)
, m_pMotionTimeModifier(pMotionTimeModifier)
, m_pCurrMotion(NULL)
, m_nCurrAniSequence(0)
, m_bPlayDone(false)
, m_nCurrType(MT_DEFAULT)
, m_nForcedType(MT_DEFAULT)
, m_bChanged(false)
, m_nWeaponType(WEAPON_NONE)
, m_bHoldSequence(false)
, m_bUseAniPostfix(false)
, m_fReservedSpeed(0.0f)
, m_pMotionIdleMemory(pMotionIdleMemory)
{
	m_bUseWeaponAni = true;
	if (m_pOwner->GetEntityType() == ETID_NPC) m_bUseWeaponAni = false;
}

XMotionLogic::~XMotionLogic()
{
	SAFE_DELETE(m_pMotionTimeModifier);
}


void XMotionLogic::AddCustomMotionAniName( const wchar_t* szMotionName, const wchar_t* szAnimationName )
{
	for (size_t i=0; i<m_vCustomMotionAniName.size(); i++)
	{
		if (m_vCustomMotionAniName[i].m_strMotion == szMotionName)
		{
			m_vCustomMotionAniName[i].m_strAni = szAnimationName;
			return;
		}
	}
	m_vCustomMotionAniName.push_back(SCustomAniName(szMotionName, szAnimationName));
}

wstring XMotionLogic::GetCustomMotionAniName( const wstring& szMotionName )
{
	for (size_t i=0; i<m_vCustomMotionAniName.size(); i++)
	{
		if (m_vCustomMotionAniName[i].m_strMotion == szMotionName)
		{
			return m_vCustomMotionAniName[i].m_strAni;
		}
	}
	return L"";
}

void XMotionLogic::ResetInfo()
{
	m_bPlayDone = false;
	m_pCurrMotion = NULL;
	m_nCurrAniSequence = 0;
	m_nCurrType = MT_DEFAULT;
	m_strLastEvent = L"";
	m_bHoldSequence = false;
}

XMotionTypes XMotionLogic::GetCurrMotionType()
{
	return m_nCurrType;
}

bool XMotionLogic::IsCameraLook()
{
	if (m_pCurrMotion) 
	{
		if (m_pOwner)
		{
			if (m_pOwner->GetEntityType() == ETID_PLAYER) 
			{
				XPlayer* player = static_cast<XPlayer*>(m_pOwner);

				switch (player->GetStance())
				{
				case CS_NORMAL: return m_pCurrMotion->IsCameraLook(XMotion::MS_NORMAL);
				case CS_BATTLE: return m_pCurrMotion->IsCameraLook(XMotion::MS_BATTLE);
				}
			}
		}

		return m_pCurrMotion->IsCameraLook();
	}
	return true;
}

bool XMotionLogic::IsCurrMotionLoop()
{
	if(m_pCurrMotion)
	{ 
		return m_pCurrMotion->IsLoop(); 
	} 

	return false;
}

bool XMotionLogic::IsCurrMotionPlayDone()
{
	if(m_pCurrMotion)
	{ 
		return m_bPlayDone; 
	} 

	return true;
}

bool XMotionLogic::IsCurrentMotion( wstring szMotionName )
{
	if ( GetCurrMotion() == szMotionName)
	{
		return true;
	}

	return false;
}

void XMotionLogic::SetAnimationTime( int nAnimationIndex, unsigned int nPlayTime )
{
	m_pMotionTimeModifier->SetAnimationTime(nAnimationIndex, nPlayTime, m_nCurrAniSequence);
}

void XMotionLogic::SetSpeed( float fSpeed )
{
	m_pMotionTimeModifier->SetSpeed(fSpeed, m_nCurrAniSequence);
}

float XMotionLogic::GetSpeed()
{
	return m_pMotionTimeModifier->GetSpeed();
}

void XMotionLogic::SetMotionTime( unsigned int nPlayTime )
{
	m_pMotionTimeModifier->SetMotionTime(nPlayTime, m_nCurrAniSequence);
}

float XMotionLogic::GetMotionTime()
{
	return m_pMotionTimeModifier->GetMotionTime();
}

void XMotionLogic::ResetAniTime()
{
	m_pMotionTimeModifier->ResetAniTime();
}

void XMotionLogic::PushMotion( const wchar_t* szMotionName, float fMotionSpeed /*=1.0f*/, float fMotionSppedRatio /*= 1.0f*/ )
{
	SMotionQueueNode new_node;
	new_node.m_strMotionName = szMotionName;
	new_node.m_fSpeed = fMotionSpeed;
	new_node.m_fSpeedRatio = fMotionSppedRatio;

	m_MotionQueue.push_back(new_node);
}

void XMotionLogic::PushCustomMotion( const wchar_t* szMotionName, const wchar_t* szAnimationName, float fMotionSpeed /*=1.0f*/, float fMotionSppedRatio /*= 1.0f*/ )
{
	AddCustomMotionAniName(szMotionName, szAnimationName);

	SMotionQueueNode new_node;
	new_node.m_strMotionName = szMotionName;
	new_node.m_fSpeed = fMotionSpeed;
	new_node.m_fSpeedRatio = fMotionSppedRatio;

	m_MotionQueue.push_back(new_node);
}

//wstring strTest;
//bool bTest = false;
bool XMotionLogic::_ChangeMotion( const wchar_t* szMotionName, XMotionTypes nType, bool bForced, bool bPreFrame /*= false */ )
{
	//if(szMotionName == wstring(L"npc_mf_knockback") )
	//{
	//	bTest = true;
	//}
	//else if(szMotionName == wstring(L"npc_idle") && bTest)
	//{
	//	int a;
	//	a = 0;

	//	bTest = false;
	//}
	//else if(bTest)
	//	bTest = false;

	//if(m_pOwner->GetUID() == XGetMyUID())
	//	mlog("모션 %s\n", MLocale::ConvUTF16ToAnsi(szMotionName).c_str());

	m_Error.Clear();

	m_bChanged = true;
	ResetAniTime();

	if (m_pCurrMotion)
	{
		if ((bForced == false) && 
			(m_bPlayDone == false) && 
			(m_pCurrMotion->IsCancelEnabled() == false) ) 
		{
			return false;
		}
	}

	XMotion* pNextMotion = info.motion->GetMotion(szMotionName);
	if (pNextMotion == NULL) 
	{
		m_Error.SetMotionError(szMotionName);

		return false;
	}

	if (!bForced && (pNextMotion == m_pCurrMotion) && (nType == m_nCurrType)) 
	{
		return true;	// 이미 해당 모션이면 변경없이 true로 반환한다.
	}

	XMotion* pPreEnterRet = OnPreEnter(pNextMotion);
	if (pPreEnterRet)
	{
		if (!bForced && (pNextMotion == m_pCurrMotion) && (nType == m_nCurrType)) 
		{
			return true;	// 이미 해당 모션이면 변경없이 true로 반환한다.
		}
		pNextMotion = pPreEnterRet;
	}

	XMotion* pPrevMotion = m_pCurrMotion;

	ResetInfo();

	m_pCurrMotion = pNextMotion;
	m_nCurrType = nType;

	if (m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty()) m_nCurrType = MT_DEFAULT;

	m_pMotionTimeModifier->CalcMotionLength(m_pCurrMotion, m_nCurrType, m_bUseWeaponAni, m_nWeaponType);

	if (SetAnimation(bForced, bPreFrame) == false)
	{
		return false;
	}

	SendEvent(XEVTL_ON_CHANGE_MOTION);

	if ( XMotionHelper::IsIdleMotion( GetCurrMotion() ))
	{
		SendEvent(XEVTL_ON_CHANGED_MOTION_TO_IDLE);
	}

	OnLeave(pPrevMotion);
	OnEnter(m_pCurrMotion);

	return true;
}

bool XMotionLogic::_SetRideAnimation( bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	bool ret = false;
	m_Error.ClearAnimationError();

	XMotionTypes currMotionType = GetCurrMotionType();

	if( NULL == m_pCurrMotion )
		return false;

	// 탈것 AnimationControllor
	XGameTransformControllerManager* pRideAnimationController	= m_pOwner->GetModuleEntity()->GetRideAnimationController();
	if( NULL == pRideAnimationController )
		return false;

	// 탈것 Ani처리
	wstring strCurrRideAni;
	wstring strCurrCharAni;
	MakeRideAniString( strCurrRideAni, strCurrCharAni );
	if( false == pRideAnimationController->SetAnimation( strCurrRideAni, bForced, bPreFrame ) )
	{
		// 애니메이션 에러
		m_Error.SetAnimationError( L"Ride :" + strCurrRideAni );
		return false;
	}

	// 캐릭터 처리
	if( false == m_pAnimationController->SetAnimation( strCurrCharAni, bForced, bPreFrame ) )
	{
		m_Error.SetAnimationError( L"Char" + strCurrCharAni );
	}

	return true;
}

bool XMotionLogic::_SetAnimation( bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	bool ret = false;
	m_Error.ClearAnimationError();

	XMotionTypes currMotionType = GetCurrMotionType();

	if (m_pCurrMotion)
	{
		if (!m_pCurrMotion->m_vecAniSequence[currMotionType].empty())
		{
			wstring strCurrAni;
			MakeCurrAniString(strCurrAni, currMotionType);

			if (m_bUseAniPostfix && m_pCurrMotion->IsAddPostfix())
			{
				wstring strCurrAniWithPostfix = strCurrAni + m_strAniPostfix;
				ret = m_pAnimationController->SetAnimation(strCurrAniWithPostfix, bForced, bPreFrame);
			}

			if (ret == false)
			{
				// postfix붙여서 실패이면 postfix를 떼고 애니메이션 변환
				ret = m_pAnimationController->SetAnimation(strCurrAni, bForced, bPreFrame);
			}

			if(ret == false)
			{
				// 실패인가? 그렇다면 애니메이션에 무기를 설정하지 않고 해본다.
				MakeCurrAniStringNoWeapon(strCurrAni, currMotionType);
				ret = m_pAnimationController->SetAnimation(strCurrAni, bForced, bPreFrame);
			}

			// 씨퀀스에 속도가 설정되어 있으면 그 속도로 바꿔준다.
			if (ret)
			{
				if (m_fReservedSpeed != 0.0f)
				{
					m_pAnimationController->SetAnimationSpeed(m_fReservedSpeed);
					m_fReservedSpeed = 0.0f;
				}
				else if (m_pCurrMotion->m_vecAniSequence[currMotionType][m_nCurrAniSequence]->GetSpeed() != 1.0f)
				{
					m_pAnimationController->SetAnimationSpeed(m_pCurrMotion->m_vecAniSequence[currMotionType][m_nCurrAniSequence]->GetSpeed());
				}
			}
			else
			{
				// 애니메이션 에러
				m_Error.SetAnimationError(strCurrAni);
			}
		}
		else	
		{
			// 만약 sequence가 하나도 없으면 custom 모션일 것이다.
			if (m_pCurrMotion->IsCustom())
			{
				wstring strCurrAni = GetCustomMotionAniName(m_pCurrMotion->GetName());

				ret = m_pAnimationController->SetAnimation(strCurrAni, bForced, bPreFrame);

				if(ret == false)
				{
					// 애니메이션 에러
					m_Error.SetAnimationError(strCurrAni);
				}
			}
		}
	}


	return ret;
}

bool XMotionLogic::SetAnimation( bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	if( ETID_PLAYER == m_pOwner->GetEntityType() )
	{
		XPlayer* pPlayer	= m_pOwner->AsPlayer();
		if( ( pPlayer ) && ( true == pPlayer->IsRide() ) )
			return _SetRideAnimation( bForced, bPreFrame );
	}
	
	return _SetAnimation( bForced, bPreFrame );
}

void XMotionLogic::MakeCurrAniString( wstring& strOut, XMotionTypes curr )
{
	XMotionAniNameMaker aniNameMaker;

	if (m_pOwner)
	{
		if (m_pOwner->GetEntityType() == ETID_PLAYER) 
		{
			XPlayer* player = static_cast<XPlayer*>(m_pOwner);

			aniNameMaker.MakePlayerAniName(strOut, 
				m_pCurrMotion, 
				curr,
				m_nCurrAniSequence, 
				m_nWeaponType, 
				player->GetStance());
		}
		else if (m_pOwner->GetEntityType() == ETID_NPC)
		{
			XNonPlayer* pNPC = m_pOwner->AsNPC();
			aniNameMaker.MakeNPCAniName(strOut, 
				m_pCurrMotion, 
				curr,
				m_nCurrAniSequence, 
				pNPC->GetMode(),
				pNPC->GetInfo(true));
		}
		else if (m_pOwner->GetEntityType() == ETID_CUTSCENE)
		{
			XCutScenePlayer* pCutScenePlayer = m_pOwner->AsCutScenePlayer();
			if(pCutScenePlayer->IsPlayer() == true)
			{
				aniNameMaker.MakePlayerAniName(strOut, 
					m_pCurrMotion, 
					curr,
					m_nCurrAniSequence, 
					pCutScenePlayer->GetCurrWeaponType(), 
					pCutScenePlayer->GetStance());
			}
			else
			{
				aniNameMaker.MakeNPCAniName(strOut, 
					m_pCurrMotion, 
					curr,
					m_nCurrAniSequence, 
					pCutScenePlayer->GetMode(),
					pCutScenePlayer->GetInfo(true));
			}
			
		}
	}
}

void XMotionLogic::MakeCurrAniStringNoWeapon( wstring& strOut, XMotionTypes curr )
{
	XMotionAniNameMaker aniNameMaker;

	if (m_pOwner)
	{
		// 플레이어만 한다.
		if (m_pOwner->GetEntityType() == ETID_PLAYER) 
		{
			XPlayer* player = static_cast<XPlayer*>(m_pOwner);

			aniNameMaker.MakePlayerAniName(strOut, 
				m_pCurrMotion, 
				curr,
				m_nCurrAniSequence, 
				WEAPON_NONE, 
				player->GetStance());
		}
	}
}

void XMotionLogic::AnimationError()
{
	// 로그로 애니메이션 에러를 남긴다.
	if(m_Error.IsExistAnimationError())
	{
		bool ret = false;

		if (m_pOwner)
		{
			// 애니메이션을 IDLE로 변경한다.
			if (m_pOwner->GetEntityType() == ETID_PLAYER)
			{
				dlog("애니메이션 에러 %s\n", MLocale::ConvUTF16ToAnsi(m_Error.GetAnimaionError().c_str()).c_str());

				if(m_pMotionIdleMemory)
					ret = m_pMotionIdleMemory->CheckChangeMotion(MOTION_NAME_IDLE, this, false);
				
				if(ret == false)
					ret = _ChangeMotion(MOTION_NAME_IDLE, MT_DEFAULT, false);
			}
			else if (m_pOwner->GetEntityType() == ETID_NPC)
			{
				if(m_pMotionIdleMemory)
					ret = m_pMotionIdleMemory->CheckChangeMotion(MOTION_NAME_NPC_IDLE, this, false);

				if(ret == false)
					ret = _ChangeMotion(MOTION_NAME_NPC_IDLE, MT_DEFAULT, false);
			}

			if(ret)
			{
				m_MotionQueue.clear();
			}
		}

		m_Error.Clear();
	}
}

bool XMotionLogic::ChangeToCustomMotion( const wchar_t* szMotionName, const wchar_t* szAnimationName, bool bForced, bool bPreFrame /*= false*/ )
{
	m_vCustomMotionAniName.clear();

	AddCustomMotionAniName(szMotionName, szAnimationName);

	bool ret = _ChangeMotion(szMotionName, MT_DEFAULT, bForced, bPreFrame);
	if(ret)
	{
		m_MotionQueue.clear();
	}
	else
	{
		AnimationError();
	}

	return ret;
}

bool XMotionLogic::ChangeMotion( const wchar_t* szMotionName, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	bool ret = _ChangeMotion(szMotionName, nType, bForced, bPreFrame);
	if(ret)
	{
		m_MotionQueue.clear();
	}
	else
	{
		AnimationError();
	}

	return ret;
}

bool XMotionLogic::ChangeMotion( const wchar_t* szMotionName, const wchar_t* szNextMotionName, XMotionTypes nType/*=MT_DEFAULT*/, bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	bool ret = _ChangeMotion(szMotionName, nType, bForced, bPreFrame);
	if (ret)
	{
		m_MotionQueue.clear();
		PushMotion(szNextMotionName);
	}
	else
	{
		AnimationError();
	}

	return ret;
}

void XMotionLogic::SetSequence( bool bForce )
{
	if(SetAnimation(bForce))
	{
		// 바뀌었다면... 메시지를 보낸다.
		SendEvent(XEVTL_ON_CHANGE_ANIMATION);
	}

	if (m_strLastEvent != L"")
	{
		OnEvent(m_strLastEvent.c_str());
	}

	m_pMotionTimeModifier->SetAnimationSpeed(m_nCurrAniSequence);
}

void XMotionLogic::NextSequence( int nNextSeqIndex )
{
	if (m_bHoldSequence) return;

	int nPreSeqIndex = nNextSeqIndex - 1;

	const int nMaxSeq = max(0, (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size() - 1);

	if (nPreSeqIndex > nMaxSeq ||
		nNextSeqIndex > nMaxSeq)
	{
		if (m_pCurrMotion->IsLoop() == false)
		{
			if (CheckPostTrigger(nPreSeqIndex))
			{
				return;
			}

			PlayDone();
		}
	}
	else
	{
		if (CheckPostTrigger(nPreSeqIndex))
		{
			return;
		}

		// 다음 시퀀스에서도 같은 애니메이션 이름이면 강제로 재생이 되게 한다.
		bool bForce = CheckDuplicationNextSequenceAnimation(nNextSeqIndex);
		m_nCurrAniSequence = nNextSeqIndex;
		SetSequence(bForce);
	}
}

void XMotionLogic::PlayDone()
{
	if (m_bPlayDone == true) return;

	m_bPlayDone = true;

	SendEvent(XEVTL_MOTION_PLAYONCE);

	PlayNextMotion();
}

void XMotionLogic::OnEvent( const wchar_t* szEvent )
{
	if (m_pCurrMotion==NULL) return;

	int nSeqSize = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();
	if (m_nCurrAniSequence < nSeqSize)
	{
		// 다음에 나올 씨퀀스중에 해당 이벤트가 있으면 그 다음 씨퀀스로 이동
		for (int nSeqIndex = m_nCurrAniSequence; nSeqIndex < nSeqSize; nSeqIndex++)
		{
			if (m_pCurrMotion->m_vecAniSequence[m_nCurrType][nSeqIndex]->IsExistEvent())
			{
				if (!_wcsicmp(m_pCurrMotion->m_vecAniSequence[m_nCurrType][nSeqIndex]->GetEventName(), szEvent))
				{
					NextSequence(nSeqIndex+1);

					m_strLastEvent = L"";
					return;
				}
			}
		}

		m_strLastEvent = szEvent;
	}
}

void XMotionLogic::Update( float fDelta )
{
	if (m_pCurrMotion == NULL) return;

	OnUpdate(m_pCurrMotion);

	// TODO: Custom 모션에 대한 update는 아직 고려하지 않았다.
	// 스킬 만들 때 만들어 줘야 함. loop 등 처리 해줘야 한다.
	//if ((m_MotionLogic.m_pCurrMotion) && (m_MotionLogic.m_pCurrMotion->IsCustom())) return;

	m_bChanged = false;

	XGameTransformControllerManager* pAnimationController = m_pAnimationController;
	// 탈것을 탔을경우 탈것을 대상으로
	if( ETID_PLAYER == m_pOwner->GetEntityType() )
	{
		XPlayer* pPlayer	= m_pOwner->AsPlayer();
		if( ( pPlayer ) && ( true == pPlayer->IsRide() ) )
			pAnimationController			= m_pOwner->GetModuleEntity()->GetRideAnimationController();
	}

	int nSeqSize = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();

	if (!m_pCurrMotion->IsCustom())
	{
		if (!m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->IsExistEvent())
		{
			if (m_nCurrAniSequence < nSeqSize-1)
			{
				const float fTimeOut = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetTimeOut();
				const int nFrameOver = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetFrameOver();

				bool bNextSeq = false;

				if (fTimeOut <= 0.0f)
				{
					if ((pAnimationController->IsOncePlayDone()) || 
						(nFrameOver > 0 && pAnimationController->GetAniFrame() >= nFrameOver))
					{
						bNextSeq = true;
					}
				}
				else
				{
					if (pAnimationController->GetElapsedTime() >= fTimeOut)
					{
						bNextSeq = true;
					}
				}

				if (bNextSeq)
				{
					NextSequence(m_nCurrAniSequence+1);
					return;
				}
			}
		}
	}

	if (m_nCurrAniSequence >= nSeqSize-1 &&
		m_pCurrMotion->IsLoop() == false && 
		pAnimationController->IsAnimationLoop() == false &&
		pAnimationController->IsOncePlayDone() && 
		m_bHoldSequence == false)
	{
		if (CheckPostTrigger(m_nCurrAniSequence))
		{
			return;
		}

		PlayDone();
	}
}

wstring XMotionLogic::GetCurrMotion()
{
	if (m_pCurrMotion) return m_pCurrMotion->GetName();
	return wstring();
}

wstring XMotionLogic::GetCurrSequence()
{
	if (m_pCurrMotion &&
		m_nCurrType < MT_COUNT &&
		m_nCurrAniSequence < (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size() &&
		m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence] != NULL)
	{
		return wstring(m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName());
	}

	return wstring();
}

void XMotionLogic::SendEvent( XEventID nEventID )
{
	if (m_pOwner == NULL) return;

	XEvent msg;
	msg.m_nID = nEventID;
	m_pOwner->Event(msg);
}

void XMotionLogic::HoldSequence()
{
	m_bHoldSequence = true;
}

void XMotionLogic::HoldRelease()
{
	m_bHoldSequence = false;
}

bool XMotionLogic::CheckPostTrigger(int nSeqIndex)
{
	if (m_pCurrMotion && m_pCurrMotion->IsExistTrigger())
	{
		XMotionAniSequence* pSeq = m_pCurrMotion->m_vecAniSequence[m_nCurrType][nSeqIndex];
		if (pSeq->GetPostTrigger()->IsValid())
		{
			XMotionTrigger_IfType nIfType = pSeq->GetPostTrigger()->m_nIfType;
			if (m_TriggerChecker.Check(nIfType, m_pOwner))
			{
				XMotionTrigger_ThenType nThenType = pSeq->GetPostTrigger()->m_nThenType;
				m_TriggerExecutor.Execute(nThenType, 
					pSeq->GetPostTrigger()->m_strThenParam1,
					pSeq->GetPostTrigger()->m_strThenParam2,
					this);

				return true;
			}
		}
	}

	return false;
}

void XMotionLogic::ChangeWeaponAniType( WEAPON_TYPE nWeaponType )
{
	if (m_nWeaponType == nWeaponType) return;

	m_nWeaponType = nWeaponType;

	if (m_pCurrMotion && m_pCurrMotion->IsWeaponAni() && m_pCurrMotion->IsLoop())
	{
		XMotionTypes currMotionType = GetCurrMotionType();

		wstring strCurrAni = m_pAnimationController->GetAnimationName();

		wstring strNextAni;
		MakeCurrAniString(strNextAni, currMotionType);

		if (strCurrAni != strNextAni)
		{
			ChangeMotion(GetCurrMotion().c_str(), currMotionType, true);
		}
	}

}

void XMotionLogic::SetAnimationNamePostfix( const wchar_t* szPostfix )
{
	m_bUseAniPostfix = true;
	m_strAniPostfix = szPostfix;
}

void XMotionLogic::ReleaseAnimationNamePostfix()
{
	m_bUseAniPostfix = false;
	m_strAniPostfix = L"";
}

void XMotionLogic::RefreshThisMotion()
{
	SetAnimation(false);
}

XMotion* XMotionLogic::OnPreEnter( XMotion* pMotion )
{
	if (pMotion && pMotion->GetEventListener())
	{
		XMotionEventListener* pListener = static_cast<XMotionEventListener*>(pMotion->GetEventListener());
		return pListener->OnPreEnter(m_pOwner);
	}

	return NULL;
}

void XMotionLogic::OnEnter( XMotion* pMotion )
{
	if (pMotion && pMotion->GetEventListener())
	{
		XMotionEventListener* pListener = static_cast<XMotionEventListener*>(pMotion->GetEventListener());
		return pListener->OnEnter(m_pOwner);
	}
}

void XMotionLogic::OnLeave( XMotion* pMotion )
{
	if (pMotion && pMotion->GetEventListener())
	{
		XMotionEventListener* pListener = static_cast<XMotionEventListener*>(pMotion->GetEventListener());
		return pListener->OnLeave(m_pOwner);
	}
}

void XMotionLogic::OnUpdate( XMotion* pMotion )
{
	if (pMotion && pMotion->GetEventListener())
	{
		XMotionEventListener* pListener = static_cast<XMotionEventListener*>(pMotion->GetEventListener());
		return pListener->OnUpdate(m_pOwner);
	}
}

void XMotionLogic::BindMotionEvents()
{
	XMotion* pMotion = info.motion->GetMotion(MOTION_NAME_NPC_IDLE);
	if (pMotion)
	{
		pMotion->SetEventListener(new XMotionEventLister_NPC_IDLE());
	}
}

void XMotionLogic::ChangeStanceType()
{
	XMotionTypes currMotionType = GetCurrMotionType();

	wstring strCurrAni = m_pAnimationController->GetAnimationName();

	wstring strNextAni;
	MakeCurrAniString(strNextAni, currMotionType);

	if (strCurrAni != strNextAni)
	{
		ChangeMotion(GetCurrMotion().c_str(), currMotionType, true, true);
	}
}

void XMotionLogic::PlayNextMotion()
{
	// 만약 큐에 다음 모션이 있으면 바로 바꿔준다.
	if ((!m_bChanged) && (!m_MotionQueue.empty()))
	{
		SMotionQueueNode node = m_MotionQueue.front();
		m_MotionQueue.pop_front();
		if ( _ChangeMotion(node.m_strMotionName.c_str(), m_nCurrType, false))
		{
			if ( node.m_fSpeed != 1.0f)
				SetSpeed( node.m_fSpeed);
			
			if(node.m_fSpeedRatio != 1.0f)
				SetSpeed( GetSpeed() * node.m_fSpeedRatio);
		}
		else
		{
			AnimationError();
		}
	}
}

void XMotionLogic::ReserveNextMotionSpeed( float fSpeed )
{
	m_fReservedSpeed = fSpeed;
}

bool XMotionLogic::CheckDuplicationNextSequenceAnimation( int nNextSeqIndex )
{
	wstring strCurAniName = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName();
	wstring strNextAniName = m_pCurrMotion->m_vecAniSequence[m_nCurrType][nNextSeqIndex]->GetAniName();

	if(strCurAniName == strNextAniName)
		return true;

	return false;
}

void XMotionLogic::Reload()
{
	m_pCurrMotion = NULL;
}

float XMotionLogic::GetOrgSpeed()
{
	return m_pMotionTimeModifier->GetOrgSpeed();
}

bool XMotionLogic::ChangeMotionQueueInfo( wstring strMotionName, wstring strAnimationName, wstring strChangeMotionName, wstring strChangeAnimationName )
{
	bool bChange = false;
	for(list<SMotionQueueNode>::iterator it = m_MotionQueue.begin(); it != m_MotionQueue.end(); ++it)
	{
		if(it->m_strMotionName == strMotionName)
		{
			vector<SCustomAniName>::iterator itAni = m_vCustomMotionAniName.begin();
			while(itAni != m_vCustomMotionAniName.end())
			{
				if (itAni->m_strMotion == strMotionName)
				{
					itAni->m_strMotion = strChangeMotionName;
					itAni->m_strAni = strChangeAnimationName;

					if(strChangeAnimationName.empty())
					{
						itAni = m_vCustomMotionAniName.erase(itAni);
						continue;
					}
				}

				++itAni;
			}

			it->m_strMotionName = strChangeMotionName;
			bChange = true;
		}
	}

	return bChange;
}

bool XMotionLogic::IsPastDeadPostTrigger()
{
	// 죽었을 처리하는 시퀀스를 지났는지 체크
	if (m_pCurrMotion &&
		m_nCurrType < MT_COUNT &&
		m_nCurrAniSequence < (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size() &&
		m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence] != NULL)
	{
		XMotionAniSequence* pCurrAniSeq = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence];

		int nCount = 0;
		int nDeadIndex = -1;
		for(vector<XMotionAniSequence*>::iterator it = m_pCurrMotion->m_vecAniSequence[m_nCurrType].begin(); it != m_pCurrMotion->m_vecAniSequence[m_nCurrType].end(); ++it)
		{
			if((*it)->GetPostTrigger()->m_nIfType == MTRIGGER_IF_DEAD)
			{
				nDeadIndex = nCount;
			}

			++nCount;
		}

		if(nDeadIndex < m_nCurrAniSequence ||
			nDeadIndex == -1)
			return true;
	}

	return false;
}

void XMotionLogic::MakeRideAniString( wstring& strRideOut, wstring& strCharOut )
{
	XMotionTypes currMotionType = GetCurrMotionType();
		
	XPlayer* pPlayer = m_pOwner->AsPlayer();
	if( NULL == pPlayer )
		return ;

	XMotionAniNameMaker aniNameMaker;
	XNPCInfo* Info	= info.npc->Get( pPlayer->GetRideInfo()->GetRideNpcID() ); 

	// Ride Obj
	aniNameMaker.MakeRideAniName(strRideOut, m_pCurrMotion, currMotionType, m_nCurrAniSequence, 0, Info );

	// Char Obj
	aniNameMaker.MakeRiderAniName( strCharOut, m_pCurrMotion, currMotionType, m_nCurrAniSequence, m_nWeaponType, pPlayer->GetStance() );
}