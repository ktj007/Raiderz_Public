#pragma once

#include "XKeyBuffer.h"
#include "XMyActionID.h"

/// 내 액션 상태
enum SPECIAL_ACTION_COMMAND
{
	SPECIAL_KEY_NONE				= -1,
	SPECIAL_KEY_DODGE_FRONT			= 0,		///< 앞쪽 회피
	SPECIAL_KEY_DODGE_BACK,						///< 뒤쪽 회피
	SPECIAL_KEY_DODGE_LEFT,						///< 왼쪽 회피
	SPECIAL_KEY_DODGE_RIGHT,					///< 오른쪽 회피
	SPECIAL_KEY_ADVANCE_ATTACK,					///< 전진 공격
	SPECIAL_KEY_DOWNATTACK,						///< 하단 공격
	SPECIAL_KEY_UPPER,							///< 띄우기 공격
	SPECIAL_KEY_SUPERATTACK,					///< 특수공격
	SPECIAL_KEY_UPPERCOMBO,						///< 띄우기 후 콤보
	SPECIAL_KEY_SWORD_DRAWING,					///< 발도
	SPECIAL_KEY_QUICK_RISE,						///< 빠른 기상
	SPECIAL_KEY_COUNTER_ATTACK,					///< 방어후 공격
	SPECIAL_KEY_GUARD,							///< 방어

	SPECIAL_KEY_MAX
};

// 액션키 정의
struct XActionKey
{
	SPECIAL_ACTION_COMMAND		nSpecialActionCommand;
	float						m_fTotalTime;	// 조합키가 다 눌러져야 하는 제한 시간
	vector<XVirtualKey>			sequenceKeys;	// 조합키
	XVirtualKey					nExtKey;		// 특수 키 누름 상태 체크

	size_t						GetKeyCount() 
	{
		return sequenceKeys.size();
	}
	XActionKey() : nSpecialActionCommand(SPECIAL_KEY_NONE), nExtKey(VKEY_NA)
	{
	}
};


class XActionKeyDef
{
protected:
	vector<XActionKey>		m_Table;

	void DefSeqKey(SPECIAL_ACTION_COMMAND nSpecialKey, int nKeyCount, XVirtualKey nKey, ... );
	void DefSeqKeyExt(SPECIAL_ACTION_COMMAND nSpecialKey, XVirtualKey nExtKey, int nKeyCount, XVirtualKey nKey, ... );
public:
	XActionKeyDef();
	~XActionKeyDef() {}

	XActionKey& GetTable(int nIndex) { return m_Table[nIndex]; }
	size_t GetSize() { return m_Table.size(); }
};