#include "stdafx.h"
#include "XActionKeyDef.h"

// 키 정의
XActionKeyDef::XActionKeyDef()
{
	// SPECIAL_KEY_ADVANCE_ATTACK
	DefSeqKey(SPECIAL_KEY_ADVANCE_ATTACK,	2, VKEY_FORWARD, VKEY_ACTION);

	// DODGE
	DefSeqKeyExt(SPECIAL_KEY_DODGE_FRONT,	VKEY_ACTION2, 1, VKEY_FORWARD);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_BACK,	VKEY_ACTION2, 1, VKEY_BACKWARD);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_LEFT,	VKEY_ACTION2, 1, VKEY_LEFT);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_RIGHT,	VKEY_ACTION2, 1, VKEY_RIGHT);

	DefSeqKeyExt(SPECIAL_KEY_DODGE_FRONT,	VKEY_FORWARD, 1, VKEY_ACTION2);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_BACK,	VKEY_BACKWARD, 1, VKEY_ACTION2);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_LEFT,	VKEY_LEFT, 1, VKEY_ACTION2);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_RIGHT,	VKEY_RIGHT, 1, VKEY_ACTION2);

	DefSeqKeyExt(SPECIAL_KEY_DODGE_FRONT,	VKEY_FORWARD, 1, VKEY_DODGE);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_BACK,	VKEY_BACKWARD, 1, VKEY_DODGE);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_LEFT,	VKEY_LEFT, 1, VKEY_DODGE);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_RIGHT,	VKEY_RIGHT, 1, VKEY_DODGE);

	DefSeqKeyExt(SPECIAL_KEY_DODGE_FRONT,	VKEY_DODGE, 1, VKEY_FORWARD);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_BACK,	VKEY_DODGE, 1, VKEY_BACKWARD);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_LEFT,	VKEY_DODGE, 1, VKEY_LEFT);
	DefSeqKeyExt(SPECIAL_KEY_DODGE_RIGHT,	VKEY_DODGE, 1, VKEY_RIGHT);

	// SPECIAL_KEY_UPPER
	DefSeqKey(SPECIAL_KEY_UPPER,			2, VKEY_BACKWARD, VKEY_ACTION);
	DefSeqKey(SPECIAL_KEY_UPPERCOMBO,		3, VKEY_BACKWARD, VKEY_FORWARD, VKEY_ACTION);

	// SPECIAL_KEY_SWORD_DRAWING
	DefSeqKeyExt(SPECIAL_KEY_SWORD_DRAWING,	VKEY_ACTION2, 1, VKEY_ACTION);

	DefSeqKey(SPECIAL_KEY_QUICK_RISE,		4, VKEY_LEFT, VKEY_RIGHT, VKEY_LEFT, VKEY_RIGHT);
	DefSeqKey(SPECIAL_KEY_COUNTER_ATTACK,	4, VKEY_LEFT, VKEY_RIGHT, VKEY_FORWARD, VKEY_ACTION);

	DefSeqKey(SPECIAL_KEY_GUARD,			1, VKEY_GUARD);

	const float SEQUENCE_TIME = 0.3f;		// 0.15초안에 

	for (size_t i = 0; i < m_Table.size(); i++)
	{
		int nSequenceLength = (int)m_Table[i].sequenceKeys.size();

		float time = SEQUENCE_TIME;
		int nLen = nSequenceLength - 3;
		if ( nLen > 0)
		{
			time += 0.1f * nLen;
		}

		if (m_Table[i].sequenceKeys.size() > 1)
		{
			m_Table[i].m_fTotalTime = time;
		}
		else
		{
			m_Table[i].m_fTotalTime = 0.001f;
		}
	}
}

void XActionKeyDef::DefSeqKey( SPECIAL_ACTION_COMMAND nSpecialKey, int nKeyCount, XVirtualKey nKey, ... )
{
	XActionKey newActionKey;
	newActionKey.nSpecialActionCommand = nSpecialKey;

	va_list marker;
	XVirtualKey key = nKey;

	va_start( marker, nKey );
	for (int i = 0; i < nKeyCount; i++)
	{
		newActionKey.sequenceKeys.push_back(key);
		key = va_arg( marker, XVirtualKey);
	}
	va_end( marker );

	m_Table.push_back(newActionKey);
}

void XActionKeyDef::DefSeqKeyExt( SPECIAL_ACTION_COMMAND nSpecialKey, XVirtualKey nExtKey, int nKeyCount, XVirtualKey nKey, ... )
{
	XActionKey newActionKey;
	newActionKey.nSpecialActionCommand = nSpecialKey;

	newActionKey.nExtKey = nExtKey;

	va_list marker;
	XVirtualKey key = nKey;

	va_start( marker, nKey );
	for (int i = 0; i < nKeyCount; i++)
	{
		newActionKey.sequenceKeys.push_back(key);
		key = va_arg( marker, XVirtualKey);
	}
	va_end( marker );

	m_Table.push_back(newActionKey);
}