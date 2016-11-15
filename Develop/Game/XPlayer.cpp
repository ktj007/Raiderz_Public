#include "stdafx.h"
#include "XPlayer.h"
#include "XModuleActorControl.h"
#include "XModuleNetControl.h"
#include "XModuleAction.h"
#include "XModuleMovable.h"
#include "XModuleEffect.h"
#include "XModuleEntity.h"
#include "XModulePlayerData.h"
#include "XConst.h"
#include "CSChrInfo.h"
#include "XCalculator.h"
#include "XPlayerPVP.h"
#include "XDuel.h"
#include "XBuffAttribute.h"
#include "XModuleBuff.h"

MImplementRTTI(XPlayer, XActor);

XPlayer::XPlayer(MUID uid, UIID nUIID) : XActor(uid, nUIID), m_nStance(CS_NORMAL), m_bSwim(false), m_bSitting(false), m_bCutScene(false), m_bAFK(false)
{
	m_pModulePlayerData = new XModulePlayerData(this);

	m_pPlayerPVP = new XPlayerPVP();
	m_pDuel = new XDuel();
}

XPlayer::~XPlayer()
{
	SAFE_DELETE(m_pDuel);
	SAFE_DELETE(m_pPlayerPVP);
	SAFE_DELETE(m_pModulePlayerData);
}

void XPlayer::RegisterModules()
{
	XActor::RegisterModules();

	m_pModulePlayerData->Initialize();
	m_Modules.AddModule(m_pModulePlayerData, true);
}

void XPlayer::UnregisterModules()
{
	m_Modules.RemoveModule(m_pModulePlayerData);
	m_pModulePlayerData->Finalize();

	XActor::UnregisterModules();
}

void XPlayer::ChangeStance(CHAR_STANCE nStance, bool bQuick)
{
	if(m_nStance == nStance)
		return;

	m_nStance = nStance;

	XModuleMovable* pModuleMovable = GetModuleMovable();
	if(pModuleMovable) pModuleMovable->TriggerStance();

	XModuleNetControl* pModuleNetControl = GetModuleNetControl();
	if (pModuleNetControl) pModuleNetControl->OnChangeStance(nStance);

	XModuleAction* pModuleAction = GetModuleAction();
	if (pModuleAction) pModuleAction->ChangeStance(nStance, bQuick);
}

float XPlayer::GetMoveSpeedDefault(unsigned int nMovementFlags)
{
	XCalculator calc;
	bool bMoveBackward = (nMovementFlags & MOVEMENTFLAG_BACKWARD) > 0;
	float fSpeed = calc.CalcPlayerMoveSpeed(bMoveBackward, m_nStance, GetCurrWeaponType());

	// 수영중이라면 무기와 관계없이 Normal에서 2/3 속도
	if(m_bSwim)
	{
		fSpeed = fSpeed * (2.0f / 3.0f);
	}

	return fSpeed;
}

float XPlayer::GetMoveSpeed(unsigned int nMovementFlags)
{
	// 버프에 의한 속도를 구함
	XBuffAttribute attrBuff = m_pModuleBuff->GetBuffAttribute();
	float fModifier = attrBuff.m_fMoveSpeedRatio;
	fModifier = max(fModifier, 0.0f);

	float fSpeed = 100.0f;
	if (attrBuff.m_bMounted)
	{
		fSpeed = XCONST::MOVE_SPEED_NONE;

		if (!gvar.Game.IsCombatingNow())
		{
			fSpeed = fSpeed * fModifier;
		}
	}
	else
	{
		fSpeed = GetMoveSpeedDefault(nMovementFlags);
		fSpeed = fSpeed * fModifier;
	}
		
	return fSpeed;
}

bool XPlayer::IsCurrStanceBattle()
{
	if (GetStance() == CS_BATTLE) return true;
	return false;
}

void XPlayer::ChangeWeapon( CHAR_STANCE nSwitching, bool bQuick )
{	
	m_nStance = nSwitching;

	XModuleNetControl* pModuleNetControl = GetModuleNetControl();
	if (pModuleNetControl) pModuleNetControl->OnChangeStance(CS_BATTLE);

	XModuleAction* pModuleAction = GetModuleAction();
	if (pModuleAction) pModuleAction->ChangeWeapon(nSwitching, bQuick);
}

bool XPlayer::OnCreate()
{
	// 사용 할 이펙트 종류 등록
	if(m_pModuleEffect)
	{
		m_pModuleEffect->SetRegisterUseEffectID((1 << XEFT_ID_HEAD) | (1 << XEFT_ID_BUFF) | ( 1<< XEFT_ID_NPCICON) | (1 << XEFT_ID_TALENT) | (1 << XEFT_ID_SWORDTRAIL));
	}

	// 페이스 애니메이션 등록
	if(m_pModuleEntity)
	{
		m_pModuleEntity->CreateFaceAnimation();
	}

	return true;
}

void XPlayer::SetBattleArenaEnemy()
{
	m_pPlayerPVP->SetBattleArenaEnemy();
}

bool XPlayer::IsAlly( XPlayer* pTargetPlayer )
{
	if (pTargetPlayer && (m_pDuel->IsOpponent(((XObject*)pTargetPlayer)) == false))
	{
		return m_pPlayerPVP->IsAlly(pTargetPlayer->m_pPlayerPVP);
	}

	return false;
}

bool XPlayer::IsEnemy( XPlayer* pTargetPlayer )
{
	return !IsAlly(pTargetPlayer);
}

//bool XPlayer::IsAllyInPvPArea( XPlayer* pTargetPlayer )
//{
//	return m_pPlayerPVP->IsAllyInPvPArea(pTargetPlayer->m_pPlayerPVP);
//}

void XPlayer::SetDuel( bool bDuel, MUID OpponentUID)
{
	if (bDuel)
	{
		if(m_pDuel->IsOpponent(OpponentUID) == false)
			m_pDuel->SetOpponentUID(OpponentUID);
		//m_pPlayerPVP->SetDuel();
	}
	else
	{
		m_pDuel->DuelClear();
		//m_pPlayerPVP->ReleaseDuel();
	}
}

bool XPlayer::IsCollidableTo( XPlayer* pTarPlayer )
{
	// pvp일 경우에는 충돌 체크를 한다.
	if (IsAlly(pTarPlayer) == false)
	{
		return true;
	}

	// Battle Arena에 있으면 무조건 충돌 체크함
	if (gvar.MyInfo.BattleArenaInfo.IsInBattleArena()) return true;

	return false;
}

void XPlayer::SetCutScene( bool bEnterCutScene )
{
	m_bCutScene = bEnterCutScene;

	if(m_bCutScene)
	{
		// 컷씬 시작
		if(m_pModuleEffect)
		{
			m_pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_PLAYER_ICON, 1);
		}

		// 앉아있으면... 푼다.
		XEvent msg(XEVTL_SIT_RISE);
		Event(msg);
	}
	else 
	{
		// 컷씬 종료
		if(m_pModuleEffect)
		{
			m_pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_PLAYER_ICON, 0);
		}
	}
}

bool XPlayer::IsDuel()
{
	return m_pDuel->IsDuel();
}

void XPlayer::RemoveOpponentUID( MUID OpponentUID )
{
	m_pDuel->RemoveOpponentUID(OpponentUID);
}

const wstring XPlayer::GetCaptionName()
{
	wstring strName = GetName();
	if (IsAFK())
	{
		strName += L"<AFK>";
	}

	return strName;
}

void XPlayer::SetOpponentUIDs(const vector<MUID>& vecUIDs)
{
	for each (MUID each in vecUIDs)
	{
		if(m_pDuel->IsOpponent(each) == false)
			m_pDuel->SetOpponentUID(each);
	}
}

bool XPlayer::IsUsableBreakablePartsWeapon()
{
	return GetModuleActorControl()->IsOverlappedEquipmentControllerWork();
}

void XPlayer::EquipOverlapped( int nItemID )
{
	GetModuleActorControl()->StartOverlappedEquipmentController(nItemID);
}

void XPlayer::UnEquipOverlapped( int nItemID )
{
	GetModuleActorControl()->EndOverlappedEquipmentController(nItemID);
}

void XPlayer::SetRide( int nRideID )
{
	if( nRideID != 0 ) //Bad Ride
	{
		m_RideInfo.SetRideNpcID( nRideID );
		m_pModuleEntity->CreateRide();
	}
}

void XPlayer::RemoveRide()
{
	m_RideInfo.SetRideNpcID(0);
	m_pModuleEntity->DeleteRide();
}
