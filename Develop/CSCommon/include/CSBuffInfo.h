#pragma once

#include "CSCommonLib.h"
#include "CSModifier.h"
#include "CSTalentInfo.h"
#include "MTypes.h"
#include "MXml.h"
#include <string>
#include "MTstring.h"
#include "CSPalette.h"

using namespace::std;

enum BUFF_TYPE
{
	BUFT_NONE=0,
	BUFT_BUFF,
	BUFT_DEBUFF,
	BUFT_AURA,
	BUFT_AURA_EFFECT_BUFF,
	BUFT_AURA_EFFECT_DEBUFF,
	BUFT_ENCHANT,
	BUFT_QUEST,
	BUFT_MAX,
};

static const TCHAR* BUFF_TYPE_STR[] = 
{
	_T("NONE"),
	_T("BUFF"),
	_T("DEBUFF"),
	_T("AURA"),
	_T("AURA_EFFECT_BUFF"),
	_T("AURA_EFFECT_DEBUFF"),
	_T("ENCHANT"),
	_T("QUEST"),
};

enum BUFF_PASSIVE_EXTRA_ATTRIB_
{
	BUFPEA_NONE = 0,
	BUFPEA_DISABLE_TALENTS,
	BUFPEA_DISABLE_TALENT_MELEE,
	BUFPEA_DISABLE_TALENT_RANGE,
	BUFPEA_DISABLE_TALENT_MAGIC,
	BUFPEA_INVINCIBILITY,
	BUFPEA_ROOT,
	BUFPEA_SLEEP,
	BUFPEA_INVISIBILITYTONPC,
	BUFPEA_INVISIBILITY,
	BUFPEA_ABSOLUTE_GUARD,
	BUFPEA_DISABLE_QUESTPVP_DROP,
	BUFPEA_RIDE,
	BUFPEA_MAX
};

static const TCHAR* BUFF_PASSIVE_EXTRA_ATTRIB_STR[] = 
{
	_T("NONE"),
	_T("DISABLE_TALENTS"),
	_T("DISABLE_TALENT_MELEE"),
	_T("DISABLE_TALENT_RANGE"),
	_T("DISABLE_TALENT_MAGIC"),
	_T("INVINCIBILITY"),
	_T("ROOT"),
	_T("SLEEP"),
	_T("INVISIBILITYTONPC"),
	_T("INVISIBILITY"),
	_T("ABSOLUTE_GUARD"),
	_T("DISABLE_QUESTPVP_DROP"),
	_T("RIDE")
};

enum BUFF_ACTIVE_EXTRA_ATTRIB_
{
	BUFAEA_NONE = 0,
	BUFAEA_DIE,
	BUFAEA_DESPAWN_ONCE,
	BUFAEA_DESPAWN_ETERNAL,
	BUFAEA_DISMOUNT,
	BUFAEA_MAX
};

static const TCHAR* BUFF_ACTIVE_EXTRA_ATTRIB_STR[] = 
{
	_T("NONE"),
	_T("DIE"),
	_T("DESPAWN_ONCE"),
	_T("DESPAWN_ETERNAL"),
	_T("DISMOUNT")
};

enum BUFF_STACK_TYPE
{
	BUFFSTACK_NONE,	// 중첩되지 않음
	BUFFSTACK_STACK, // 중첩되고 각 버프별로 다른 생명주기를 가짐
	BUFFSTACK_STACK_UPDATE, // 중첩되고 기존 버프의 duration/period 시간 초기화
};

static const TCHAR* BUFF_STACK_TYPE_STR[] = 
{
	_T("none"),
	_T("stack"),
	_T("stack_n_update"),
};




class CSCOMMON_API CSBuffInfo
{
public:
	int						m_nID;						// id (모두)
	BUFF_TYPE				m_nType;
	BUFF_STACK_TYPE			m_nStackType;				// 스택타입
	tstring					m_strName;					// 버프 이름(모두)
	int						m_nStackMaxCount;			// 스탯 최대 갯수

	bool					m_bUseChangeEquipmentSlot;
	int						m_nEquipItemIDs[ITEMSLOT_MAX];				// 변경될 장비 아이템

	bool					m_bUseChangePalette;		// 팔레트 변경할지 여부
	int						m_arrPalettes[PALETTESLOT_MAX]; // 변경될 팔레트
	
	int						m_nStackSlot;				// (모두)
	int						m_nStackPower;				// (모두)

	float					m_fWeaponApplyRate;			// 무기 대미지 배율 (1 = 100%)
	DAMAGE_ATTRIB			m_nDamageAttrib;			// 데미지 종류 (서버)
	int						m_nMinDamage;				// 최소 데미지
	int						m_nMaxDamage;				// 최대 데미지
	int						m_nMinHeal;					// 최소 회복량
	int						m_nMaxHeal;					// 최대 회복량
	float					m_fPercentageHeal;			// 비율적 회복
	BUFF_PASSIVE_EXTRA_ATTRIB_		m_nPassiveExtraAttrib;
	BUFF_PASSIVE_EXTRA_ATTRIB_		m_nPassiveExtraAttrib2;
	float							m_fPassiveExtraAttribValue;
	float							m_fPassiveExtraAttribValue2;
	BUFF_ACTIVE_EXTRA_ATTRIB_		m_nActiveExtraAttrib;

	CSInstantModifier		m_InstantModifier;
	CSActorModifier			m_ActorModifier;
	CSPlayerModifier		m_PlayerModifier;
	
	tstring					m_strEffectName;			// 이펙트 이름
	tstring					m_strBoneName;				// 본 이름

	tstring					m_strUseAniName;			// 애니메이션 이름
	tstring					m_strAniPostfix;			// 애니메이션 접미사

	CSRiposte				m_Riposte;		// Riposte 관련 정보
	GTalentResist			m_Resist;

	bool					m_bCancelable;			// 디스펠과 같은 버프를 지우는 효과가 적용될지 여부	

	//Mount
	int						m_RideNPCID;

	RequireBuff				m_RequireBuff;
	
	struct TriggeredBuff
	{
		// 이 효과와 연결된 버프의 UID
		int				 nBuffID;	
		// 발동 시점
		TALENT_CONDITION nTiming;	
		// 효과 정보
		CSEffectInfo	 infoEffect;
		// 발동 확률
		float	    	 fRate;

		TriggeredBuff();
	} m_TriggeredBuff;

	virtual void CopyAll(CSBuffInfo* pTarget) = 0;
protected:
	bool			IsMatchPassiveExtraAttrib(BUFF_PASSIVE_EXTRA_ATTRIB_ nAttrib) const;
	bool			IsMatchActiveExtraAttrib(BUFF_ACTIVE_EXTRA_ATTRIB_ nAttrib) const;
public:
	CSBuffInfo();

	bool HasDamage();
	bool HasHealEffect();
	bool IsAuraEffectBuff();
	bool IsBuff();
	bool IsDebuff();
	bool IsQuestBuff();
	bool IsMesmerize();
	bool IsEnchant() const;
	bool IsRideBuff() const;
	bool HasProcEffect() const;
};


class CSCOMMON_API CSBuffInfoMgr : public map<int , CSBuffInfo* > 
{
private:
	CSBuffInfoMgr(CSBuffInfoMgr& other);
protected:
	void ParseBuffCommon(CSBuffInfo* pBuffInfo, MXmlElement* pElement, MXml* pXml);
public:
	CSBuffInfoMgr() {}
	virtual ~CSBuffInfoMgr() {}
};
