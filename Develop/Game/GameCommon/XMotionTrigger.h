#pragma once

#include "MTypes.h"

enum XMotionTrigger_IfType
{
	MTRIGGER_IF_NONE = -1,
	MTRIGGER_IF_TRUE = 0,
	MTRIGGER_IF_DEAD,
	MTRIGGER_IF_MAX
};

enum XMotionTrigger_ThenType
{
	MTRIGGER_THEN_NONE = -1,
	MTRIGGER_THEN_CHANGE_MOTION = 0,
	MTRIGGER_THEN_HOLD,
	MTRIGGER_THEN_MAX
};

class XMotionTrigger
{
public:
	XMotionTrigger_IfType		m_nIfType;
	XMotionTrigger_ThenType		m_nThenType;
	tstring						m_strThenParam1;
	tstring						m_strThenParam2;

	XMotionTrigger() : m_nIfType(MTRIGGER_IF_NONE), m_nThenType(MTRIGGER_THEN_NONE) {}
	bool IsValid()
	{
		if (m_nIfType != MTRIGGER_IF_NONE && m_nThenType != MTRIGGER_THEN_NONE) return true;
		return false;
	}
};


