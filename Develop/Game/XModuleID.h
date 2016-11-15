#ifndef _XMODULE_ID_H
#define _XMODULE_ID_H

enum ModuleID
{
	//----------------------------------------------------------------------------
	// 1.
	// 고정입니다. 순서 변경시 주의 바랍니다.
	//----------------------------------------------------------------------------
	// XModuleMessageQueue::OnUpdate(float fDelta) -> XObject::Event(XEvent & msg) -> XObject::OnEvent(XEvent & msg)
	// -> XModuleContainer::Event(XEvent & msg) -> XModule::Event(XEvent & msg) -> XModuleAction::OnEvent(XEvent msg)
	// -> XActor::DeadProcEnterField() -> XModuleAction::DoneDIe() -> XActionDie::DoneDie()
	// -> XModuleEntity::_SetUpVector(const MVector3 & up) -> XModuleEntity::_UpdateTransform()
	XMID_MESSAGE_QUEUE = 0,			// 시간지연후에 받아야 하는 메세지를 관리
	XMID_PLAYER_CONTROL,	// MyControl, NetControl의 공통적인 부분을 담당
	//XModuleMotion::OnUpdate(float fDelta) -> XMotionLogic::Update(float fDelta) -> MotionLogic::PlayDone()
	// -> XMotionLogic::SendEvent(XEventID nEventID=XEVTL_MOTION_PLAYONCE) -> XObject::Event(XEvent&)
	// -> XObject::OnEvent(XEvent&) -> XModuleContainer::Event(XEvent&) -> XModule::Event(XEvent&)
	// -> XModuleMyControl::OnEvent(XEvent&) -> XStateMachine<XModuleMyControl>::Event(XEvent&)
	// -> XMyActionNormalAttack::Event(XEvent&) -> XMyActionNormalAttack::doAttack(TALENT_NORMAL_ATTACK_TYPE nAttackType=NA_NORMAL2)
	// -> XModuleMyControl::SyncPlayerDirToCameraDir() -> XObject::SetDirection(const MVector3&)
	// -> XModuleEntity::SetDirection(const MVector3&) 과정을 통해 모션이 ENTITY보다 나중에 실행되면 Entity가 두번실행되어야합니다.
	XMID_MOTION,				// 오브젝트의 애니메이션, 모션을 관리
	// Move에 의한 위치설정 / Entity보다 먼저 불려야한다.
	XMID_MOVABLE,				// 이동 관리

	//----------------------------------------------------------------------------
	// 2.
	// 순서에는 관계가 없습니다.
	// 그러나 Entity 이전에 처리 해야 합니다.
	//----------------------------------------------------------------------------
	XMID_ACTION,				// 오브젝트의 액션을 관리 - 모션보다 상위 개념
	XMID_COLLISION,				// 충돌 관리
	// 탈랜트에 의한 이팩트추가.
	// 이팩트 추가에 의해 UpdateForRender가 결정될수 있으므로 Entity보다 먼저 불러준다.
	XMID_TALENT,				// 탤런트 처리
	XMID_BREAKABLEPARTS,		// 브레이커블 파츠
	XMID_BUFF,					// 버프
	XMID_SOUND,					// 사운드
	XMID_DATA,					// 데이타
	XMID_BGW,					// 백그라운드 워크

	//----------------------------------------------------------------------------
	// 3.
	// 애니메이션을 처리 합니다.
	//----------------------------------------------------------------------------
	XMID_ENTITY,				// 보여지는 것과 관련된 기초 모듈(pos, mesh, animation 관리)

	//----------------------------------------------------------------------------
	// 4.
	// 애니메이션 이후에 처리 해야 하는 것들...
	//----------------------------------------------------------------------------
	XMID_EFFECT,				//
	XMID_UI,					// Interaction
	XMID_POST,					// 커맨드 전송 관리

	MAX_MODULE_ID
};


#endif