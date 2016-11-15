#pragma once

#include "XMotionEventListener.h"

class XMotionEventLister_NPC_IDLE : public XMotionEventListener
{
public:
	XMotionEventLister_NPC_IDLE() {}
	virtual ~XMotionEventLister_NPC_IDLE() {}
	virtual XMotion* OnPreEnter(XActor* pOwner);	///< 이 모션으로 들어오기 전		- false로 반환하면 이 모션으로 변경이 취소됨
};
