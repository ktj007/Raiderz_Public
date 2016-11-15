#pragma once

#include "XMotion.h"
#include "XTestWrapper.h"

class FBaseMotion
{
public:
	FBaseMotion();
	virtual ~FBaseMotion()
	{

	}

	XTestInfoWrapper<XMotionMgr>		m_MotionMgrWrapper;
};