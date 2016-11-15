#include "stdafx.h"
#include "XMyActionNormalAttack.h"
#include "XModuleAction.h"
#include "XModuleMotion.h"
#include "XModuleEntity.h"
#include "XModuleMyControl.h"
#include "XModuleBuff.h"
#include "XEventID.h"
#include "XMyPlayer.h"
#include "XModuleTalent.h"
#include "XController.h"
#include "XSpecialActionTransition.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "CSItemHelper.h"
#include "XStrings.h"
#include "XMyActionIdle.h"
#include "XPost_Action.h"
#include "XCombo.h"

// Normal Attack State ////////////////////////////////////////////////////
XMyActionNormalAttack::XMyActionNormalAttack(XModuleMyControl* pOwner) 
: XMyActionState(pOwner)
, m_AttackType(NA_NA)
, m_nTalentID(0)
, m_pCombo(new XCombo())
{
	
}


XMyActionNormalAttack::~XMyActionNormalAttack()
{
	delete m_pCombo;
}


bool XMyActionNormalAttack::CheckEnterable(void* pParam)
{
	TALENT_NORMAL_ATTACK_TYPE nAttackType = ConvertFromParam( pParam);
	int nTalentID = GetTalentID( nAttackType);

	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsUsableTalent( nTalentID) == false)
		return false;

	// Disabled 버프가 걸린 상태에서는 공격 키 콤보 입력을 받지 않음
	m_pCombo->SetEnable( attrBuff.IsAttackableMelee() ? true : false);


	if ( m_pOwner->GetMyPlayerOwner()->IsDead())
		return false;

	if ( CheckSTA( nTalentID) == false)
	{
		wstring strMsg = XGetStr(L"SMSG_HAVE_NO_STA");
		global.ui->OnPresentationLower(strMsg, PRESENTATION_ICON_EXCLAMATION, true);
		return false;
	}

	if ( nAttackType == NA_SUPER)
	{
		// 방어키로 할당됨
		if ( CSItemHelper::IsGuardableWeaponType( m_pOwner->GetMyPlayerOwner()->GetCurrWeaponType()))
			return false;
	}

	// 브레이커블파츠 무기 사용중이어서 사용 가능한 액션인가?
	if(m_pOwner->UsableActionState_OverlappedEquipmentItem(GetID(), (int)ConvertFromParam(pParam)) == false)
		return false;

	return true;
}

void XMyActionNormalAttack::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	TALENT_NORMAL_ATTACK_TYPE nAttackType = ConvertFromParam(pParam);

	m_nTalentID = 0;
	m_strMotionName = L"";
	m_strReturnMotionName = L"";
	m_pCombo->Init();
	gg.controller->ClearKeyBuffer();

	doAttack(nAttackType);
}

void XMyActionNormalAttack::Exit()
{
	m_pCombo->ReleaseReserved();

	gg.controller->SetFreeLookCamera(false);

	XModuleTalent* pModuleTalent = m_pOwner->GetOwner()->GetModuleTalent();
	if( pModuleTalent)
	{
		pModuleTalent->ForceTerminate();
	}

	// 상체 애니메이션을 종료한다.
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction)
		pModuleAction->StopUpperAni();

	XMyActionState::Exit();
}

void XMyActionNormalAttack::Update(float fDelta)
{
	UpdateCheckCombo();
}

XEventResult XMyActionNormalAttack::Event(XEvent& msg)
{
	if (XMyActionState::Event(msg) == MR_TRUE) return MR_TRUE;

	switch (msg.m_nID)
	{
	case XEVTL_MYACTION_USE_TALENT:
		{
			OnEvent_UseTalent(msg);

			return MR_TRUE;
		}
		break;
	case XEVTL_VIRTUAL_KEY_PRESSED:
		{
			OnEvent_VirtualKeyPressed(msg);
		}
		break;
	case XEVTL_MOTION_PLAYONCE:
		{
			OnEvent_MotionPlayOnce();
			return MR_TRUE;
		}
		break;

	case XEVTL_ON_CAN_CANCEL_ACTION :
		{
			OnEvent_OnCanCancelAction();
			return MR_TRUE;
		}
		break;
	case XEVTL_UPPER_MOTION_PLAYONCE:
		{
			XMyActionIdleParam idleParam;
			idleParam.bSetIdleAnimation = false;
			m_pOwner->DoAction(ACTION_STATE_IDLE, &idleParam);
			return MR_TRUE;
		}
		break;
	}
	return MR_FALSE;
}

bool XMyActionNormalAttack::doAttack(TALENT_NORMAL_ATTACK_TYPE nAttackType)
{
	if (nAttackType == NA_USE_TALENT__CLIENT_ONLY)
	{
		if(m_pOwner->DoAction(ACTION_STATE_USE_TALENT, m_nTalentID) == false)
			return false;

		return true;
	}
	else if (nAttackType == NA_GUARD__CLIENT_ONLY)
	{
		if (m_pOwner->IsCurrentGuardEnabled())
		{
			if(m_pOwner->DoAction(ACTION_STATE_GUARD) == false)
				return false;
		}
		
		return true;
	}

	int nTalentID = GetTalentID( nAttackType);
	if (nTalentID == 0)
	{
		return false;
	}

	if (!CheckSTA(nTalentID))
	{
		return false;
	}

	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsUsableTalent( nTalentID) == false)
		return false;

	m_AttackType = nAttackType;
	m_nTalentID = nTalentID;

	m_pCombo->ReleaseReserved();
	m_pCombo->OnAttack(nAttackType);

	// NormalAttack은 상체 애니메이션을 사용안한다.
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction)
		pModuleAction->StopUpperAni();

	m_pOwner->SyncPlayerDirToCameraDir();

	if (m_pOwner->UseTalent(m_nTalentID))
	{
		SetupMotionNames();
	}

	m_pOwner->GetOwner()->GetModuleAction()->StopMove();

	PostAttack(m_AttackType);
	gg.controller->ClearKeyBuffer();

	m_pOwner->ReleaseCancelAction();

	return true;
}

bool XMyActionNormalAttack::CheckCanReserveCombo( SPECIAL_ACTION_COMMAND nSpecialActionCommand /*= SPECIAL_KEY_NONE*/ )
{
	if (m_pCombo->CheckCanReserveCombo() == false) return false;

	if(CheckCurrentReturnMotion())
		return false;

	return true;
}

int XMyActionNormalAttack::GetTalentID( TALENT_NORMAL_ATTACK_TYPE nAttackType)
{
	int nTalentID = 0;
	WEAPON_TYPE nWeaponType = gvar.MyInfo.EquipmentSlot.GetCurrWeaponType();

	return CSTalentInfoHelper::NormalAttackTalentID( nWeaponType, nAttackType, gvar.MyInfo.EquipmentSlot.IsEquipShield());	
}

bool XMyActionNormalAttack::CheckSTA(int nTalentID)
{
	if (AmIGod()) return true;

	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);

	const int nMySTA = GetMyStatusSTA();

	if (pTalentInfo == NULL || pTalentInfo->m_nSTACost > nMySTA) return false;

	return true;
}

bool XMyActionNormalAttack::OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	//////////////////////////////////////////////////////////////////////////
	// 즉시 사용
	if(ImmediatelyAction(nSpecialActionCommand))
		return true;

	//////////////////////////////////////////////////////////////////////////
	// 콤보
	return  ReserveCombo(nSpecialActionCommand);
}

TALENT_NORMAL_ATTACK_TYPE XMyActionNormalAttack::ConvertFromParam( void* pParam )
{
	TALENT_NORMAL_ATTACK_TYPE ret;
	int nParam = (pParam) ? *(int*)pParam : 0;

	ret = (TALENT_NORMAL_ATTACK_TYPE)nParam;
	if ((ret < 0) || (ret >= NA_MAX))
		ret = NA_NORMAL1;

	return ret;
}


void XMyActionNormalAttack::PostAttack( TALENT_NORMAL_ATTACK_TYPE nAttackType )
{
	// 카메라에서 캐릭터 방향으로 변경
	//vec3 vCameraDir = gg.controller->GetBackViewCameraDir();
	// melee 공격은 캐릭터 방향으로 한다.

	XObject* pOwnerObject = m_pOwner->GetOwner();
	vec3 vAttackDir = pOwnerObject->GetDirection();
	vec3 vPos = pOwnerObject->GetPosition();

	XPostAttackMelee(vPos, vAttackDir, nAttackType);
}


void XMyActionNormalAttack::SetupMotionNames()
{
	XTalentInfo* pTalentInfo = m_pOwner->GetOwner()->GetModuleTalent()->GetInfo();
	m_strMotionName = pTalentInfo->m_szUseAnimation;
	m_strReturnMotionName = pTalentInfo->m_szCastingLoopAnimation;

	wstring t = pTalentInfo->m_szUseAnimation;
	int f = t.find(L";");
	if (f != wstring::npos)
	{
		m_strMotionName = t.substr(0, f);
		m_strReturnMotionName = t.substr(f+1);
	}
	else
		m_strMotionName = t;
}

void XMyActionNormalAttack::UpdateCheckCombo()
{
	XKeyBuffer& KB = gg.controller->GetKeyBuffer();

	TALENT_NORMAL_ATTACK_TYPE nNextAttack = NA_NA;

	if (CheckCanReserveCombo())
	{
		if (KB.IsExistPressedKeyNode(VKEY_ACTION))
		{
			nNextAttack = m_pCombo->GetNextNormalAttack();
		}

		if (m_pOwner->IsCurrentGuardEnabled())
		{
			nNextAttack = NA_GUARD__CLIENT_ONLY;
		}

		if (nNextAttack != NA_NA)
		{
			XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
			if (pAttackChecker->CheckConst(m_AttackType, nNextAttack))
			{
				m_pCombo->Reserve(nNextAttack);

				gg.controller->ClearKeyBuffer();
			}
		}
	}
	else if(CheckCurrentReturnMotion())
	{
		// 리턴중일때 가능한... 일반 공격
		if (KB.IsExistPressedKeyNode(VKEY_ACTION))
		{
			nNextAttack = m_pCombo->GetNextNormalAttack();
		}

		if (nNextAttack != NA_NA)
		{
			XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
			if (pAttackChecker->CheckConst(m_AttackType, nNextAttack))
			{
				m_pCombo->Reserve(nNextAttack);

				doAttack( m_pCombo->GetNextAttackType());
			}
		}
	}
}

bool XMyActionNormalAttack::CheckCanCommandWhenReturnMotion( SPECIAL_ACTION_COMMAND nSpecialActionCommand /*= SPECIAL_KEY_NONE*/, TALENT_NORMAL_ATTACK_TYPE nAttackType /*= NA_NA*/ )
{
	// 방어는 판정 시간 후 항상 가능하다
	if (nSpecialActionCommand == SPECIAL_KEY_GUARD)
	{
		XModuleTalent* pModuleTalent = m_pOwner->GetOwner()->GetModuleTalent();
		if( pModuleTalent && pModuleTalent->GetTalent() && pModuleTalent->GetTalent()->GetInfo())
		{
			XTalentInfo* pTalentInfo = pModuleTalent->GetTalent()->GetInfo();
			float fHitTime = 10.0f;

			size_t nHitSize = pTalentInfo->m_HitInfo.m_vSegments.size();
			if (nHitSize > 0)
			{
				fHitTime = pTalentInfo->m_HitInfo.m_vSegments[nHitSize-1].m_fCheckTime;
			}

			if (pModuleTalent->GetTalent()->GetElapsedTime() >= fHitTime)
			{
				return true;
			}
		}

	}

	// 리턴 애니메이션일 때에는 콤보가 가능하지 않다.
	// 1. 회피나 방어 동작은 가능하다. - Ticket #1537
	if(CheckCurrentReturnMotion())
	{
		if(nSpecialActionCommand == SPECIAL_KEY_DODGE_FRONT ||
			nSpecialActionCommand == SPECIAL_KEY_DODGE_BACK ||
			nSpecialActionCommand == SPECIAL_KEY_DODGE_LEFT ||
			nSpecialActionCommand == SPECIAL_KEY_DODGE_RIGHT ||
			nSpecialActionCommand == SPECIAL_KEY_GUARD ||
			nSpecialActionCommand == SPECIAL_KEY_UPPER ||
			nSpecialActionCommand == SPECIAL_KEY_ADVANCE_ATTACK)
			return true;
	}

	return false;
}

bool XMyActionNormalAttack::ReserveCombo( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	if (CheckCanReserveCombo(nSpecialActionCommand) == false)
	{
		return false;
	}

	m_pCombo->ReleaseReserved();

	XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
	TALENT_NORMAL_ATTACK_TYPE nNextAttackType = pAttackChecker->Trans(nSpecialActionCommand);

	if (pAttackChecker->CheckConst(m_AttackType, nNextAttackType) == false) 
		return false;

	switch (nNextAttackType)
	{
	case NA_DODGE_FRONT:
	case NA_DODGE_BACK:
	case NA_DODGE_LEFT:
	case NA_DODGE_RIGHT:
	case NA_UPPER:
	case NA_ADVANCE:
	case NA_GUARD__CLIENT_ONLY:
		{
			m_pCombo->Reserve(nNextAttackType);

			return true;
		}
		break;
	case NA_SUPER:
		{
			XMyPlayer* pMyPlayer= m_pOwner->GetMyPlayerOwner();

			// 1hs는 SUPER키가 방어키로 할당되었기 때문
			if (pMyPlayer->GetCurrWeaponType() == WEAPON_1H_SLASH)
			{
				return false;
			}

			m_pCombo->Reserve(nNextAttackType);				
			return true;
		}
		break;
	case NA_UPPERCOMBO1:
		{
			if ( m_pOwner->IsCanCancelUpper())
			{
				if (m_AttackType == NA_UPPER)
				{
					m_pCombo->Reserve(nNextAttackType);
				}
			}
			else
			{
				m_pCombo->Reserve(nNextAttackType);
			}
			return true;
		}
		break;
	}

	return false;
}

bool XMyActionNormalAttack::ImmediatelyAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	TALENT_NORMAL_ATTACK_TYPE eAttackType = NA_NA;

	// 리턴 모션일때 사용이 가능한거...
	if(CheckCanCommandWhenReturnMotion(nSpecialActionCommand, eAttackType))
	{
		XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
		TALENT_NORMAL_ATTACK_TYPE nNextAttackType = pAttackChecker->Trans(nSpecialActionCommand);

		// 위에서 선택된 타입이 우선권이다.
		if(eAttackType > NA_NA)
			nNextAttackType = eAttackType;

		if (pAttackChecker->CheckConst(m_AttackType, nNextAttackType) == false) 
			return false;

		switch (nNextAttackType)
		{
		case NA_DODGE_FRONT:
		case NA_DODGE_BACK:
		case NA_DODGE_LEFT:
		case NA_DODGE_RIGHT:
		case NA_GUARD__CLIENT_ONLY:
		case NA_UPPER:
		case NA_ADVANCE:
			{
				if(doAttack(nNextAttackType))
				{
					// 상체 애니메이션을 종료한다.
					XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
					if (pModuleAction)
						pModuleAction->StopUpperAni();
				}

				return true;
			}
			break;
		case NA_SUPER:
			{
				XMyPlayer* pMyPlayer= m_pOwner->GetMyPlayerOwner();

				// 1hs는 SUPER키가 방어키로 할당되었기 때문
				if (pMyPlayer->GetCurrWeaponType() == WEAPON_1H_SLASH)
				{
					return false;
				}

				doAttack(nNextAttackType);				
				return true;
			}
			break;
		case NA_UPPERCOMBO1:
			{
				if ( m_pOwner->IsCanCancelUpper())
				{
					if (m_AttackType == NA_UPPER)
					{
						doAttack(nNextAttackType);
					}
				}
				else
				{
					doAttack(nNextAttackType);
				}
				return true;
			}
		}
	}

	return false;
}

void XMyActionNormalAttack::OnEvent_UseTalent( XEvent &msg )
{
	if (CheckCanReserveCombo())
	{
		m_pCombo->ReleaseReserved();

		XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
		if (pAttackChecker->CheckConst(m_AttackType, NA_USE_TALENT__CLIENT_ONLY))
		{
			m_nTalentID = *(int*)(msg.m_pData);
			m_pCombo->Reserve(NA_USE_TALENT__CLIENT_ONLY);
		}
	}
	else if(CheckCurrentReturnMotion())
	{
		// 리턴 모션일때에는 탤런트 사용 가능
		m_pCombo->ReleaseReserved();

		XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
		if (pAttackChecker->CheckConst(m_AttackType, NA_USE_TALENT__CLIENT_ONLY))
		{
			m_nTalentID = *(int*)(msg.m_pData);

			if(doAttack(NA_USE_TALENT__CLIENT_ONLY))
			{
				// 성공시
				XTalentInfo* pTalentInfo = info.talent->Get(m_nTalentID);
				if(pTalentInfo && pTalentInfo->m_nMovableType == MUT_NONE)
				{
					// 탤런트 사용시 상체 애니메이션이 없다면... 이전 상체 애니메이션 종료
					XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
					if (pModuleAction)
						pModuleAction->StopUpperAni();
					
				}
			}
		}
	}
}

void XMyActionNormalAttack::OnEvent_VirtualKeyPressed( XEvent &msg )
{
	XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
	switch (nVirtualKey)
	{
		//case VKEY_USE_TALENT:
		//	{
		//		m_pCombo->ReleaseReserved();

		//		XKeyBuffer& kb = gg.controller->GetKeyBuffer();
		//		kb.Remove(VKEY_ACTION);
		//		kb.Remove(VKEY_FORWARD);
		//		kb.Remove(VKEY_BACKWARD);
		//		kb.Remove(VKEY_LEFT);
		//		kb.Remove(VKEY_RIGHT);
		//	}
		//	break;
	case VKEY_FORWARD:
	case VKEY_BACKWARD:
	case VKEY_LEFT:
	case VKEY_RIGHT:
		{
			m_pOwner->m_ReserveAttack.Release();
		}
		break;
	}
}

void XMyActionNormalAttack::OnEvent_MotionPlayOnce()
{
	// 자동 모드일 경우
	if ( XGETCFG_GAME_AUTONORMALATTACK  &&  global.ui->IsMouseVisible() == false)
	{
		if ( m_pCombo->IsReserved() == false)
		{
			if ( CheckCanReserveCombo())
			{
				doAutoNormalAttack();
			}
			return;
		}

		else
		{
			if ( CheckCanReserveCombo())
			{
				if(doAttack( m_pCombo->GetNextAttackType()) == false)
				{
					// 콤보가 실패했다면... 오토노멀어택실시
					doAutoNormalAttack();
				}
			}
			return;
		}
	}

	// 수동 모드일 경우
	else if ( m_pCombo->IsReserved())
	{
		if ( CheckCanReserveCombo())
		{
			doAttack( m_pCombo->GetNextAttackType());
		}
	}
}

void XMyActionNormalAttack::OnEvent_OnCanCancelAction()
{
	if ( m_pCombo->CheckReservedTime() )
	{
		doAttack(m_pCombo->GetNextAttackType());
	}
}

int XMyActionNormalAttack::GetMyStatusSTA()
{
	return gvar.MyInfo.ChrStatus.nSTA;
}

bool XMyActionNormalAttack::CheckCurrentReturnMotion()
{
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	if (!pModuleMotion) return false;

	if (pModuleMotion->IsCurrentMotion(m_strReturnMotionName) == true ||
		pModuleMotion->IsCurrentUpperMotion(m_strReturnMotionName) == true) 
		return true;

	return false;
}

void XMyActionNormalAttack::doAutoNormalAttack()
{
	if ( gg.controller->IsVirtualKeyPressedAsync( VKEY_ACTION) == true)
	{
		TALENT_NORMAL_ATTACK_TYPE nNextAttack = m_pCombo->GetNextNormalAttack();
		if ( nNextAttack >= NA_NORMAL1  &&  nNextAttack <= NA_NORMAL6)
		{
			XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
			if (pAttackChecker->CheckConst(m_AttackType, nNextAttack))
			{
				m_pCombo->Reserve( nNextAttack);

				doAttack( m_pCombo->GetNextAttackType());
			}
		}
	}
}

