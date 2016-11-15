#pragma once

#include "XMotion.h"

class XActor;
class XMotion;

class XMotionEventListener : public XBaseMotionEventLister
{
public:
	XMotionEventListener() {}
	virtual ~XMotionEventListener() {}
	virtual XMotion* OnPreEnter(XActor* pOwner)	{ return NULL; }	///< 이 모션으로 들어오기 전		- NULL이 아닌 값으로 반환하면 이 모션으로 변경됨
	virtual void OnEnter(XActor* pOwner)		{}					///< 이 모션으로 바꾼 후
	virtual void OnLeave(XActor* pOwner)		{}					///< 다른 모션으로 바꾼 후
	virtual void OnUpdate(XActor* pOwner)		{}					///< 업데이트
};
