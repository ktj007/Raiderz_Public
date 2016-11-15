#pragma once


#define ANIM_NAME_IDLE				_T("idle")
#define ANIM_NAME_WALK				_T("walk")
#define ANIM_NAME_JUMP				_T("jump")

#define ANIM_NAME_RUN_FRONT			_T("run_f")
#define ANIM_NAME_RUN_BACK			_T("run_b")
#define ANIM_NAME_RUN_LEFT			_T("run_l")
#define ANIM_NAME_RUN_RIGHT			_T("run_r")


//#define ANIM_NAME_JUMP_READY		_T("running_jump_ready")
#define ANIM_NAME_JUMP_UP			_T("jump_up")
#define ANIM_NAME_JUMP_DOWN			_T("jump_down")
#define ANIM_NAME_JUMP_land			_T("jump_land")

#define ANIM_NAME_IDLE_COMBAT		_T("idle_combat")

// 모션 목록 -- 나중에는 enum으로 바꾸던가 해야할듯..birdkr
#define MOTION_NAME_IDLE					_T("idle")
#define MOTION_NAME_RUN						_T("run")
#define MOTION_NAME_JUMP					_T("jump")
#define MOTION_NAME_FALLING					_T("falling")
#define MOTION_NAME_DIE						_T("die")
#define MOTION_NAME_AIMING					_T("aiming")
#define MOTION_NAME_SHOOT					_T("shoot")
#define MOTION_NAME_GUARD					_T("guard")
#define MOTION_NAME_GUARD_RETURN			_T("guard_r")
#define MOTION_NAME_GUARD_BLOCK				_T("guarded")
#define MOTION_NAME_GUARD_KNOCKBACK			_T("guard_kb")
#define MOTION_NAME_ROTATE					_T("rotate")
#define MOTION_NAME_LOOTING_ITEM			_T("LootingItem")
#define MOTION_NAME_SWIM_IDLE				_T("swimidle")
#define MOTION_NAME_SWIM					_T("swim")
#define MOTION_NAME_MF_BEATEN				_T("mf_beaten")
#define MOTION_NAME_MF_BEATEN_FRONT			_T("mf_beaten_front")
#define MOTION_NAME_MF_BEATEN_BACK			_T("mf_beaten_back")
#define MOTION_NAME_MF_BEATEN_LEFT			_T("mf_beaten_left")
#define MOTION_NAME_MF_BEATEN_RIGHT			_T("mf_beaten_right")
#define MOTION_NAME_MF_KNOCKBACK			_T("mf_knockback")
#define MOTION_NAME_MF_STUN					_T("mf_stun")
#define MOTION_NAME_MF_DEFLECT				_T("mf_deflect")
#define MOTION_NAME_MF_THROWUP				_T("mf_throwup")
#define MOTION_NAME_MF_KNOCKDOWN			_T("mf_knockdown")
#define MOTION_NAME_MF_UPPERED				_T("mf_uppered")
#define MOTION_NAME_MF_UPPERED_SHORT		_T("mf_uppered_short")
#define MOTION_NAME_MF_GRAPPLED				_T("mf_grappled")
#define MOTION_NAME_MF_GRAPPLED_COLLISION	_T("mf_grappled_collision")

#define MOTION_NAME_RELOAD					_T("reload")

#define MOTION_NAME_CHANGE_STANCE_TO_BATTLE	_T("change_stance_to_battle")
#define MOTION_NAME_CHANGE_STANCE_TO_NORMAL	_T("change_stance_to_normal")
#define MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE	_T("shield_change_stance_to_battle")
#define MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL	_T("shield_change_stance_to_normal")
#define MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE	_T("run_change_stance_to_battle")
#define MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL	_T("run_change_stance_to_normal")
#define MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE	_T("run_shield_change_stance_to_battle")
#define MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL	_T("run_shield_change_stance_to_normal")

#define MOTION_NAME_CHANGE_STANCE_TO_BATTLE2	_T("change_stance_to_battle2")
#define MOTION_NAME_CHANGE_STANCE_TO_NORMAL2	_T("change_stance_to_normal2")
#define MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2	_T("shield_change_stance_to_battle2")
#define MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2	_T("shield_change_stance_to_normal2")
#define MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE2	_T("run_change_stance_to_battle2")
#define MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL2	_T("run_change_stance_to_normal2")
#define MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE2	_T("run_shield_change_stance_to_battle2")
#define MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL2	_T("run_shield_change_stance_to_normal2")

#define MOTION_NAME_ATTACK_1_RETURN			_T("PS_atk1_r")
#define MOTION_NAME_ATTACK_2_RETURN			_T("PS_atk2_r")
#define MOTION_NAME_ATTACK_3_RETURN			_T("PS_atk3_r")
#define MOTION_NAME_ATTACK_4_RETURN			_T("PS_atk4_r")
#define MOTION_NAME_ATTACK_5_RETURN			_T("PS_atk5_r")
#define MOTION_NAME_ATTACK_1F_RETURN		_T("PS_atk1f_r")
#define MOTION_NAME_ATTACK_SWORDDRAWING		_T("PS_sworddrawing")
#define MOTION_NAME_ATTACK_SWORDDRAWING_RETURN	_T("PS_sworddrawing_r")

#define MOTION_NAME_SHIELD_GUARD			_T("shield_guard")
#define MOTION_NAME_SHIELD_GUARD_RETURN		_T("shield_guard_r")
#define MOTION_NAME_SHIELD_GUARD_BLOCK		_T("shield_guarded")
#define MOTION_NAME_SHIELD_GUARD_KNOCKBACK	_T("shield_guard_kb")

#define MOTION_NAME_SLEEP					_T("sleep")
#define MOTION_NAME_REBIRTH					_T("rebirth")
#define MOTION_NAME_SIT_DOWN				_T("sit_down")
#define MOTION_NAME_SIT_RISE				_T("sit_rise")
#define MOTION_NAME_SIT_IDLE				_T("sit_idle")
#define MOTION_NAME_BED_SLEEP				_T("bed_sleep")

// npc
#define MOTION_NAME_NPC_IDLE				_T("npc_idle")
#define MOTION_NAME_NPC_IDLE_COMBAT			_T("npc_idle_combat")
#define MOTION_NAME_NPC_WALK				_T("npc_walk")
#define MOTION_NAME_NPC_RUN					_T("npc_run")
#define MOTION_NAME_NPC_DIE					_T("npc_die")
#define MOTION_NAME_NPC_GUARD				_T("npc_guard")
#define MOTION_NAME_NPC_SHIELD_GUARD		_T("npc_guards")
#define MOTION_NAME_NPC_GUARD_RETURN		_T("npc_guard_r")
#define MOTION_NAME_NPC_SHIELD_GUARD_RETURN	_T("npc_guards_r")

#define MOTION_NAME_NPC_MF_BEATEN			_T("npc_mf_beaten")
#define MOTION_NAME_NPC_MF_BEATEN_FRONT		_T("npc_mf_beaten_front")
#define MOTION_NAME_NPC_MF_BEATEN_BACK		_T("npc_mf_beaten_back")
#define MOTION_NAME_NPC_MF_KNOCKBACK		_T("npc_mf_knockback")
#define MOTION_NAME_NPC_MF_DEFLECT			_T("npc_mf_deflect")
#define MOTION_NAME_NPC_MF_THROWUP			_T("npc_mf_throwup")
#define MOTION_NAME_NPC_MF_KNOCKDOWN		_T("npc_mf_knockdown")
#define MOTION_NAME_NPC_MF_UPPERED			_T("npc_mf_uppered")
#define MOTION_NAME_NPC_MF_STUN				_T("npc_mf_stun")
#define MOTION_NAME_NPC_MF_STUN2			_T("npc_mf_stun2")

#define MOTION_NAME_NPC_TURN_LEFT			_T("npc_turn_l")
#define MOTION_NAME_NPC_TURN_RIGHT			_T("npc_turn_r")

#define MOTION_NAME_NPC_SLEEP				_T("npc_sleep")

// 특수한 모션 - 이것은 XMotion.xml에서 기술하지 않고 따로 관리한다.
#define MOTION_NAME_TALENT_CASTING			_T("skill_casting")
#define MOTION_NAME_TALENT_CASTING_LOOP		_T("skill_casting_loop")
#define MOTION_NAME_TALENT_USE				_T("skill_use")
#define MOTION_NAME_TALENT_EXTRA			_T("skill_extra")
#define MOTION_NAME_TALENT_EXTRA2			_T("skill_extra2")
#define MOTION_NAME_TALENT_EXTRA3			_T("skill_extra3")
#define MOTION_NAME_CUSTOM					_T("custom")			// 분류 없음

// Sorc motions?


// 모션 이벤트
#define MOTION_EVENT_GRAVITY_DOWN			_T("gravity_down")		// 하강 시작시
#define MOTION_EVENT_LANDED					_T("event_landed")		// 땅에 착지시
#define MOTION_EVENT_TIME_OUT				_T("timeout")			// 애니메이션 타임 아웃
#define MOTION_EVENT_MF_STUN_EVENT			_T("mf_stun_event")	
#define MOTION_EVENT_MF_KNOCKBACK_EVENT		_T("mf_kb_event")
#define MOTION_EVENT_MF_KNOCKDOWN_EVENT		_T("mf_kd_event")
#define MOTION_EVENT_MF_THROWUP_EVENT		_T("mf_tu_event")
#define MOTION_EVENT_DOSHOOT				_T("doShoot")
#define MOTION_EVENT_DOTHROW_READY			_T("doThrowReady")
#define MOTION_EVENT_NEXT_MOTION			_T("next_motion")
#define MOTION_EVENT_THROW_GRAPPLED			_T("mf_throw_event")
#define MOTION_EVENT_SPEW_SWALLOWED			_T("mf_spew_event")




enum NPC_STUN_USE_ANIMATION
{
	NPC_USE_STUN = 0,
	NPC_USE_SLEEP,
	NPC_USE_IDLE
};


#define ANIM_PLAYER_MF_TU1_TIME			(0.83f)
#define ANIM_NPC_MF_TU1_TIME			(1.67f)