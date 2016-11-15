#ifndef _XVIRTUAL_KEY_H
#define _XVIRTUAL_KEY_H

#include <map>
using namespace std;

// 이걸 추가하거나 변경하면 XKeyMapTable.cpp의 InitVirtualKeyString()에도 변경해줘야 합니다.
const enum XVirtualKey
{
	VKEY_NA				= 0,
	VKEY_FORWARD,					///< 앞으로 이동
	VKEY_BACKWARD,					///< 뒤로 이동
	VKEY_LEFT,						///< 왼쪽으로 이동
	VKEY_RIGHT,						///< 오른쪽으로 이동
	VKEY_JUMP,						///< 점프
	VKEY_CHANGE_STANCE,				///< 스탠스 변경
	VKEY_ACTION,					///< 액션,기본 공격
	VKEY_ACTION2,					///< 두번째 액션, 기본은 방어
	VKEY_AUTO_RUN,					///< Auto Run
	VKEY_CHANGE_WEAPON,				///< 무기 변경
	VKEY_NPCINTERACTION,			///< NPC 인터랙션
	VKEY_PCINTERACTION,				///< PC 인터랙션
	VKEY_LOCKTARGET,				///< 타겟 고정
	VKEY_TOGGLE_MOUSE,				///< 마우스 토글
	VKEY_SIT,						///< 앉기
	VKEY_TOGGLE_FREECAMERA,			///< 프리 카메라
	VKEY_GUARD,						///< 가드
	VKEY_DODGE,						///< 회피

	// 인터페이스용
	VKEY_UI_NA,						///< UI
	VKEY_UI_MAINMENU,				///< 메인 메뉴창
	VKEY_UI_CHARACTER,				///< 캐릭터 창
	VKEY_UI_INVENTORY,				///< 인벤토리 창
	VKEY_UI_TALENT,					///< 텔런트 창
	VKEY_UI_JOURNAL,				///< 저널창
	VKEY_UI_SOCIAL,					///< 파티창
	VKEY_UI_MAP,					///< 메뉴창
	VKEY_UI_CONTROLSHELP1,			///< 조작키 도움말 1
	VKEY_UI_CONTROLSHELP2,			///< 조작키 도움말 2

	VKEY_UI_PALETTE0,
	VKEY_UI_PALETTE1,
	VKEY_UI_PALETTE2,
	VKEY_UI_PALETTE3,
	VKEY_UI_PALETTE4,
	VKEY_UI_PALETTE5,
	VKEY_UI_PALETTE6,
	VKEY_UI_PALETTE7,
	VKEY_UI_PALETTE8,
	VKEY_UI_PALETTE9,

	VKEY_UI_PALETTE_SET0,
	VKEY_UI_PALETTE_SET1,
	VKEY_UI_PALETTE_SET2,
	VKEY_UI_PALETTE_SET3,
	VKEY_UI_PALETTE_SET4,

	VKEY_COUNT		= 128			///< 가상키는 128개를 넘을 수 없습니다.
};



#endif