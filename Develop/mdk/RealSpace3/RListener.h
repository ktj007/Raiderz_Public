#pragma once

#include "RTypes.h"
#include <list>

namespace rs3 {

class RActor;
class RAnimationEvent;
class REffectSceneNode;
class RListener;

enum REVENT
{
	RE_NONE = 0,
	RE_SHADOW_CHANGE,
	RE_RENDERING_CHANGE,
	RE_ENVIRONMENT_CHANGE,
	RE_ENVIRONMENT_BLENDING,
	RE_DEVICE_LOST,
	RE_DEVICE_RESTORE,

	RE_END
};

//////////////////////////////////////////////////////////////////////////
// 이벤트 발생 객체
class RS_API REventBroadcaster
{
	std::list<RListener*>	m_listeners;
public:
	virtual ~REventBroadcaster();

	void	AddListener(RListener* pListener);
	void	RemoveListener(RListener* pListener);

	void	Notify();

};

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  이벤트 수신 객체
//
class RS_API RListener
{
public:

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	RListener(void)
	{
	}
	virtual ~RListener(void)
	{
	}

	virtual void					Update() {}


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 애니메이션 관련
	//
	virtual void					OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent) { }
};

}
