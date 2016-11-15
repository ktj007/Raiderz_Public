#include "stdafx.h"
#include "XModuleMyControl.h"
#include "XModuleMovable.h"
#include "XModuleEntity.h"
#include "XModuleAction.h"
#include "XModuleTalent.h"
#include "XModuleCollision.h"
#include "XModuleEffect.h"
#include "XModulePost.h"
#include "XModuleBuff.h"
#include "XModuleMessageQueue.h"
#include "XMyPlayer.h"
#include "XController.h"
#include "XEventID.h"
#include "XGame.h"
#include "XWorld.h"
#include "CSItemHelper.h"
#include "XActionKeyDef.h"
#include "XSpecialActionTransition.h"
#include "XMyActionNormalAttack.h"
#include "XMyActionBow.h"
#include "XMyActionGuard.h"
#include "XMyActionHit.h"
#include "XMyActionUseTalent.h"
#include "XMyActionIdle.h"
#include "XMyActionJump.h"
#include "XMyActionFalling.h"
#include "XMyActionChangeState.h"
#include "XMyActionSwitchingWeapon.h"
#include "XMyActionHit.h"
#include "XMyActionDie.h"
#include "XMyActionUseItem.h"
#include "XMyActionHold.h"
#include "XMyActionLootingItem.h"
#include "XMyActionRebirth.h"
#include "XMyActionGrappled.h"
#include "XMyActionMapObj.h"
#include "XMyActionChangeChannel.h"
#include "XMyActionQuestInteraction.h"
#include "XMyActionDeactivative.h"
#include "XMyActionSwallowed.h"
#include "XMyActionSit.h"
#include "XMyActionGesture.h"
#include "XMyActionNPCInteraction.h"
#include "XMyActionEnchant.h"
#include "XMyActionDye.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "XEffectInvoker.h"
#include "XTalentHitParam.h"
#include "XMouseMove.h"
#include "XStrings.h"
#include "XPlayerPVP.h"
#include "XCameraManager.h"
#include "XSensorController.h"
#include "XSensorManager.h"
#include "XLightEffect.h"
#include "CSMotionFactorHelper.h"
#include "XInteractionInfo.h"
#include "XGameTransformControllerManager.h"
#include "XTargetIndicator.h"

#include "XPost_Trade.h"
#include "XPost_Guild.h"
#include "XPost_Action.h"
#include "XPost_Duel.h"
#include "XChallengerQuestExitChecker.h"

#include "XHelp.h"
#include "XNetDebuger.h"

bool XModuleMyControl::CheckSpecialActionKey( SPECIAL_ACTION_COMMAND& eCmd )
{
	eCmd		 = (SPECIAL_ACTION_COMMAND)gg.controller->CheckSequenceKeyCommand();

	// 마우스로 가드인 경우는 따로 체크한다.
	if(IsCurrentGuardEnabled())
		eCmd = SPECIAL_KEY_GUARD;

	if (eCmd == SPECIAL_KEY_NONE)
		return false;

	return m_pActionFSM->OnSpecialAction( eCmd );
}

bool XModuleMyControl::CheckUsableSpecialActionKey(XEvent& msg)
{
	// 탤런트 히트인 경우에는 스페셜 액숀 키를 쓰면 안된다.
	if(msg.m_nID == XEVTD_TALENT_HIT)
		return false;

	return true;
}

bool XModuleMyControl::DoAction(ACTION_STATE nState, void* pParam)
{
	if ((GetCurrentActionStateID() == ACTION_STATE_HIT) && (nState == ACTION_STATE_HIT))
	{
		m_pActionFSM->GetCurrentState()->Exit();
		if (m_pActionFSM->GetCurrentState()->CheckEnterable(pParam))
			m_pActionFSM->GetCurrentState()->Enter(pParam);
		else
			return DoAction(ACTION_STATE_IDLE, pParam);
	}
	else
		return m_pActionFSM->ChangeState(nState, pParam);

	return true;
}

bool XModuleMyControl::DoAction(ACTION_STATE nState, int nParam)
{
	return DoAction(nState, &nParam);
}

XMyPlayer* XModuleMyControl::GetMyPlayerOwner()
{
	return m_pOwner->AsMyPlayer();
}

void XModuleMyControl::InitStates()
{
	XMyActionState* pState;

	pState = new XMyActionIdle(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionJump(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionFalling(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionChangeState(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionHit(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionGuard(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionNormalAttack(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionUseTalent(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionBow(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionUseItem(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionSwitchingWeapon(this);m_pActionFSM->AddState(pState);
	pState = new XMyActionLootingItem(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionRebirth(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionDie(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionHold(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionGrappled(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionMapObj(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionChangeChannel(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionQuestInteraction(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionDeactivative(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionSwallowed(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionSit(this);			m_pActionFSM->AddState(pState);
	pState = new XMyActionGesture(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionNPCInteraction(this);	m_pActionFSM->AddState(pState);
	pState = new XMyActionEnchant(this);		m_pActionFSM->AddState(pState);
	pState = new XMyActionDye(this);			m_pActionFSM->AddState(pState);

	// 기본 상태 설정
	m_pActionFSM->ChangeState(ACTION_STATE_IDLE);
}


/////////////////////////////////////////////////////////////////////////////////////////////
XModuleMyControl::XModuleMyControl(XObject* pOwner)
: XModuleActorControl(pOwner)
, m_bOnTargetObject(false)
, m_bCancelAction(false)
, m_pMouseMove(new XMouseMove(pOwner))
//, m_uidCharacterLightEffect(MUID::ZERO)
{
	_ASSERT(MIsExactlyClass(XMyPlayer, pOwner));

	m_pAttackChecker = new XSpecialActionTransChecker(GetMyPlayerOwner(), &gvar.MyInfo.Talent);
	m_pAttackChecker->Init();

	m_bPaused = false;
	m_bAutoRun = false;
	m_bAutoRunKeyRelease = true;
	m_bRestoreCamera = false;
	m_nMovementFlags = 0;

	m_nCancelParam = -1;

	m_pActionFSM = new XMyActionFSM(this);

	m_TouchSensorCheckRegulator.SetTickTime(0.1f);

	m_vCharacterFrontMoveDir		= -vec3::AXISY;

	m_bDamageRepeat			= false;
	m_bFallingStart = false;
	m_bCurrentFreeLook = false;

	m_bMouseAutoMove = false;
	m_vMouseAutoMove = vec3::ZERO;

	m_pTargetIndicator	= new XTargetIndicator(pOwner);

	m_pChallengerQuestExitChecker = NULL;

	m_pOverlappedEquipmentController = new XMyBPartWeaponController;
}

XModuleMyControl::~XModuleMyControl(void)
{
	SAFE_DELETE(m_pChallengerQuestExitChecker);
	SAFE_DELETE(m_pTargetIndicator);

	m_pActionFSM->DeleteAll();
	SAFE_DELETE(m_pActionFSM);
	SAFE_DELETE(m_pAttackChecker);
	SAFE_DELETE(m_pMouseMove);
}

void XModuleMyControl::OnInitialized()
{
	XModuleActorControl::OnInitialized();
	InitStates();

	m_pModulePost = m_pOwner->GetModulePost();
	_ASSERT(m_pModulePost);

	//CreateCharacterLightEffect();
}

void XModuleMyControl::OnSubscribeEvent()
{
	XModuleActorControl::OnSubscribeEvent();

	Subscribe(XEVTL_VIRTUAL_KEY_PRESSED);
	Subscribe(XEVTL_VIRTUAL_KEY_PRESSEDONCE);
	Subscribe(XEVTL_VIRTUAL_KEY_RELEASED);
	Subscribe(XEVTD_TALENT_HIT);

	// MyActionState에서 받는 메세지 --
	Subscribe(XEVTL_ON_MOTION_EVENT);
	Subscribe(XEVTL_ON_CHANGE_MOTION);
	Subscribe(XEVTL_ON_CHANGE_ANIMATION);

	Subscribe(XEVTL_MOTION_PLAYONCE);
	Subscribe(XEVTL_UPPER_MOTION_PLAYONCE);
	
	Subscribe(XEVTL_ON_RELEASE_TALENT_KEY);

	Subscribe(XEVTL_ON_CHANGED_MOTION_TO_IDLE);
	Subscribe(XEVTL_JUMP_FAILED);

	Subscribe(XEVTL_TALENT_START);
	Subscribe(XEVTL_TALENT_CANCELED);
	Subscribe(XEVTL_TALENT_ACT_SPELL);
	Subscribe(XEVTL_TALENT_RECASTING);

	Subscribe(XEVTL_USE_ITEM);
	//Subscribe(XEVTL_HOLD_KEYABLE);

	Subscribe(XEVTL_MYACTION_SWITCHING_WEAPON_BEGIN);
	Subscribe(XEVTL_MYACTION_SWITCHING_WEAPON_START);
	Subscribe(XEVTL_MYACTION_USE_TALENT);
	Subscribe(XEVTL_MYACTION_NPC_INTERACTION_END);

	Subscribe(XEVTL_LOOT_END_READY);
	Subscribe(XEVTL_LOOT_END);
	Subscribe(XEVTL_LOOT_GETITEM);
	Subscribe(XEVTL_LOOT_GETALLITEM);

	Subscribe(XEVTL_CHANGE_STANCE);
	Subscribe(XEVTL_CHANGED_STANCE);
	
	Subscribe(XEVTL_MYACTION_RELEASE_ACTION_HOLD);

	Subscribe(XEVTL_THROW);
	Subscribe(XEVTL_GRAPPLED_TERRAIN_COL);

	Subscribe(XEVTL_CRAFTING_REQ);	

	Subscribe(XEVTL_EXPCEPTION_HOLD);

	Subscribe(XEVTD_END_MF_STUN);
	Subscribe(XEVTD_END_MF_KNOCKDOWN);

	Subscribe(XEVTL_GUARD_KNOCKBACK);
	Subscribe(XEVTL_GUARDED);
	Subscribe(XEVTL_GUARD_FAILED);

	Subscribe(XEVTL_INTERACTION_CLICK);
	Subscribe(XEVTL_MOUSE_MOVE_CLICK);

	Subscribe(XEVTL_SWALLOWED_DISAPPEAR);

	Subscribe(XEVTL_ENCHANT_ITEM_SELECT);
	Subscribe(XEVTL_ENCHANT_START);
	Subscribe(XEVTL_ENCHANT_CANCEL);
	Subscribe(XEVTL_ENCHANT_CHECK_REQ);
	Subscribe(XEVTL_ENCHANT_CHECK_SUCCESS);
	Subscribe(XEVTL_ENCHANT_RESULT);

	Subscribe(XEVTL_DYE_ITEM_SELECT);
	Subscribe(XEVTL_DYE_START_ITEM);
	Subscribe(XEVTL_DYE_END_ITEM);
	Subscribe(XEVTL_DYE_CANCEL);

	Subscribe(XEVTL_CAMERA_PVP_VIEW_CLEAR);

	Subscribe(XEVTL_DIE_ANI_END);
	Subscribe(XEVTD_ACTION_SLEEP);
}

XEventResult XModuleMyControl::OnEvent(XEvent& msg)
{
	if (XModuleActorControl::OnEvent(msg) == MR_TRUE) return MR_TRUE;

	if (CheckActionEventAcceptable(&msg) == false)	
	{
		if (msg.m_nID == XEVTL_VIRTUAL_KEY_PRESSED ||
			msg.m_nID == XEVTL_VIRTUAL_KEY_RELEASED ) return MR_IGNORED;
	}

	// key sequence process
	SPECIAL_ACTION_COMMAND eCmd		= SPECIAL_KEY_NONE;
	if (CheckUsableSpecialActionKey(msg) && CheckSpecialActionKey( eCmd ))
	{
		global.ui->GetHelpMgr()->SpecialActionCmd( eCmd );
		return MR_TRUE;
	}

	// Helper
	global.ui->GetHelpMgr()->MyControlEvent( msg );

	// ActionFSM event process
	if (m_pActionFSM->Event(msg) == MR_TRUE) return MR_TRUE;

	switch (msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_ACTION:
//			case VKEY_GUARD:
				{
					m_bAutoRun=false;
					m_bMouseAutoMove = false;
				}
				break;
			case VKEY_AUTO_RUN:
				{
					//// 탈것 임시키		_by tripleJ
					//XPlayer* pPlayer		= m_pOwner->AsPlayer();
					//if( pPlayer )
					//{
					//	int nID	= pPlayer->GetRideID();
					//	if( 0 == nID )
					//	{
					//		pPlayer->SetRide( 2000000 );
					//	}
					//		
					//	else
					//		pPlayer->SetRide( 0 );
					//}
					////==

					if(GetCurrentActionStateID() != ACTION_STATE_IDLE || 
						m_bAutoRunKeyRelease == false)
						break;

					m_bAutoRun = !m_bAutoRun;
					m_bMouseAutoMove = false;
					m_bAutoRunKeyRelease = false;
					if (m_bAutoRun)
					{
						SyncPlayerDirToCameraDir();
						m_bCurrentFreeLook = false;
					}
					else
					{
						m_pModuleAction->Stop();
					}
				}
				break;
			case VKEY_LOCKTARGET:
				{
					if (XGetInteractionInfo().stTargetActorInfo.bLockOnTarget)
					{
						m_pTargetIndicator->Init();
						XGetInteractionInfo().SetTargetUIID(0);
					}
					else
					{
						UIID uiidSearch = XGetInteractionInfo().GetSearchUIID();

						XNetDebuger::PostAIMonitorLockTarget(uiidSearch);				// 운영자 등급이면서 로컬 서버에 접속했을 때에만 동작함

						XObject* pObject = gg.omgr->FindActor_UIID(uiidSearch);
						if (pObject && !pObject->AsActor()->IsDead())
							XGetInteractionInfo().SetTargetUIID(uiidSearch);
						else
							return MR_TRUE;
					}

					XGetInteractionInfo().stTargetActorInfo.bLockOnTarget = !XGetInteractionInfo().stTargetActorInfo.bLockOnTarget;
					XGetInteractionInfo().stTargetActorInfo.nLockOnTime = timeGetTime();

					if(global.script)
					{
						global.script->GetGlueGameEvent().OnTargetInfoRefresh();
						global.script->GetGlueGameEvent().OnEnemyInfoRefresh();
					}
				}
				break;
			};
		}
		return MR_TRUE;
	case XEVTL_VIRTUAL_KEY_RELEASED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:
				{
					if (GetCurrentActionStateID() == ACTION_STATE_IDLE) 
					{
						if (m_bAutoRun == false && m_bMouseAutoMove == false && 
							GetCurrMotionType() == MT_DEFAULT)
						{
							m_pModuleAction->Stop();
						}
					}
				}
				break;
			case VKEY_TOGGLE_FREECAMERA:
				{
					if (gvar.MyInfo.ChrInfo.nPlayerGrade >= PLAYER_GRADE_DEVELOPER)
					{
						m_bCurrentFreeLook = !m_bCurrentFreeLook;
					}					
				}
				break;
			case VKEY_AUTO_RUN:
				{
					m_bAutoRunKeyRelease = true;
				}
				break;
			}
		}
		return MR_TRUE;
	case XEVTD_TALENT_HIT:
		{
			HitByTalent(static_cast<XTalentHitParam*>(msg.m_pData));
			SAFE_DELETE(msg.m_pData);
		}
		return MR_TRUE;

	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			DoActionIdle();
			break;
		}
	case XEVTL_CHANGE_STANCE:
		{
			if ( m_pModuleAction)
				m_pModuleAction->StopMove();

			DoAction( ACTION_STATE_CHANGE_STANCE);
			break;
		}
	case XEVTL_CHANGED_STANCE:
		{
			if ( m_ChangeWeaponInfo.IsEmpty() == false)
				ChangeWeapon();

			break;
		}
	case XEVTL_JUMP_FAILED:
		{
			if (GetCurrentActionStateID() == ACTION_STATE_JUMP)
			{
				DoActionIdle();
			}
			break;
		}
	case XEVTL_TALENT_CANCELED:
		{
			if (GetCurrentActionStateID() == ACTION_STATE_USE_TALENT ||
				GetCurrentActionStateID() == ACTION_STATE_USE_ITEM ||
				GetCurrentActionStateID() == ACTION_STATE_NORMAL_ATTACK)
			{
				DoActionIdle();
			}
			break;
		}
	case XEVTL_EXPCEPTION_HOLD:
		{
			Stop();
		}
		break;
	case XEVTL_INTERACTION_CLICK:
		{
			MPoint ptCursorPos = *(static_cast<MPoint*>(msg.m_pData));
		}
		break;
	case XEVTL_MOUSE_MOVE_CLICK:
		{
			MPoint ptCursorPos = *(static_cast<MPoint*>(msg.m_pData));
			MouseMoveClick(ptCursorPos);
		}
		break;
	};

	return MR_FALSE;
}

XMotionTypes XModuleMyControl::GetCurrMotionType()
{
	XMotionTypes nMotionType = MT_DEFAULT;

	if ( global.ui->IsMouseVisible() == false)
	{
		if ( gg.controller->IsVirtualKeyPressedAsync( VKEY_FORWARD))		nMotionType = MT_FORWARD;
		else if ( gg.controller->IsVirtualKeyPressedAsync(VKEY_BACKWARD))	nMotionType = MT_BACKWARD;
		else if ( gg.controller->IsVirtualKeyPressedAsync( VKEY_LEFT))		nMotionType = MT_LEFT;
		else if ( gg.controller->IsVirtualKeyPressedAsync( VKEY_RIGHT))		nMotionType = MT_RIGHT;
	}

	return nMotionType;
}

//////////////////////////////////////////////////////////////////////////
void XModuleMyControl::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	PFI_BLOCK_THISFUNC(304);
	XModuleActorControl::OnUpdate(fDelta);

	_UpdateMyCharacterAlpha();
}

bool XModuleMyControl::OnUpdateCheckUpdatable()
{
	return (gg.omgr != NULL);
}

void XModuleMyControl::OnUpdateState( float fDelta )
{
	PFC_THISFUNC;

	// 탈랜트 상태 업데이트
	if ( m_pModuleTalent->IsActive())		
	{
		int nID = -1;
		if(m_pModuleTalent->GetInfo())
		{
			nID = m_pModuleTalent->GetInfo()->m_nID;
		}

		m_pModuleMovable->TriggerTalentActive(nID);
	}
	else									m_pModuleMovable->TriggerTalentDeActive();

	// Falling 상태 체크
	UpdateStateFalling();


	m_pActionFSM->Update(fDelta);

	XModuleActorControl::OnUpdateState(fDelta);
}


void XModuleMyControl::OnUpdateCommand( float fDelta )
{
	PFC_THISFUNC;

	// 카메라 방향으로 이동
	vec3 dir(gg.controller->GetBackViewCameraDir());

	// 라이트 위치(임시 삭제)
	//_UpdateLightEffectPos(dir);

	// 전진 이동방향 업데이트 및 카메라 설정
	_UpdateFrontDirAndSetCamera( fDelta, dir);

	// 캐릭터 방향 업데이트
	_UpdateMyDir( fDelta, dir);

	// 내 캐릭터 이동 업데이트
	bool bKeyAction = _UpdateMyMovement(fDelta, m_pModuleEntity, dir);

	// post packet
	m_pModulePost->UpdateMovement(fDelta, m_pModuleEntity->GetPosition(), dir, bKeyAction, m_nMovementFlags);

	// 경계선 경고
	_UpdateRestrictedArea(fDelta);
	
	// post key event
	if (GetCurrentActionStateID() == ACTION_STATE_IDLE)
	{
		queue<XVirtualKey>& q = gg.controller->GetReservedKey();
		while (!q.empty())
		{
			XEvent evt;
			evt.m_nID = XEVTL_VIRTUAL_KEY_PRESSED;
			XVirtualKey key = q.back();
			evt.m_pData = &key;
			Event(evt);

			q.pop();
		}
	}
}

void XModuleMyControl::OnUpdateSpecial( float fDelta )
{
	PFC_THISFUNC;

	_UpdateTouchSensorTrigger(fDelta);

	if(m_pTargetIndicator)	m_pTargetIndicator->OnUpdate();

	m_MapSignAreaChecker.UpdateTouchAreaTrigger(m_pOwner, fDelta);

	if (m_pChallengerQuestExitChecker)
		m_pChallengerQuestExitChecker->Update(fDelta);
}

void XModuleMyControl::OnPostUpdate(float fDelta)
{
	PFC_THISFUNC;

	m_pActionFSM->PostUpdate(fDelta);

	XModuleActorControl::OnPostUpdate(fDelta);
}

//////////////////////////////////////////////////////////////////////////

bool XModuleMyControl::_UpdateMyMovement(float fDelta, XModuleEntity* pModuleEntity, vec3& vDir)
{
	if ( m_bPaused == true)	return false;

	if (gg.controller->IsMainBackViewCamera() == false) return false;


	// 버프에 의한 이동 금지인지 확인
	XBuffAttribute attrBuff = m_pModuleBuff->GetBuffAttribute();
	if ( attrBuff.IsMovable() == false)
		return false;

		
	unsigned int nTempMovementFlags = m_nMovementFlags;				// 키 변환에 대한 체크
	m_nMovementFlags = 0;

	const int nActionFSMStateID = GetCurrentActionStateID();
	const XMovableFactor& movableFactor = m_pModuleMovable->GetMovableFactor();

	if (nActionFSMStateID != ACTION_STATE_IDLE &&
		nActionFSMStateID != ACTION_STATE_CHANGE_STANCE &&
		nActionFSMStateID != ACTION_STATE_WEAPON_CHANGE) 
	{
		bool bReturn = true;
		if (nActionFSMStateID == ACTION_STATE_HIT) m_nMovementFlags |= MOVEMENTFLAG_MOTION_FACTOR;
		else if(nActionFSMStateID == ACTION_STATE_GUARD) m_nMovementFlags |= MOVEMENTFLAG_GUARD;

		if (movableFactor.IsFalling()) 
		{
			m_nMovementFlags |= MOVEMENTFLAG_FALLING;
		}
		if( nActionFSMStateID == ACTION_STATE_JUMP)
		{
			if(m_pActionFSM->OnSpecialAction(SPECIAL_KEY_NONE) == false)
				m_nMovementFlags |= MOVEMENTFLAG_JUMPING;
			else
				bReturn = false;
		}

		if (nActionFSMStateID == ACTION_STATE_USE_TALENT) 
		{
			if(m_pActionFSM->OnSpecialAction(SPECIAL_KEY_NONE) == false)
				m_nMovementFlags |= MOVEMENTFLAG_USING_TALENT;
			else 
				bReturn = false;
		}

		if((nActionFSMStateID == ACTION_STATE_NORMAL_ATTACK && CheckAttackReturnAnimation()))
			bReturn = false;

		if(bReturn)
		{
			m_pModuleAction->InitPrevDir();
			return false;
		}
	}

	bool bMoveKeyPress = false;
	vec3 dir = vDir;

	// 오토런인가?
	if (m_bAutoRun)
	{
		bMoveKeyPress = true;
		m_nMovementFlags = MOVEMENTFLAG_FORWARD;
	}

	// 스탠스 스피드 값
	const float fSpeedRatio = GetExtSpeedRatio();

	// 마우스 이동인가?
	if(m_bMouseAutoMove)
	{
		CheckMouseMoveState();

		if (CheckActionEventAcceptable() == false &&
			global.ui->IsMouseCursorToggleKeyDown() == false )
			return false;
	}
	// 밑에부터는 액션 이벤트(키 인풋 등) 수신불가능할때는 리턴해버린다. 위에 flag는 비활성화일때에도 적용해줘야 하는 것들.
	else if (CheckActionEventAcceptable() == false &&
		global.ui->IsMouseCursorToggleKeyDown() == false )
	{
		float fSpeed = GetMyPlayerOwner()->GetMoveSpeed(m_nMovementFlags);
		if ( fSpeedRatio  > 0.0f)
			fSpeed *= fSpeedRatio;

		XMovableGoParam* pGoParam = new XMovableGoParam(XMovableGoParam::E_GO_DIRECTION);
		XMovableFactor::SInterpolation param = m_pModuleMovable->GetMovableFactor().INTERPOLATION;
		pGoParam->Set( param.m_vTargetPos, dir, m_fSpeed, param.m_fRotateSpeed, BOOST_NONE, m_nMovementFlags , true );
		pGoParam->SetKeyPress( bMoveKeyPress);
		m_pModuleAction->UpdateMyMovementAction( fDelta, pGoParam);
		delete pGoParam;

		return false;
	}


	if (gg.controller->IsVirtualKeyPressedAsync(VKEY_FORWARD))
	{
		m_bAutoRun = false;
		m_bMouseAutoMove = false;
		bMoveKeyPress = true;
		m_nMovementFlags = MOVEMENTFLAG_FORWARD;
	}
	else if (gg.controller->IsVirtualKeyPressedAsync(VKEY_BACKWARD))
	{
		m_bAutoRun = false;
		m_bMouseAutoMove = false;
		bMoveKeyPress = true;
		m_nMovementFlags = MOVEMENTFLAG_BACKWARD;

		dir = -vDir;
	}

	if (gg.controller->IsVirtualKeyPressedAsync(VKEY_LEFT))
	{
		m_bMouseAutoMove = false;
		vec3 pos = pModuleEntity->GetPosition();
		vec3 left = -CrossProduct(vDir, UP_VECTOR);
		left.z = 0.0f;
		left.Normalize();

		if (bMoveKeyPress)
		{
			dir = dir + left;
			dir.Normalize();
			m_nMovementFlags |= MOVEMENTFLAG_LEFT;
		}
		else
		{
			dir = left;
			m_nMovementFlags = MOVEMENTFLAG_LEFT;
			bMoveKeyPress = true;
		}
	}
	else if (gg.controller->IsVirtualKeyPressedAsync(VKEY_RIGHT))
	{
		m_bMouseAutoMove = false;
		vec3 right = CrossProduct(vDir, UP_VECTOR);
		right.z = 0.0f;
		right.Normalize();

		if (bMoveKeyPress)
		{
			dir = dir + right;
			dir.Normalize();
			m_nMovementFlags |= MOVEMENTFLAG_RIGHT;
		}
		else
		{
			dir = right;
			m_nMovementFlags = MOVEMENTFLAG_RIGHT;
			bMoveKeyPress = true;
		}
	}


	// 스피드 계산
	m_fSpeed = GetMyPlayerOwner()->GetMoveSpeed(m_nMovementFlags);

	if ( fSpeedRatio  > 0.0f)
		m_fSpeed *= fSpeedRatio;


	// 디버그시 쉬프트 키를 누르면 이동 속도가 빠르다.
#ifdef _DEBUG
	if (global.ui->IsDebugSpeedUpKeyDown())
		m_fSpeed *= 5.0f;
#endif

	if(m_bMouseAutoMove &&
		bMoveKeyPress == false)
	{
		return true;
	}

	static bool bLastMoving = bMoveKeyPress;
	if (bMoveKeyPress==false && bLastMoving==true)
	{
		if (nActionFSMStateID == ACTION_STATE_IDLE) 
		{
			if (m_pModuleAction->GetCurrentMotionName() == MOTION_NAME_RUN)
			{
				if (GetCurrMotionType() == MT_DEFAULT)
				{
					m_pModuleAction->Stop();
				}
			}
		}
	}
	bLastMoving = bMoveKeyPress;

	XMovableGoParam* pGoParam = new XMovableGoParam(XMovableGoParam::E_GO_DIRECTION);
	XMovableFactor::SInterpolation param = m_pModuleMovable->GetMovableFactor().INTERPOLATION;
	pGoParam->Set( param.m_vTargetPos, dir, m_fSpeed, param.m_fRotateSpeed, BOOST_NONE, m_nMovementFlags , true );
	pGoParam->SetKeyPress( bMoveKeyPress);
	m_pModuleAction->UpdateMyMovementAction( fDelta, pGoParam);
	delete pGoParam;

	if( nTempMovementFlags != m_nMovementFlags)
	{
		return true;
	}

	return false;
}


void XModuleMyControl::_UpdateTouchSensorTrigger(float fDelta)
{
	PFC_THISFUNC;

	if (!m_TouchSensorCheckRegulator.IsReady(fDelta))		return;
	if (gg.currentgamestate->GetWorld()->IsNowLoading())	return;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return;

	vec3 vMyPos = m_pModuleEntity->GetPosition();
	m_pSensorController->Update(pFieldInfo, vMyPos);
}

//////////////////////////////////////////////////////////////////////////

void XModuleMyControl::OnHit(XTalentHitParam* pHitInfo)
{
	m_HitController.SetHitData(pHitInfo);
	XModuleActorControl::OnHit(pHitInfo);
}

void XModuleMyControl::OnDie( MF_STATE nMFState /*= MF_NONE*/, bool bSetAni /*= true*/ )
{
	m_bAutoRun = false;
	m_bMouseAutoMove = false;

	GetMyPlayerOwner()->OnDie();

	DoForceCameraLock();

	DoActionDie();
}

void XModuleMyControl::DoActionDie()
{
	if (m_bReservedHitByTalent == false)
	{
		DoAction(ACTION_STATE_DIE);
	}
}

void XModuleMyControl::Rebirth()
{
	GetMyPlayerOwner()->OnRebirth();

	m_bAutoRun = false;
	m_bMouseAutoMove = false;

	// 죽은 액션에 대한 데이터를 초기화한다.
	if(m_pOwner->GetModuleAction())
	{
		m_pOwner->GetModuleAction()->EndDie();
	}


	// 이동 관련 상태 초기화
	if ( m_pModuleMovable)
		m_pModuleMovable->TriggerDie( false);
}

void XModuleMyControl::OnRebirth()
{
	Rebirth();

	DoAction(ACTION_STATE_REBIRTH);
}

void XModuleMyControl::OnRebirthSoulBinding()
{
	Rebirth();

	DoActionIdle();
}

void XModuleMyControl::HitByTalent(XTalentHitParam * pHitInfo)
{
	m_HitController.DelHitData();

	CheckReservedHitByTalent();

	XObject* pAttackObject = gg.omgr->FindActor_UIID(pHitInfo->nAttackerUIID);
	if (pAttackObject && pAttackObject->GetModuleTalent())
	{
		pAttackObject->GetModuleTalent()->OnHitTarget(pHitInfo->nTalentID);
	}

	m_bAutoRun					= false;
	m_bDamageRepeat				= false;
	m_bMouseAutoMove			= false;

	bool bDieCheck				= false;

	//if(m_pOwner->GetUID() == XGetMyUID())
	//	mlog("모션 팩터 %s(%d)\n", MLocale::ConvTCHARToAnsi(MF_STATE_NAME[pHitInfo->nMFState]).c_str(), pHitInfo->nMFWeight);

	// 죽었다면.. 피격을 실행하지 않는다.
	//if(m_DeathController.IsDeadActionImmediatelyOnHit(GetMyPlayerOwner(), pHitInfo->nMFState))
	//{
	//	OnDie();
	//}
	//else 
	{
		// 탤런트 취소 체크		
		if(m_pModuleTalent->IsActivate() && m_pModuleTalent->GetInfo() &&
			(m_pModuleTalent->GetInfo()->m_nSkillType == ST_SOCIAL ||																	// 소셜 탤런트 사용중이면 취소가 되게
			(m_pModuleTalent->GetInfo()->m_bCancelable && XTalentHelper::IsTalentCancelableMotionFactor(pHitInfo->nMFState)) ))			// 최소 가능한 탤런트 체크
			m_pModuleTalent->Cancel();
	

		if(pHitInfo->nMFState != MF_GRAPPLED &&
			pHitInfo->nMFState != MF_SWALLOWED)
		{
			if (!CSMotionFactorHelper::IsNoneOrFakes(pHitInfo->nMFState))
			{
				if (pHitInfo->nMFWeight > 0)
				{
					DoAction(ACTION_STATE_HIT, pHitInfo);
					bDieCheck = true;
				}
			}
			// MF_NONE일때 탤런트 사용중이라면... 모션팩터를 안한다. 
			else if (m_pModuleTalent->IsActive() == false || pHitInfo->nMFState == MF_FAKE_KNOCKBACK)
			{
				stMotionFactorResult rtMotionFactor;
				stServerKnockbackVictimInfo knockbackInfo(pHitInfo->vServerKnockbackDir, pHitInfo->vServerKnockbackVictimPos, pHitInfo->vServerKnockbackVictimTargetPos);
				m_pModuleAction->ActMF(pHitInfo->nMFState, pHitInfo->nMFWeight, pHitInfo->nAttackerUIID, knockbackInfo, pHitInfo->nTalentID, rtMotionFactor);

				// 죽은 처리...
				if(GetMyPlayerOwner()->IsDead())
				{
					OnDie(pHitInfo->nMFState, rtMotionFactor.bUseMFAnimation);
					bDieCheck = true;
				}
			}
		}
		else
		{
			if(pHitInfo->nMFState == MF_GRAPPLED)
			{
				// 잡기 모션이라면...
				_DamageRepeatInfo * pDamageRepeatInfo = InitDamageRepeat(pHitInfo);
				if(StartGrappled(pDamageRepeatInfo) == false)
					SAFE_DELETE(pDamageRepeatInfo);
			}
			else if(pHitInfo->nMFState == MF_SWALLOWED)
			{
				// 먹기 모션이라면...
				_DamageRepeatInfo * pDamageRepeatInfo = InitDamageRepeat(pHitInfo);
				if(StartSwallowed(pDamageRepeatInfo) == false)
					SAFE_DELETE(pDamageRepeatInfo);
			}

			return;
		}
	}

	// 죽은 처리...
	if(bDieCheck == false && GetMyPlayerOwner()->IsDead())
	{
		OnDie(pHitInfo->nMFState);
	}

	// 피격및 데미지 보여주기
	_DamageInfo damageInfo;
	damageInfo.nDamage				= pHitInfo->nDamage;
	damageInfo.nFlags				= pHitInfo->nFlags;
	damageInfo.nMFState				= pHitInfo->nMFState;
	damageInfo.nAttackedTalentID	= pHitInfo->nTalentID;
	damageInfo.nAttackerUIID		= pHitInfo->nAttackerUIID;
	damageInfo.bSecondaryEffect		= pHitInfo->bSecondaryEffect;

	ShowHitAndDamage(&damageInfo, pAttackObject);
	FakeBeatenAndSetMultiplyColor(pHitInfo);

	BirdTest_ShowDelay(pHitInfo);
}

void XModuleMyControl::SetCancelAction(int nParam)
{
	m_bCancelAction = true;
	m_nCancelParam = nParam;
	m_nLastCancelActionTime = XGetNowTime();

	XEvent evt;
	evt.m_nID = XEVTL_ON_CAN_CANCEL_ACTION;
	Event( evt);
}

void XModuleMyControl::DoActionUseItem( int nSlotID )
{
	bool bRet = DoAction(ACTION_STATE_USE_ITEM, &nSlotID);
	//_ASSERT(bRet);
}

void XModuleMyControl::DoActionUseTalent(int nTalentID)
{
	XEvent evt;
	evt.m_nID = XEVTL_MYACTION_USE_TALENT;
	evt.m_pData = (void*)&nTalentID;
	Event( evt);
}

void XModuleMyControl::DoActionEquipItem( SH_ITEM_SLOT nSlot, MUID uidItemID)
{
	// ! XItem::m_uidID 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//// 전투 상태라면
	//if ( GetMyPlayerOwner()->GetStance() == CS_BATTLE)
	//{
	//	// 지금 사용하는 무기와 교체하려는 무기 슬롯이 같은지 확인
	//	int nWeaponSet = gvar.MyInfo.EquipmentSlot.GetWeaponSet();
	//	if ( (nSlot == ITEMSLOT_RWEAPON  &&  nWeaponSet == 0)  ||  (nSlot == ITEMSLOT_RWEAPON2  &&  nWeaponSet == 1))
	//	{
	//		// 무기 변경 정보 설정
	//		m_ChangeWeaponInfo.Set( nSlot, uidItemID);

	//		// 스탠스 변경
	//		DoAction( ACTION_STATE_CHANGE_STANCE);

	//		return;
	//	}
	//}

	//m_pModulePost->PostEquipItem( nSlot, uidItemID);
}

// cur 각도에서 tar 각도까지 가장 짧게 회전하는 값을 구한다
float GetLowerDiffAngle( float cur, float tar)
{
	static const float _2PI = MMath::PI * 2.0f;

	float fDiff1 = tar - cur;
	float fDiff2 = (fDiff1 > 0.0f) ? (fDiff1 - _2PI) : (fDiff1 + _2PI);
	float ang = ( abs( fDiff1) < abs( fDiff2)) ? fDiff1 : fDiff2;

	return ang;
}

void XModuleMyControl::_UpdateMyDir( float fDelta, const vec3& dir )
{
	if ( m_bPaused == true)
		return;

	if ( gg.controller->IsMainBackViewCamera() == false)
		return;

	if ( m_bOnTargetObject == true)
		return;

	if ( GetMyPlayerOwner()->IsDead())
		return;


	// 버프에 의한 회전 금지인지 확인
	XBuffAttribute attrBuff = m_pModuleBuff->GetBuffAttribute();
	if ( attrBuff.IsMovable() == false)
		return;

	// 가드 상태에서는 회전 금지...
	if(CheckGuardKnockback())
		return;


	vec3 vMyDir = Normalize( m_pOwner->GetDirection());
	float fTarDir = dir.GetAngleXY();
	float fMyDir = vMyDir.GetAngleXY();


	// 대각선 방향 이동에 대한 회전을 설정
	static const float fLimitRotateAngleA = MMath::PI / 8.0f;
	static const float fLimitRotateAngleB = MMath::PI / 8.0f + 0.3f;
	static const float fLimitRotateAngleC = MMath::PI / 2.0f;
	static const float fRotSpeed = 7.0f;

	if ( ( (m_nMovementFlags & MOVEMENTFLAG_FORWARD)  &&  (m_nMovementFlags & MOVEMENTFLAG_LEFT))  ||
		( (m_nMovementFlags & MOVEMENTFLAG_BACKWARD)  &&  (m_nMovementFlags & MOVEMENTFLAG_RIGHT)) )
	{
		fTarDir += fLimitRotateAngleA;
	}
	else if ( ( (m_nMovementFlags & MOVEMENTFLAG_FORWARD)   &&  (m_nMovementFlags & MOVEMENTFLAG_RIGHT))  ||
		( (m_nMovementFlags & MOVEMENTFLAG_BACKWARD)  &&  (m_nMovementFlags & MOVEMENTFLAG_LEFT)) )
	{
		fTarDir -= fLimitRotateAngleA;
	}

	//Rotating the model left and right when we move in those directions
	if( !m_bAutoRun && GetMyPlayerOwner()->GetModuleMyControl()->GetCurrMotionType() == MT_LEFT &&  m_nMovementFlags & MOVEMENTFLAG_LEFT )
		fTarDir += fLimitRotateAngleC;

	if( !m_bAutoRun && GetMyPlayerOwner()->GetModuleMyControl()->GetCurrMotionType() == MT_RIGHT && m_nMovementFlags & MOVEMENTFLAG_RIGHT )
		fTarDir -= fLimitRotateAngleC;


	// 카메라 회전에 따른 캐릭터 회전
	float fDiff = GetLowerDiffAngle( fMyDir, fTarDir);
	if ( abs( fDiff) < 0.01f)
		return;

	float fRot = 0.0f;
	if ( fDiff > 0.0f)			fRot = min( fDiff,  fRotSpeed * fDelta);
	else if ( fDiff < 0.0f)		fRot = max( fDiff, -fRotSpeed * fDelta);

	float _diff = fDiff - fRot;
 	if ( _diff > fLimitRotateAngleB)		fRot += _diff - fLimitRotateAngleB;
	else if ( _diff < -fLimitRotateAngleB)	fRot += _diff + fLimitRotateAngleB;


	// 회전 적용
	vec3 vNextDir;
	vNextDir.x = m_pOwner->GetDirection().x * cos( fRot) - m_pOwner->GetDirection().y * sin( fRot);
	vNextDir.y = m_pOwner->GetDirection().y * cos( fRot) + m_pOwner->GetDirection().x * sin( fRot);
	vNextDir.z = 0.0f;
	Normalize( vNextDir);

	m_pOwner->SetDirection( vNextDir);
}

void XModuleMyControl::_UpdateFrontDirAndSetCamera( float fDelta, vec3 & dir)
{
	//////////////////////////////////////////////////////////////////////////
	// 컨트롤
	// 컨트롤이 아니라도 사용가능한가?
	if(m_bCurrentFreeLook)
	{
		m_bCurrentFreeLook = false;
		if(m_bAutoRun ||																	// 오토런일때
			m_bMouseAutoMove ||																// 마우스 이동시
			(GetCurrentActionStateID() == ACTION_STATE_IDLE && m_nMovementFlags == 0))		// 이동이 없는 IDLE일때
		{
			m_bCurrentFreeLook = true;
		}
	}
		
	// 컨트롤 상태 및 마우스 이동
	if (global.ui)
	{
		/*if(global.ui->IsFreeCameraKeyDown())
		{
			// 컨트롤 키와 마우스 이동시 자유모드 작동
			m_bCurrentFreeLook = !m_bCurrentFreeLook;
		}
		else*/ if(m_bMouseAutoMove)
			m_bCurrentFreeLook = true;
	}

	// 카메라 자유모드 셋팅
	gg.controller->SetFreeLookCamera(m_bCurrentFreeLook);

	//////////////////////////////////////////////////////////////////////////
	// 설정
	bool bMotionCameraLook = m_pModuleAction->IsCameraLook();
	if(!bMotionCameraLook || m_bCurrentFreeLook ||
		gg.controller->IsMainBackViewCamera() == false)		// BackViewCamera가 아니라면...
	{
		// 캐릭터 이동 방향 앞으로 설정
		dir = m_vCharacterFrontMoveDir;
	}
	else
	{
		// 카메라 방향으로 설정
		m_vCharacterFrontMoveDir = dir;
	}

	XMath::NormalizeZ0(dir);
}

void XModuleMyControl::SyncPlayerDirToCameraDir()
{
	if (global.ui)
	{
		if (global.ui->IsFreeCameraKeyDown()) return;
	}

	// 카메라 방향으로 이동
	vec3 vCamDir;
	if (global.camera->IsPrimaryCameraType(CAMERA_GRAPPLED))
	{
		// grapple camera가 작동줄일때는 현재 카메라의 방향을 캐릭터의 방향으로 설정한다.
		// Ticket #1930 관련버그
		vCamDir = global.camera->GetPrimaryCamera()->GetDirection();
	}
	else
	{
		vCamDir = gg.controller->GetBackViewCameraDir();
	}

	vCamDir.z = 0.0f;
	vCamDir.Normalize();
	m_pOwner->SetDirection(vCamDir);
	m_vCharacterFrontMoveDir = vCamDir;
}


// CancelAction 액션은 새로운 탤런트를 쓰거나 일정 시간이 지나면 풀린다.
void XModuleMyControl::ReleaseCancelAction()
{
	m_bCancelAction = false;
	m_nCancelParam = -1;
}

bool XModuleMyControl::CheckActionEventAcceptable( XEvent* pEvent /*= NULL*/ )
{
	//	if (global.ui->IsFocusMainInput() == false) return false;
	if (CheckActionEventAcceptableByMouseVisible(pEvent) == false &&
		global.ui->IsMouseVisible()) return false;

	return true;
}

void XModuleMyControl::OnSwitchingWeaponBegin()
{
	// 무기 체인지 시작
	XEvent msg1(XEVTL_MYACTION_SWITCHING_WEAPON_BEGIN);
	m_pActionFSM->Event(msg1);
}

void XModuleMyControl::OnSwitchingWeapon( int8 nWeaponSet )
{
	// 무기 체인지
	//mlog("스위칭 %d\n", nWeaponSet);
	XEvent msg2(XEVTL_MYACTION_SWITCHING_WEAPON_START, &nWeaponSet);
	if(m_pActionFSM->Event(msg2) == MR_FALSE)
	{
		// 이벤트가 존재하지 않으면... 현재 다른 상태로 인식
		// 다른 액션을 취하지 않고 그냥 무기만 바꾼다.
		gvar.Game.pMyPlayer->SwitchingWeaponSet(nWeaponSet);
	}
}

void XModuleMyControl::OnLootEndReady()
{
	// 루팅 종료 준비
	XEvent msg(XEVTL_LOOT_END_READY);
	m_pActionFSM->Event(msg);
}

void XModuleMyControl::OnHitLooting()
{
	// 루팅중일때 맞으면...
	XEvent msg(XEVTL_LOOT_END_READY);
	if(m_pActionFSM->Event(msg) == MR_FALSE)
	{
		// 루팅 상태가 아니라면...
		// 창이라도 닫는다.
		if(global.script)
		{
			global.script->GetGlueGameEvent().OnLootingEnd();
		}
	}
}

bool XModuleMyControl::IsCanStopMouseToggle()
{
	// 마우스 토글을 썼을때 캐릭터 정지 할 수 있는 상태인가?
	int nStateID = m_pActionFSM->GetCurrentState()->GetID();
	if(GetMyPlayerOwner()->IsDead() == false &&				// 죽은 상태가 아니라면...
		nStateID== ACTION_STATE_IDLE &&						// 현재 FSM 상태에 따라 허용
		m_bAutoRun == false)
	{
		return true;
	}

	return false;
}

bool XModuleMyControl::IsCanCancelAllMotion()
{
	return (m_bCancelAction && m_nCancelParam == CANCEL_NEXT_ATTACK_PARAM_ALL);
}

bool XModuleMyControl::IsCanCancelUpper()
{
	return m_bCancelAction && (m_nCancelParam == CANCEL_NEXT_ATTACK_PARAM_UPPERED);
}

void XModuleMyControl::_UpdateReleaseCancelAction()
{
	const unsigned int CANCEL_ACTION_RELEASE_TIME = 1000;	// 1초 후 풀림
	unsigned int nNowTime = XGetNowTime();
	if (nNowTime - m_nLastCancelActionTime > CANCEL_ACTION_RELEASE_TIME)
	{
		ReleaseCancelAction();
	}
}

float XModuleMyControl::GetExtSpeedRatio()
{
	float fSpeed = 1.0f;

	// 버프에 의한 속도를 구함
	if ( m_pModuleBuff)
	{
		XBuffAttribute attrBuff = m_pModuleBuff->GetBuffAttribute();
		fSpeed = attrBuff.m_fMoveSpeedRatio;
	}

	return fSpeed;
}

void XModuleMyControl::InitStand()
{
	// 초기에 서있는 Animation을 위해 정지모션을 보낸다.
	XModuleAction * pModuleAction = m_pOwner->GetModuleAction();
	if(pModuleAction)
	{
		pModuleAction->Stop();
	}
}

void XModuleMyControl::OnWarp( const vec3& vPos, const vec3& vDir )
{
	XModuleActorControl::OnWarp(vPos, vDir);

	// 캐릭터 방향 설정, 모델뿐만 아니라 코드에서 쓰이는 방향까지 바꿔야 한다.
	m_vCharacterFrontMoveDir = vDir;

	if(gg.controller) 
	{
		gg.controller->SetEnabled(true);
		gg.controller->ClearKeyBuffer();

		InitStand();
		InitAutoMove();
		
		m_pSensorController->Clear();	// 워프하면 센서 밟고 있던 것 초기화

		// 카메라 위치도 방향으로 새로 고치자.
		vec3 vCameraDir = gg.controller->GetBackViewCameraDir();
		vCameraDir.x = vDir.x;
		vCameraDir.y = vDir.y;
		vCameraDir.Normalize();
		gg.controller->SetBackViewCameraDir(vCameraDir);
	}
}

void XModuleMyControl::StopRun()
{
	SetAutoRun(false);

	XModuleAction* pModuleAction = GetOwner()->GetModuleAction();
	if ( pModuleAction) 
		pModuleAction->Stop();

	m_nMovementFlags = 0;

	m_pModulePost->PostMoveStop(GetOwner()->GetPosition());
}

void XModuleMyControl::SetPause( bool bPause)
{
	if ( m_bPaused == bPause)
		return;
	
	m_bPaused = bPause;


	if ( bPause == true)
	{
		StopRun();
//		DoAction( ACTION_STATE_IDLE);
	}
	else if ( bPause == false)
	{
	}

	if ( m_pModuleAction)
		m_pModuleAction->SetPause( bPause);
}

bool XModuleMyControl::ChangeWeapon()
{
	if ( m_ChangeWeaponInfo.IsEmpty())
		return false;


	// 무기 변경
	m_pModulePost->PostEquipItem(m_ChangeWeaponInfo.m_nSlot, m_ChangeWeaponInfo.m_uidWeaponID);
	m_ChangeWeaponInfo.Clear();


	// 스탠스 다시 변경
	XEvent evt( XEVTL_CHANGE_STANCE);
	m_pOwner->PostDelayEvent( evt, 500);


	return true;
}

void XModuleMyControl::DoActionLootingItem(MUID& uidNPC, float fLootTime)
{
	gvar.MyInfo.LootingItem.fTime = fLootTime;
	DoAction(ACTION_STATE_LOOTINGITEM, &uidNPC);
}

void XModuleMyControl::OnLootEnd()
{
	XEvent msg(XEVTL_LOOT_END);
	m_pActionFSM->Event(msg);
}

void XModuleMyControl::OnLootItemStart()
{
	if (GetCurrentActionStateID() != ACTION_STATE_LOOTINGITEM) return;

	XMyActionLootingItem* pActionState = static_cast<XMyActionLootingItem*>(m_pActionFSM->GetCurrentState());
	pActionState->LootingStart();
}

void XModuleMyControl::DoActionIdle()
{
	DoAction(ACTION_STATE_IDLE);
}

// TODO : 일단 구분해서 사용하려고 따로 만들었는데 아래것이 필요없어지도록 또는
//			필요없어지면 지웁니다.
void XModuleMyControl::DoActionIdleWithSpecialState()
{
	if (GetCurrentActionStateID() == ACTION_STATE_MAPOBJ_INTERACTION	||
		GetCurrentActionStateID() == ACTION_STATE_CHANGE_CHANNEL		||
		GetCurrentActionStateID() == ACTION_STATE_QUEST_INTERACTION		||
		GetCurrentActionStateID() == ACTION_STATE_DEACTIVATIVE			||
		GetCurrentActionStateID() == ACTION_STATE_HOLD)
	{
		DoActionIdle();
	}
}

void XModuleMyControl::DoInnBeginSleep()
{
	if (GetCurrentActionStateID() != ACTION_STATE_MAPOBJ_INTERACTION) return;

	XMyActionMapObj* pActionState = static_cast<XMyActionMapObj*>(m_pActionFSM->GetCurrentState());
	pActionState->BeginSleep();
}

void XModuleMyControl::DoInnEndSleep()
{
	if (GetCurrentActionStateID() != ACTION_STATE_MAPOBJ_INTERACTION) return;

	XMyActionMapObj* pActionState = static_cast<XMyActionMapObj*>(m_pActionFSM->GetCurrentState());
	pActionState->EndSleep();
}

bool XModuleMyControl::IsCurrentGuardEnabled()
{
	// 강제 테스트
	if(gvar.Debug.bForceGuard)
		return true;

	// 가드 할 수 있는 상태인가?
	if (CheckActionEventAcceptable() == false &&
		global.ui->IsMouseCursorToggleKeyDown() == false )
		return false;

	if (GetMyPlayerOwner()->GetStance() != CS_BATTLE) return false;

	// 가드가 가능한 무기 타입인가?
	WEAPON_TYPE nWeaponType = GetMyPlayerOwner()->GetCurrWeaponType();

	if (CSItemHelper::IsGuardableWeaponType(nWeaponType) == false)
	{
		return false;
	}

	// 키 체크
	bool bKeyPressed = false;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:
	case WEAPON_1H_BLUNT:
	case WEAPON_TWO_HANDED:
	case WEAPON_STAFF:
	case WEAPON_2H_BLUNT:
		{
			bKeyPressed = (gg.controller->IsVirtualKeyPressedAsync(VKEY_GUARD) || 
							(gg.controller->IsVirtualKeyPressedAsync(VKEY_ACTION) && gg.controller->IsVirtualKeyPressedAsync(VKEY_ACTION2)));
		}
		break;
	default:
		_ASSERT(0);
	}

	if (bKeyPressed == false) return false;

	return true;
}

void XModuleMyControl::CheckSwim( bool bIn )
{
	// 서버에게 패킷을 날려야 하는 상태인지 체크
	XMyPlayer* pMyPlayer = GetMyPlayerOwner();

	if(bIn)
	{
		// 지상에서 물속으로 첫 진입시...
		if(pMyPlayer->IsSwim() == false)
		{
			XPostSwim(true);
		}
	}
	else
	{
		// 물속에서 지상으로 첫 진입시...
		if(pMyPlayer->IsSwim() == true)
		{
			XPostSwim(false);
		}
	}

	XModuleActorControl::CheckSwim(bIn);
}

void XModuleMyControl::OnTalentInstantEffectGain( XTalentInfo* pTalentInfo)
{
	int nMyHP = gvar.MyInfo.GetHP();

	gvar.MyInfo.Talent.GainInstantEffect(pTalentInfo);

	int nGainHp = gvar.MyInfo.GetHP() - nMyHP;
	XModuleEffect* pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect && nGainHp > 0)
	{
		XCaptionEffectEventData* pEventData = new XCaptionEffectEventData;
		pEventData->m_nParam1 = nGainHp;
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_HEAL, pEventData);
	}

	// 이펙트 효과...
	XModuleActorControl::OnTalentInstantEffectGain(pTalentInfo);
}

void XModuleMyControl::DoActionChangeChannel()
{
	DoAction( ACTION_STATE_CHANGE_CHANNEL);
}

void XModuleMyControl::FallingDamage()
{
	float fFallingHeight = m_pModuleMovable->GetMovableFactor().GENERAL_STATE.m_fFallingStartHeight - m_pModuleMovable->GetMovableFactor().GENERAL_STATE.m_fFallingEndHeight;

	if (fFallingHeight < 0.f)	return;
	if (fFallingHeight < 600.f) return;

	bool bColWater = false;

	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if (pModuleCollision != NULL) 
	{
		vec3 vPos = m_pOwner->GetPosition();
		vec3 vOut;
		bColWater = pModuleCollision->PickTerrainWater(vPos, vOut);
	}

	//mlog("FallingDamage(%d, %f) \n", bColWater, fFallingHeight);

	XPostFallingEnd(bColWater, fFallingHeight);

	m_pModuleMovable->InitMovableFactorFalling();
}

void XModuleMyControl::DoActionQuestInteraction()
{
	DoAction( ACTION_STATE_QUEST_INTERACTION);
}

void XModuleMyControl::UpdateStateFalling()
{
	if(m_pModuleMovable->GetMovableFactor().GENERAL_STATE.m_bFallingStartRecording)
	{
		// Falling 시작시...
		if(m_bFallingStart == false)
		{
			// Falling 애니메이션
			if(GetCurrentActionStateID() == ACTION_STATE_IDLE)
			{
				DoAction(ACTION_STATE_FALLING, IsAutoRun());
			}

			m_bFallingStart = true;
		}
	}
	else if(m_bFallingStart)
	{
		FallingDamage();
		m_bFallingStart = false;
	}
}


void XModuleMyControl::DoActionDeactivate( const int nBuffID, const wchar_t* szUseAni)
{
	stDeactiveData deactiveData;
	deactiveData.nBuffID = nBuffID;
	deactiveData.strBuffAnimation = szUseAni;
	DoAction( ACTION_STATE_DEACTIVATIVE, &deactiveData);
}


bool XModuleMyControl::IsCurrentGuard()
{
	if(m_pActionFSM->GetCurrentStateID() == ACTION_STATE_GUARD)
		return true;
	
	return false;
}

void XModuleMyControl::OnGuardKnockBack(vec3& vDir)
{
	// 현재 가드 액션 상태라면... 넉백이라는 정보를 알려서 가드를 푼다.
	XEvent msg(XEVTL_GUARD_KNOCKBACK);
	m_pActionFSM->Event(msg);

	XModuleActorControl::OnGuardKnockBack(vDir);
}

bool XModuleMyControl::IsPostPosSync()
{
	XMyActionState* pCurrentActionState = GetCurrentActionState();

	// 싱크 포지션으로 보내야 하는 것들
	if((pCurrentActionState &&
		((pCurrentActionState->GetID() == ACTION_STATE_GUARD &&	IsCurrentMotionGuardKnockback() == true) ||		// 가드 넉백시 모션에서는 싱크 포지션으로 보낸다.
		pCurrentActionState->GetID() == ACTION_STATE_CHANGE_STANCE ||											// 무기 체인지시... 
		pCurrentActionState->GetID() == ACTION_STATE_JUMP)) ||
		m_pModuleMovable->GetMovableFactor().IsMoveDummyNode())													// 더미 노드에 의한 움직임이면...
	{	
		return true;
	}

	return false;
}

void XModuleMyControl::DoGrappledEnd()
{
	if (GetCurrentActionStateID() != ACTION_STATE_GRAPPLED) return;

	XMyActionGrappled* pActionState = static_cast<XMyActionGrappled*>(m_pActionFSM->GetCurrentState());
	pActionState->FinishGrappled();
}

void XModuleMyControl::OnHitByBuff( TD_BUFF_HIT* pBuffHitInfo )
{
	// 탤런트 취소
	if (m_pModuleTalent->IsActive())
	{
		if ( m_pModuleTalent->GetInfo()->m_bCancelable && 
			XTalentHelper::IsTalentCancelableMotionFactor((MF_STATE)pBuffHitInfo->nMFState) )
		{
			// 사용중이던 탈랜트 사용중지
			m_pModuleTalent->Cancel();
		}
	}

	// 모션 팩터
	XTalentHitParam * pHitInfo			= new XTalentHitParam();

	pHitInfo->nAttackerUIID				= UIID_INVALID;
	pHitInfo->vServerKnockbackDir		= vec3::ZERO;
	pHitInfo->vServerKnockbackVictimPos = vec3::ZERO;
	pHitInfo->nTalentID			= 0;
	pHitInfo->nMFState					= MF_STATE(pBuffHitInfo->nMFState);
	pHitInfo->nMFWeight					= pBuffHitInfo->nMFWeight;

	HitByTalent(pHitInfo);

	SAFE_DELETE(pHitInfo);
}

bool XModuleMyControl::StartGrappled( _DamageRepeatInfo* pInfo )
{
	if ( GetCurrentActionStateID() == ACTION_STATE_GRAPPLED)
	{
		XEvent evt( XEVTL_GRAPPLED_NEW_START, (void*)( pInfo));
		m_pActionFSM->Event( evt);
	}
	else
	{
		DoAction(ACTION_STATE_GRAPPLED, (void*)( pInfo));

		// 확인
		if ( GetCurrentActionStateID() != ACTION_STATE_GRAPPLED)
		{
			return false;
		}
	}

	m_bDamageRepeat = true;

	return true;
}

bool XModuleMyControl::StartSwallowed( _DamageRepeatInfo* pInfo )
{
	if ( GetCurrentActionStateID() == ACTION_STATE_SWALLOWED)
	{
		XEvent evt( XEVTL_SWALLOWED_NEW_START, (void*)( pInfo));
		m_pActionFSM->Event( evt);
	}
	else
	{
		DoAction(ACTION_STATE_SWALLOWED, (void*)( pInfo));

		// 확인
		if ( GetCurrentActionStateID() != ACTION_STATE_SWALLOWED)
		{
			return false;
		}
	}

	m_bDamageRepeat = true;

	return true;
}

void XModuleMyControl::OnGuarded()
{
	// 현재 가드 액션 상태라면... 가드에대한 정보를 알려서 처리하자
	XEvent msg(XEVTL_GUARDED);
	m_pActionFSM->Event(msg);
}

void XModuleMyControl::UseRefillPotion()
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItemByID(XCONST::REFILL_POTION_HP_ITEMID);
	if( pItem == NULL)
	{
		pItem = gvar.MyInfo.Inventory.GetItemByID(XCONST::REFILL_POTION_SP_ITEMID);
		if( pItem == NULL)
		{
			pItem = gvar.MyInfo.Inventory.GetItemByID(XCONST::REFILL_POTION_MP_ITEMID);
			if( pItem == NULL)	return;
		}
	}

	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseItem(pItem->m_nSlotID);
}

void XModuleMyControl::InitFallingStartZ()
{
	m_pModuleMovable->InitMovableFactorFalling();
}

void XModuleMyControl::_UpdateMyCharacterAlpha()
{
	if (global.camera == NULL || global.camera->GetCamera() == NULL) return;
	if (gg.controller->IsMainBackViewCamera())
	{
		vec3 vCameraPos = global.camera->GetCamera()->GetPosition();
		vec3 vCharacterPos = m_pModuleEntity->GetPosition();

		vCharacterPos.z += XCONST::CAMERA_TARGET_HEIGHT;

		const float fCameraDist = vCameraPos.DistanceTo(vCharacterPos);

		m_CharacterAlphaMgr.Check(fCameraDist, m_pModuleEntity);
	}
	//else if(global.camera->IsPrimaryCameraType(CAMERA_INTERACTION))
	//{
	//	// 인터렉션은 npc와 카메라 사이에 있으면 알파처리
	//	XObject* pNPC = gg.omgr->Find(XGetInteractionInfo().InteractionTargetUID);
	//	if(pNPC == NULL)
	//		return;

	//	//vec3 vCameraPos = global.camera->GetCamera()->GetPosition();
	//	vec3 vNpcPos = pNPC->GetPosition();
	//	vNpcPos.z += XCONST::CAMERA_TARGET_HEIGHT;
	//	vec3 vCharacterPos = m_pModuleEntity->GetPosition();
	//	vCharacterPos.z += XCONST::CAMERA_TARGET_HEIGHT;

	//	//float fCameraDistToNPC = vCameraPos.DistanceTo(vNpcPos);
	//	//float fCameraDistToPlayer = vCameraPos.DistanceTo(vCameraPos);
	//	//if(fCameraDistToNPC > fCameraDistToPlayer)
	//	//	fCameraDistToPlayer = m_CharacterAlphaMgr.CONST_ALPHA_VALUE() - 10.0f;
	//	float fPlayerDistToNPC = vCharacterPos.DistanceTo(vNpcPos);
	//	if(60.0f > fPlayerDistToNPC)
	//		fPlayerDistToNPC = m_CharacterAlphaMgr.CONST_ALPHA_VALUE() - 10.0f;
	//		
	//	m_CharacterAlphaMgr.Check(fPlayerDistToNPC, m_pModuleEntity);
	//}
}

void XModuleMyControl::DoActionGesture( const wchar_t* szMotionName )
{
	DoAction(ACTION_STATE_GESTURE, (void*)szMotionName);
}

void XModuleMyControl::OnGuardFailed()
{
	XEvent msg(XEVTL_GUARD_FAILED);
	m_pActionFSM->Event(msg);
}

void XModuleMyControl::SetMouseMove( vec3& vMovePos )
{
	if(GetMyPlayerOwner()->IsDead())
		return;

	if(m_bMouseAutoMove == false)
	{
		// 일단 정지
		Stop();
	}

	// 액션 상태 체크
	const int nActionFSMStateID = GetCurrentActionStateID();
	if(nActionFSMStateID == ACTION_STATE_HOLD)
		return;

	// 이동 방향
	vec3 vMoveDir = (vMovePos - m_pOwner->GetPosition());
	float fMoveLen = vMoveDir.Length();

	m_vCharacterFrontMoveDir = vMoveDir.Normalize();

	// 이동 플래그(전진)
	m_nMovementFlags = MOVEMENTFLAG_FORWARD;

	// 이동 시간
	const float fSpeedRatio = GetExtSpeedRatio();
	float fMoveSpeed = GetMyPlayerOwner()->GetMoveSpeed( m_nMovementFlags) * fSpeedRatio;
	float fMoveTime = fMoveLen / fMoveSpeed + 3.0f;

	// 이동 데이터
	XMovableGoParam* pGoParam = new XMovableGoParam(XMovableGoParam::E_GO_MOUSE_MOVE);
	XMovableFactor::SInterpolation param = m_pModuleMovable->GetMovableFactor().INTERPOLATION;
	pGoParam->SetMoveCheckType(MCT2_MOUSE_MOVE);
	pGoParam->Set(vMovePos, m_vCharacterFrontMoveDir, fMoveSpeed, 200.0f, BOOST_NONE, m_nMovementFlags & 0x000F, true, fMoveTime);

	m_pModuleAction->SetMyMovementActionByMouseMove(pGoParam);

	m_bMouseAutoMove = true;
	m_vMouseAutoMove = vMovePos;

	delete pGoParam;
}

void XModuleMyControl::MouseMoveClick( MPoint& pt )
{
	// 액션 상태 검사
	const int nActionFSMStateID = GetCurrentActionStateID();
	if (nActionFSMStateID != ACTION_STATE_IDLE ||
		CheckMouseMovable() == false) 
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	vec3 vMovePos = vec3::ZERO;
	if(m_pMouseMove->OnClick(pt.x, pt.y, vMovePos) && vMovePos != vec3::ZERO)
	{
		m_bAutoRun = false;
		SetMouseMove(vMovePos);
	}
}

void XModuleMyControl::ChangeIndicatorEffectPos()
{
	m_pTargetIndicator->ChangeIndicatorEffectPos();  
}

void XModuleMyControl::SetTargetIndicatorToggle( bool bShow )
{
	m_pTargetIndicator->SetShow(bShow);
}

bool XModuleMyControl::IsHelpMessage()
{
	unsigned long ltime = timeGetTime();
	if (XGetInteractionInfo().stTargetActorInfo.nLockOnTime + TARGET_INFO_REFRESH_TIME > ltime)
	{
		return true;
	}

	return false;
}

bool XModuleMyControl::IsCurrentGrappled()
{
	if(m_pActionFSM->GetCurrentStateID() == ACTION_STATE_GRAPPLED ||
		m_pActionFSM->GetCurrentStateID() == ACTION_STATE_SWALLOWED)
		return true;

	return false;
}

void XModuleMyControl::CheckMouseMoveState()
{
	m_nMovementFlags = MOVEMENTFLAG_FORWARD;

	const XMovableFactor& movableFactor = m_pModuleMovable->GetMovableFactor();

	// 도착 했는가? 아님 충돌인가?
	if(movableFactor.INTERPOLATION.m_bArrived == true ||
		movableFactor.MOUSE_MOVE.m_bColEnd == true)
	{
		InitMouseMove();
		m_nMovementFlags = 0;
	}
	else if (m_pModuleAction->GetCurrentMotionName() == MOTION_NAME_IDLE &&
		m_vMouseAutoMove != vec3::ZERO)
	{
		// 중간에 무슨일로 Idle로 되었다면...
		SetMouseMove(m_vMouseAutoMove);
	}
	else
	{
		// 속도 체크
		const float fSpeedRatio = GetExtSpeedRatio();
		float fMoveSpeed = GetMyPlayerOwner()->GetMoveSpeed( m_nMovementFlags) * fSpeedRatio;

		if(fMoveSpeed != movableFactor.INTERPOLATION.m_fMoveSpeed)
		{
			SetMouseMove(m_vMouseAutoMove);
		}
	}
}

void XModuleMyControl::OnUseTalent( int nTalentID, vec3& vPos, vec3& vDir )
{
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo( nTalentID, m_pOwner);
	if ( pTalentInfo && pTalentInfo->m_nMovableType == MUT_NONE)
	{
		InitMouseMove();
		m_bAutoRun = false;
	}

	XModuleActorControl::OnUseTalent(nTalentID, vPos, vDir);
}

bool XModuleMyControl::CheckActionEventAcceptableByMouseVisible( XEvent* pEvent /*= NULL*/ )
{
	// 마우스 포인터가 활성화 되어 있는데... 사용 가능한 액션 이벤트
	// 제외(VKEY_FORWARD, VKEY_BACKWARD, VKEY_LEFT, VKEY_RIGHT, VKEY_ACTION, VKEY_JUMP, VKEY_ACTION2) 이 액션은... 알아서 통과한다.

	if (pEvent == NULL ||
		pEvent->m_pData == NULL)
		return false;

	XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(pEvent->m_pData);
	if(nVirtualKey == VKEY_CHANGE_WEAPON ||
		nVirtualKey == VKEY_SIT ||
		nVirtualKey == VKEY_CHANGE_STANCE ||
		nVirtualKey == VKEY_NPCINTERACTION	||
		nVirtualKey == VKEY_PCINTERACTION ||
		nVirtualKey == VKEY_FORWARD ||
		nVirtualKey == VKEY_BACKWARD ||
		nVirtualKey == VKEY_LEFT ||
		nVirtualKey == VKEY_RIGHT)
		return true;

	return false;
}

void XModuleMyControl::InitAutoMove()
{
	InitMouseMove();
	m_bAutoRun = false;
}

bool XModuleMyControl::IsMoving()
{
	return ( m_nMovementFlags == MT_DEFAULT  ?  false : true);
}

bool XModuleMyControl::IsJumping()
{
	return ( MOVEMENTFLAG_JUMPING & m_nMovementFlags ? true : false );
}

void XModuleMyControl::Init_ForChangeField()
{
	InitStand();
	InitAutoMove();

	m_bCurrentFreeLook = false;
	m_bFallingStart = false;

	m_pSensorController->Clear();

	m_pModuleBuff->Init_ForChangeField();

	DoForceCameraLock();
}

void XModuleMyControl::Final_ForChangeField()
{
	m_pSensorController->Clear();

	SetPause( true);
	DoGrappledEnd();
	InitFallingStartZ();
}

void XModuleMyControl::RequestCancel_PreInteraction()
{
	if (gvar.Game.bTradeReq == true)
	{
		XPostTrade_StartAccept(false);
		gvar.Game.bTradeReq = false;
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "CANCELED");
	}
	else if (gvar.Game.bPartyReq == true)
	{
		XPostPartyRequestInviteQuestionRespond(false);
		gvar.Game.bPartyReq = false;
		global.script->GetGlueGameEvent().OnGameEvent( "PARTY", "CANCELED");
	}
	else if (gvar.Game.bGuildReq == true)
	{
		XPostGuild_Invite_Res(false);
		gvar.Game.bGuildReq = false;
		global.script->GetGlueGameEvent().OnGameEvent( "GUILD", "CANCELED");
	}
	else if (gvar.Game.bDuelReq == true)
	{
		XPostDuel_QuestionRespond( false);
		gvar.Game.bDuelReq = false;
		global.script->GetGlueGameEvent().OnGameEvent( "DUEL", "CANCELED");
	}
}

//void XModuleMyControl::_UpdateLightEffectPos(vec3 vCameraDir)
//{
//	if(m_uidCharacterLightEffect == MUID::ZERO)
//		return;
//
//	if(vCameraDir == vec3::ZERO)
//		return;
//
//	XLightEffect* pCharacterLightEffect = global.emgr->GetXLightEffect(m_uidCharacterLightEffect);
//
//	if(pCharacterLightEffect == NULL)
//	{
//		if(CreateCharacterLightEffect() == false)
//			return;
//
//		pCharacterLightEffect = global.emgr->GetXLightEffect(m_uidCharacterLightEffect);
//	}
//
//	vec3 vLightEffectPos = m_pOwner->GetPosition();
//	vLightEffectPos.z += XCONST::CAMERA_TARGET_HEIGHT;
//
//	vLightEffectPos += -vCameraDir * XCONST::CONST_CAMERA_LIGHT_EFFECT_DISTANCE;
//
//	MMatrix matWorld;
//	matWorld.SetLocalMatrix(vLightEffectPos, vec3::AXISY, vec3::AXISZ);
//
//	pCharacterLightEffect->SetWorldMatrix(matWorld);
//	pCharacterLightEffect->SetRadius(XCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS);
//	pCharacterLightEffect->SetColorRGB(XCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR);
//}

//bool XModuleMyControl::CreateCharacterLightEffect()
//{
//	if(XCONST::CONST_CAMERA_LIGHT_EFFECT_ENABLE == false)
//		return false;
//
//	m_uidCharacterLightEffect = global.emgr->AddXLightEffect(NULL, wstring(L""), XCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS, XCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR, 0.0f, false);
//
//	return (m_uidCharacterLightEffect != MUID::ZERO);
//}

void XModuleMyControl::_UpdateRestrictedArea( float fDelta )
{
	if(gg.currentgamestate->GetWorld() && gg.currentgamestate->GetWorld()->GetInfo())
	{
		CSFieldFileInfo* pFieldFileInfo = info.field->GetFieldList().Find(gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID);
		m_RestrictedAreaChecker._UpdateRestrictedArea(fDelta, m_pModuleEntity->GetPosition(), gg.currentgamestate->GetWorld()->GetInfo(), pFieldFileInfo);
	}
}

void XModuleMyControl::OnHeal( int nAmount, UIID nUserUIID /*= UIID_INVALID*/ )
{
	gvar.MyInfo.SetHP(gvar.MyInfo.GetHP() + nAmount);

	XModuleActorControl::OnHeal(nAmount, nUserUIID);
}

void XModuleMyControl::InitSensorController(XGameState* pState)
{
	if (pState && pState->GetSensorManager())
	{
		m_pSensorController = pState->GetSensorManager()->GetController();
	}
	else
	{
		m_pSensorController = NULL;
	}
}

void XModuleMyControl::StartChallengerQuestExitTimer()
{
	if (m_pChallengerQuestExitChecker == NULL)
		m_pChallengerQuestExitChecker = new XChallengerQuestExitChecker;

	m_pChallengerQuestExitChecker->Start();
}

void XModuleMyControl::StopChallengerQuestExitTimer()
{
	if (m_pChallengerQuestExitChecker)
	{
		m_pChallengerQuestExitChecker->Stop();
		SAFE_DELETE(m_pChallengerQuestExitChecker);
	}
}

void XModuleMyControl::DoActionInteraction( MUID uidTarge )
{
	DoAction(ACTION_STATE_INTERACTION, &uidTarge);
}

bool XModuleMyControl::CheckAttackReturnAnimation()
{
	wstring strCurMotion = m_pOwner->GetModuleMotion()->GetCurrMotion();
	wstring strCurUpperMotion = m_pOwner->GetModuleMotion()->GetCurrUpperMotion();

	if(strCurMotion == MOTION_NAME_ATTACK_1_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_2_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_3_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_4_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_5_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_1F_RETURN ||
		strCurMotion == MOTION_NAME_ATTACK_SWORDDRAWING ||
		strCurUpperMotion == MOTION_NAME_ATTACK_1_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_2_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_3_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_4_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_5_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_1F_RETURN ||
		strCurUpperMotion == MOTION_NAME_ATTACK_SWORDDRAWING)
	{
		return true;
	}

	return false;
}

void XModuleMyControl::HitByBuff( XTalentHitParam * pHitInfo )
{
	if(pHitInfo)
		HitByTalent(pHitInfo);
}

bool XModuleMyControl::DoActionEnchantUseItem( int nSlotID )
{
	return DoAction(ACTION_STATE_ENCHANT, &nSlotID);
}

void XModuleMyControl::DoActionMapObjInteraction(MUID uidSearch)
{
	DoAction(ACTION_STATE_MAPOBJ_INTERACTION, (void*)&uidSearch);
}

void XModuleMyControl::DoCameraLock( MUID& uidTarget, wstring& strBoneName, float& fDurationTime )
{
	if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
		if (pBackViewCamera)
		{
			pBackViewCamera->StartCameraLock(uidTarget, strBoneName, fDurationTime);
		}
	}
}

void XModuleMyControl::DoForceCameraLock()
{
	XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
	if (global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW) && pBackViewCamera)
	{
		pBackViewCamera->EndCameraLock();
	}
}

bool XModuleMyControl::CheckMouseMovable()
{
	// 움직일수 없는 상태인지 체크
	XBuffAttribute attrBuff = m_pModuleBuff->GetBuffAttribute();
	if ( attrBuff.IsMovable() == false)
		return false;

	return true;
}

void XModuleMyControl::DoActionSitDown()
{
	DoAction(ACTION_STATE_SIT, true);
}

void XModuleMyControl::DoActionSitRise()
{
	XEvent msg(XEVTL_SIT_RISE);
	m_pOwner->Event(msg);
}

void XModuleMyControl::SetAFK( bool bAFK )
{
	XMyPlayer* pMyPlayer = GetMyPlayerOwner();
	if (pMyPlayer->IsDead()) return;

	pMyPlayer->SetAFK(bAFK);

	if (bAFK)
	{
		DoActionSitDown();

		pMyPlayer->SetCharacterPane_Player();
		global.ui->OnSystemMsg(XGetStr(L"SMSG_AWAY_FROM_KEYBOARD_BEGIN"));
	}
	else
	{
		pMyPlayer->SetCharacterPane_Player();
		global.ui->OnSystemMsg(XGetStr(L"SMSG_AWAY_FROM_KEYBOARD_END"));
	}
}

bool XModuleMyControl::CheckGuardKnockback()
{
	// 현재 가드 넉백 상태인가?
	if(GetCurrentActionStateID() != ACTION_STATE_GUARD)
		return false;

	XMyActionGuard* pActionState = static_cast<XMyActionGuard*>(m_pActionFSM->GetCurrentState());
	return pActionState->IsGuardKnockback();
}

bool XModuleMyControl::IsSendablePositionPost()
{
	// 피격 대기 정보를 뒤져서 위치 정보를 날려도 되는지 봅니다.
	// 모션팩터가 있으면 보내지 마시오.
	vector<XEvent> vecEvent;
	m_pOwner->GetModuleMessageQueue()->GetEventQueueNode(XEVTD_TALENT_HIT, vecEvent);

	for(vector<XEvent>::iterator it = vecEvent.begin(); it != vecEvent.end(); ++it)
	{
		XTalentHitParam* pHitParam = static_cast<XTalentHitParam*>(it->m_pData);
		if(pHitParam && !CSMotionFactorHelper::IsNoneOrFakes(pHitParam->nMFState))
		{
			return false;
		}
	}

	return true;
}

void XModuleMyControl::ClearLockOnTarget( const UIID& uidObj )
{
	if( false == XGetInteractionInfo().stTargetActorInfo.bLockOnTarget )
		return ;

	if( uidObj != XGetInteractionInfo().GetTargetUIID() )
		return ;

	m_pTargetIndicator->Init();
	XGetInteractionInfo().SetTargetUIID(0);
	XGetInteractionInfo().stTargetActorInfo.bLockOnTarget = false;
	XGetInteractionInfo().stTargetActorInfo.nLockOnTime = timeGetTime();

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnTargetInfoRefresh();
		global.script->GetGlueGameEvent().OnEnemyInfoRefresh();
	}
}