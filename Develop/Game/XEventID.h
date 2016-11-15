#ifndef _XMESSAGE_ID_H
#define _XMESSAGE_ID_H


/// 메세지 ID
/// - 메세지는 이벤트용으로만 사용합니다. 오브젝트에 명령을 메세지로 주지 않도록 합시다. - birdkr(2007-02-02)
enum XEventID
{
	XEVT_UNKNOWN = 0,

	// 로컬 전용 -------------------------------------------------
	XEVTL_VIRTUAL_KEY_PRESSED,			///< 키입력
	XEVTL_VIRTUAL_KEY_PRESSEDONCE,		///< 키입력 / 첫 눌렸을때 한번만 보냄
	XEVTL_VIRTUAL_KEY_RELEASED,			///< 키입력 해제

	// 애니메이션
	XEVTL_MOTION_PLAYONCE,				///< 에니메이션 한번 플레이 완료됨
	XEVTL_UPPER_MOTION_PLAYONCE,		///< 상체 에니메이션 한번 플레이 완료됨

	XEVTL_ON_MOTION_EVENT,
	XEVTL_ON_CHANGE_MOTION,				///< 모션이 바뀌었을 때
	XEVTL_ON_CHANGE_ANIMATION,			///< 모션안에서 애니메이션이 바뀔때
	XEVTL_ON_CHANGED_MOTION_TO_IDLE,	///< 모션이 IDLE로 바뀌었음..

	// 액션
	XEVTL_ON_CAN_CANCEL_ACTION,			///< 액션을 캔슬 할 수 있을때
	XEVTL_ON_RELEASE_TALENT_KEY,		///< 탤런트 사용키 뗌

	XEVTL_JUMP_FAILED,					///< 점프 실패
	XEVTL_SWIM_IN,						///< 수영하러 들어간다.
	XEVTL_SWIM_OUT,						///< 수영중 지형과 충돌하여 밖으로 나간다.

	// 탤런트 관련
	XEVTL_TALENT_START,					///< 탤런트 시작
	XEVTL_TALENT_CANCELED,				///< 탤런트 취소
	XEVTL_TALENT_ACT_SPELL,				///< 마법 발사
	XEVTL_TALENT_RECASTING,				///< 캐스팅 준비

	XEVTL_USE_ITEM,						///< 아이템 사용

	// MyAction 관련
	XEVTL_MYACTION_RELEASE_ACTION_HOLD,		///< Hold인 MyAction 상태를 Idle로 전이시켜라.
	XEVTL_MYACTION_SWITCHING_WEAPON_BEGIN,	///< 무기 변경 시작
	XEVTL_MYACTION_SWITCHING_WEAPON_START,	///< 무기 변경
	XEVTL_MYACTION_USE_TALENT,				///< 탤런트를 사용하라
	XEVTL_MYACTION_NPC_INTERACTION_END,			///< 인터렉션 종료

	XEVTL_LOOT_END_READY,				///< 루팅 종료 준비 완료(서버에게 메세지를 보냈다.)
	XEVTL_LOOT_END,						///< 루팅 종료
	XEVTL_LOOT_GETITEM,					///< 아이템요청
	XEVTL_LOOT_GETALLITEM,				///< 모든아이템요청

	XEVTL_CHANGE_STANCE,				///< 스탠스를 변경함
	XEVTL_CHANGED_STANCE,				///< 스탠스가 변경됨

	XEVTL_INTERACTION_CLICK,			///< 화면에 마우스 클릭
	XEVTL_MOUSE_MOVE_CLICK,				///< 화면에 마우스 클릭, 이동처리

	// 잡기 관련
	XEVTL_THROW,				///< 던져지는 모션이 시작됨
	XEVTL_GRAPPLED_TERRAIN_COL,			///< 땅체 충돌했다.
	XEVTL_GRAPPLED_NEW_START,			///< 잡기를 다시 시작한다(이미 잡기 상태가 끝나지 않았는데 잡기를 당한 경우)

	//XEVTL_HOLD_KEYABLE,				///< 인터랙션커맨드에 대한 응답이 오면 키프레스가 먹도록

	XEVTL_MOTION_PLAYONCE_NPC_DIE,		///< NPC_DIE 애니메이션이 완료됨.
	XEVTL_DIE_ANI_END,					///< 다이 애니메이션 완료되었음...(XModuleMotion에서 처리하는 이벤트가 아니라... XActionDie에서 알리는 이벤트)

	XEVTL_CRAFTING_REQ,					///< 크래프팅 요청.

	// 예외처리 발생
	XEVTL_EXPCEPTION_HOLD,				///< 끼었을때 예외처리 발생

	// 가드
	XEVTL_GUARD_KNOCKBACK,				///< 가드 넉백이다
	XEVTL_GUARDED,						///< 가드이다
	XEVTL_GUARD_FAILED,					///< 가드 실패

	// 먹기 관련
	XEVTL_SWALLOWED_NEW_START,			///< 먹기를 다시 시작한다(이미 잡기 상태가 끝나지 않았는데 잡기를 당한 경우)
	XEVTL_SWALLOWED_DISAPPEAR,			///< 먹혀서 사라지는 시간...

	// 앉기 관련
	XEVTL_SIT_RISE,
	
	// 강화
	XEVTL_ENCHANT_ITEM_SELECT,			///< 강화 아이템 선택
	XEVTL_ENCHANT_START,				///< 강화 시작
	XEVTL_ENCHANT_CANCEL,				///< 강화 취소
	XEVTL_ENCHANT_CHECK_REQ,			///< 강화 체크
	XEVTL_ENCHANT_CHECK_SUCCESS,		///< 강화 체크 성공
	XEVTL_ENCHANT_RESULT,				///< 강화 결과

	//SoulHunterZ - Item Attunement
	XEVTL_ATTUNE_ITEM_SELECT,
	XEVTL_ATTUNE_START,
	XEVTL_ATTUNE_CANCEL,
	XEVTL_ATTUNE_CHECK_REQ,
	XEVTL_ATTUNE_CHECK_SUCCESS,
	XEVTL_ATTUNE_RESULT,

	//염색
	XEVTL_DYE_ITEM_SELECT,				///< 염색 아이템 선택
	XEVTL_DYE_START_ITEM,				///< 염색 시작
	XEVTL_DYE_END_ITEM,					///< 염색 완료
	XEVTL_DYE_CANCEL,					///< 염색 취소

	// PvP 카메라 관려
	XEVTL_CAMERA_PVP_VIEW_CLEAR	,		///< PvP 프리 카메라 해제

	// Net 관련
	XEVTL_NET_MOVE,						///< NetPlayer 이동

	XEVTL_INTERPOLATION_MOVE_ARRIVED,	///< 보간 이동 도착 메세지

	// 지연 메세지--------------------------------------------------------
	XEVTD_TALENT_HIT,					///< 스킬에 맞음(지연이 있어 메세지로 처리함) (인자 - XTalentInfo*)
	XEVTD_GUARD_DEFENSE,				///< 가드함(지연이 있어 메세지로 처리함) (인자 - _DelayedGuardDefenseInfo*)

	XEVTD_END_MF_STUN,					///< 스턴 모션팩터 종료
	XEVTD_END_MF_KNOCKDOWN,				///< 넉다운 모션팩터 종료

	XEVTD_DISAPPEAR_ME,					///< 천천히 사라진다.

	XEVTD_MESH_LOADING_COMPLETE,		///< 모델 메쉬 로딩 완료

	XEVTD_NPC_SAY_END,					///< NPC SAY END - 말풍선 끝날때

	XEVTD_ACTION_SLEEP,					///< 

	XEVT_MAX = 65535
};







#endif // #ifndef _XMESSAGE_ID_H