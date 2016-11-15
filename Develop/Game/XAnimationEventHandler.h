#pragma once

#include "XModuleEntity.h"

typedef void(*ANIMATIONEVENTHANDLER) (XModuleEntity&, const wstring&, const wstring&, const wstring&);
typedef map<wstring,ANIMATIONEVENTHANDLER>	ANIMATIONEVENTMAP;

class XAnimationEventHandler
{
private:
	map<wstring,ANIMATIONEVENTHANDLER> m_eventHandlers;

private:
	bool			CheckDuplicationEffectLoop(XModuleEntity& entity, wstring& strParam1, wstring& strParam2, wstring& strParam3);

public:
	XAnimationEventHandler()
	{
		// 애니메이션 이벤트 이름규칙과 처리할 펑션을 등록한다.
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_walk",					OnAnimationEvent_walk) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_effect",				OnAnimationEvent_effect) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_sound",					OnAnimationEvent_sound) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_camera",				OnAnimationEvent_camera) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_spell_fire",			OnAnimationEvent_spell_fire) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_arc_shoot",				OnAnimationEvent_arc_shoot) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_throw",					OnAnimationEvent_throw) );
		//m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_camera",				OnAnimationEvent_camera) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_custom",				OnAnimationEvent_Custom) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_swallowed_disappear",	OnAnimationEvent_swallowed_disappear) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_battle_start",			OnAnimationEvent_battle_start) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_battle_end",			OnAnimationEvent_battle_end) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_bparts",				OnAnimationEvent_breakable_parts) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_face_ani_start",		OnAnimationEvent_face_ani_start) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_face_ani_end",			OnAnimationEvent_face_ani_end) );
		m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( L"sh_defence_start_sound",	OnAnimationEvent_defence_start_sound) );
	}
	void OnEvent(RActor* pActor, const RAnimationEvent* pEvent, XModuleEntity& entity);

	static XAnimationEventHandler& GetInstance();

	static void		OnAnimationEvent_walk(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_effect(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_sound(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_camera(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_spell_fire(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_arc_shoot(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_throw(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_Custom(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_swallowed_disappear(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_battle_start(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_battle_end(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_breakable_parts(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_face_ani_start(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_face_ani_end(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);
	static void		OnAnimationEvent_defence_start_sound(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3);

	void			OnWalkSound( XModuleEntity& entity );
};

