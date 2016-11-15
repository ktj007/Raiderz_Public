#pragma once

// 무기 상태
static enum 
{
	WC_NONE = 1,
	WC_1HS	= (1<<1),
	WC_1HB	= (1<<2),
	WC_1HP	= (1<<3),
	WC_2HD	= (1<<4),
	WC_STF	= (1<<5),
	WC_ARC	= (1<<6),
	WC_2HB	= (1<<7),
	WC_DWD	= (1<<8),
	WC_DWP	= (1<<9),

	WC_ALL	= 0xFFFFFFFF
};

// 키 상태
static enum XConditionKeyState
{
	_PRESSED		= 1,			///< 눌렀는지
	_PRESSEDONCE	= (1<<1),		///< 한번만 눌렀는지
	_RELEASED		= (1<<2),		///< 떼었는지
	_KEY_FORWARD	= (1<<3),		///< 앞키를 함께 눌렀는지
	_KEY_DOWN		= (1<<4),		///< 뒤키를 함께 눌렀는지
	_KEY_LEFT		= (1<<5),		///< 왼쪽키를 함께 눌렀는지
	_KEY_RIGHT		= (1<<6),		///< 오른쪽키를 함께 눌렀는지
};

// 자세 상태
static enum XConditionStance
{
	_STANCE_NORMAL	= 1,
	_STANCE_BATTLE	= (1<<1),
	_STANCE_ALL		= 0xFFFFFFFF
};

// 내 액션 상태 전이 조건
struct XMyActionTransCondition
{
	XVirtualKey		nKey;				///< 키
	unsigned int	nKeyState;			///< 조합키에 사용할 상태(아직 안쓰임-조합키가 없음..-_-)
	unsigned int	nWeaponState;		///< 들고 있는 무기 타입(WEAPON_NONE이면 아무무기나 상관없음)
	unsigned int	nStances;			///< 자세
};

// 내 액션 전이
struct XMyActionTransition
{
	ACTION_STATE				nNextState;		///< 다음 상태
	XMyActionTransCondition		Condition;		///< 조건
};


class XKeyConditionChecker
{
private:
	vector<XMyActionTransition>		m_vecMyActionTransitions;
public:
	void AddCondition(XMyActionTransition& transition);
	ACTION_STATE CheckKeyCondition(XVirtualKey nVirtualKey, unsigned int nKeyState, CHAR_STANCE nCurrStance);
};
