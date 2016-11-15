#pragma once

#include <map>
#include <string>
#include "RListener.h"

using namespace std;

class rs3::RActor;
class AnimationEventHandler;

class AnimationEventListener : public RListener, public multimap < string, AnimationEventHandler* >
{
public:
	virtual ~AnimationEventListener();

	void Clear();
	void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent);
	void DeleteHandler(AnimationEventHandler* pHandler);
};


class AnimationEventHandler
{
public:
	AnimationEventHandler(void);
	virtual ~AnimationEventHandler(void);

	AnimationEventListener::iterator m_itrator;

	string	m_strEventName;
	string	m_strSoundFileName;
    string	m_strEffectFileName;
	string	m_strEffectParent;

	virtual bool Do();

};

/////////////// SoulHuntAnimationEventListener
typedef void(*ANIMATIONEVENTHANDLER) (const RAnimationEvent* );
typedef map<string,ANIMATIONEVENTHANDLER>	ANIMATIONEVENTMAP;

class SoulHuntAnimationEventListener : public AnimationEventListener
{
public:
	SoulHuntAnimationEventListener(void);
	virtual ~SoulHuntAnimationEventListener(void);

	void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent);

	static map<string,ANIMATIONEVENTHANDLER> m_eventHandlers;

	static void OnAnimationEvent_walk(const RAnimationEvent *pEvent);
	static void OnAnimationEvent_effect(const RAnimationEvent *pEvent);
	static void OnAnimationEvent_sound(const RAnimationEvent *pEvent);
};
