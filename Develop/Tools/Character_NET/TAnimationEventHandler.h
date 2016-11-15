#pragma once

#include <map>
#include <string>
#include "RListener.h"

using namespace std;

class rs3::RActor;
class rs3::REffectSceneNode;
class TAnimationEventHandler;
class TSoulHuntAnimationEventListener;

// [10/16/2007 madduck]  툴용이라 이쪽을 수정하게 되면 게임쪽에도 고쳐야 한다.
class TAnimationEventListener : public RListener, public multimap < string, TAnimationEventHandler* >
{
public:
	virtual ~TAnimationEventListener();

	void Clear();
	virtual void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent);
	void DeleteHandler(TAnimationEventHandler* pHandler);
};


class TAnimationEventHandler
{
public:
	TAnimationEventHandler(void);
	virtual ~TAnimationEventHandler(void);

	TAnimationEventListener::iterator m_itrator;

	string	m_strEventName;
	string	m_strSoundFileName;
    string	m_strEffectFileName;
	string	m_strEffectParent;
	virtual bool Do();

	REffectSceneNode* m_pEffectSceneNode;
};

/////////////// SoulHuntAnimationEventListener
typedef void(*ANIMATIONEVENTHANDLER) (TSoulHuntAnimationEventListener&, const RAnimationEvent* );
typedef map<string,ANIMATIONEVENTHANDLER>	ANIMATIONEVENTMAP;

struct stLoopAniEffect
{
	MUID				m_uid;
	string				m_string1;
	string				m_string2;
	string				m_string3;
};

class TSoulHuntAnimationEventListener : public TAnimationEventListener
{
private:
	bool CheckEffectLoop(RActor* pActor, string& strEffectFileName, string& strBoneName);
	void DeleteLoopAniEffect();
	void CheckDeleteLoopAniEffect(const RAnimationEvent *pEvent);

public:
	TSoulHuntAnimationEventListener(void);
	virtual ~TSoulHuntAnimationEventListener(void);

	virtual void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent);

	static map<string,ANIMATIONEVENTHANDLER> m_eventHandlers;

	static void OnAnimationEvent_walk(TSoulHuntAnimationEventListener& handler, const RAnimationEvent *pEvent);
	static void OnAnimationEvent_sound(TSoulHuntAnimationEventListener& handler, const RAnimationEvent *pEvent);
	static void OnAnimationEvent_custom(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);
	static void OnAnimationEvent_effect(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);
	static void OnAnimationEvent_camera(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);
	static void OnAnimationEvent_face_ani_start(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);
	static void OnAnimationEvent_face_ani_end(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);
	static void OnAnimationEvent_defence_start_sound(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent);

	REffectSceneNode* m_pEffectSceneNode;

	vector<stLoopAniEffect>		m_vLoopAniEffect;

	void StopAnimationEffect();
};
