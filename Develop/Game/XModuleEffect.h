#ifndef _XMODULE_EFFECT_H
#define _XMODULE_EFFECT_H

#include "XModule.h"
#include "XHeadEffect.h"
//#include "XInteractionEffect.h"
#include "XBuffEffect.h"
#include "XNPCIConEffect.h"
#include "XTalentEffect.h"
#include "XSwordTrail.h"
#include "XModuleEffectReserve.h"
#include "XModuleActorControl.h"
#include "XProjectile.h"

#define	REBIRTH_EFFECT_NAME		L"player_rebirth"

//////////////////////////////////////////////////////////////////////////
// 1. SetRegisterUseEffectID()를 통해서 사용할 이펙트를 등록을 하셔야 합니다.
// 2. ModuleEffect를 통해서 출력하는 이펙트는 모아서 Update에서 처리해야 합니다.
// 2-1. 모아서 처리를 할려면 ModuleEffect에 처리를 해야 하는 것들은 모두 이벤트로 처리를 해야 합니다.
// 2-2. 함수로 Effect를 처리하던 것을 이벤트로 처리해야 합니다.
//////////////////////////////////////////////////////////////////////////
enum XEffectEventID
{
	XEFTEVT_UNKNOWN = 0,

	// Base
	XEFTEVT_EFFECT_MF_STUN,				///< 스턴 이펙트
	XEFTEVT_EFFECT_END_MF_STUN,			///< 스턴 종료 이벤트
	XEFTEVT_EFFECT_REBIRTH,				///< 부활 이펙트

	XEFTEVT_EFFECT_SWORDTRAIL,			///< 검광
	XEFTEVT_EFFECT_END_SWORDTRAIL,		///< 검광 종료
	XEFTEVT_EFFECT_STOP_SWORDTRAIL,		///< 검광 취소

	XEFTEVT_EFFECT_NPC_ICON,			///< NPC 아이콘
	XEFTEVT_EFFECT_PLAYER_ICON,			///< Player 아이콘

	XEFTEVT_EFFECT_LOOTABLE,			/// 루팅이 되면 보여준다.
	XEFTEVT_EFFECT_LOOTABLE_DEL,		/// 루팅 이펙트 삭제

	// Buff
	XEFTEVT_EFFECT_BUFF,				///< 버프 이펙트
	XEFTEVT_EFFECT_BUFF_END,			///< 버프 이펙트 종료
	XEFTEVT_EFFECT_BUFF_DIE,			///< 죽었을때 버프 이펙트 종료 
	XEFTEVT_EFFECT_BUFF_CAPTION,		///< 버프 Caption
	XEFTEVT_EFFECT_BUFF_RESET,
	XEFTEVT_EFFECT_BUFF_ENCHANT,		///< 강화버프

	// Talent
	XEFTEVT_EFFECT_TALENT,				///< 탤런트 이펙트
	XEFTEVT_EFFECT_TALENT_DEL,			///< 현재 저장되어 있는 탤런트 이펙트 삭제
	XEFTEVT_EFFECT_TALENT_EFFECT_PRE_LOADER, ///< 탤런트 이펙트 미리 로딩하기
	XEFTEVT_EFFECT_TALENT_GUARD_DISABLE, ///< 가드 불가능한 탤런트 이펙트 

	// Damage
	XEFTEVT_EFFECT_SHOW_HIT_DAMAGE,		///< 피격 이펙트
	XEFTEVT_EFFECT_SHOW_DAMAGE,			///< 데미지 Caption
	XEFTEVT_EFFECT_SHOW_PROJECTIL_DAMAGE,///< 발사체가 없어진 데미지 이펙트

	// Caption
	XEFTEVT_EFFECT_CAPTION_HEAL,		///< 힐 Caption
	XEFTEVT_EFFECT_CAPTION_HP_REGEN,	///< Hp Regen Caption
	XEFTEVT_EFFECT_CAPTION_GUARDDEFENSE,///< 방어 Caption
	XEFTEVT_EFFECT_CAPTION_DEBUG,		///< 디버그용 Caption
	XEFTEVT_EFFECT_CAPTION_EXP,			///< 경치 Caption
	XEFTEVT_EFFECT_CAPTION_EN,			///< 정신력 Caption

};

enum XEffectID
{
	XEFT_ID_NONE			= 0,
	XEFT_ID_HEAD			= 1,
	XEFT_ID_BUFF,
	XEFT_ID_NPCICON,
	XEFT_ID_TALENT,
	XEFT_ID_SWORDTRAIL,
	XEFT_ID_CAPTION
};

//////////////////////////////////////////////////////////////////////////
class XModuleEffect;

class XEffectEventData
{
public:
	float					m_fRemainTime;

	XEffectEventID			m_nEffectEventID;
	int						m_nParam1;

public:
	XEffectEventData()
	{
		m_fRemainTime			= 0.0f;

		m_nEffectEventID		= XEFTEVT_UNKNOWN;
		m_nParam1				= 0;
	}
	virtual ~XEffectEventData() {}
	bool					bSecondary;

	virtual bool			OnEffectEvent(XModuleEffect * pOwner);
};

class XBuffEffectEventData : public XEffectEventData
{
public:
	BUFF_TYPE				m_eType;
	BUFF_EFFECT_TYPE		m_eEffectType;
	
	bool					m_bRemainBuffGain;
	bool					m_bIsDeBuff;

	wstring					m_strBuffName;

	int						m_nPartsSlotType;
public:
	XBuffEffectEventData():m_bRemainBuffGain(false), m_eType(BUFT_NONE), m_eEffectType(BUFF_GAIN_EFFECT), m_bIsDeBuff(false), m_nPartsSlotType(-1) {}
	virtual ~XBuffEffectEventData() {}

	virtual bool			OnEffectEvent(XModuleEffect * pOwner);
};

class XTalentEffectEventData : public XEffectEventData
{
public:
	TALENT_EFFECT_TYPE		m_nType;

	XTalentInfo *			m_pTalentInfo;
	MUID					m_uidOwner;
	MUID					m_uidAttacker;

	vec3					m_vPos;
	MMatrix					m_matMeleeEffect;

	unsigned int			m_nDamageFlags;

	bool					m_bEffectStorage;

private:
	void SetEffectData(CREATE_TALENT_EFFECT_DATA& effectData)
	{
		effectData.pTalentInfo		= m_pTalentInfo;
		effectData.pOwner			= gg.omgr->Find(m_uidOwner);
		effectData.pAttacker		= gg.omgr->Find(m_uidAttacker);
		effectData.nType			= m_nType;
		effectData.effectPos		= m_vPos;
		effectData.nDamageFlags		= m_nDamageFlags;
		effectData.bEffectStorage	= m_bEffectStorage;
		effectData.matEffect		= m_matMeleeEffect;
	}

public:
	XTalentEffectEventData() : m_pTalentInfo(NULL), m_nType(CAST_EFFECT), m_uidOwner(MUID::ZERO), m_uidAttacker(MUID::ZERO), m_vPos(0, 0, 0), m_bEffectStorage(false), m_nDamageFlags(0)
	{
		m_matMeleeEffect.MakeIdentity();
	}

	virtual ~XTalentEffectEventData() {}

	void Set(XTalentInfo * inTalentInfo, TALENT_EFFECT_TYPE inType, XObject * inObj, vec3 inpos = vec3(0.0f, 0.0f, 0.0f), XObject * inOtherObj = NULL, bool bStorage = false, unsigned int nDamageFlags = 0, MMatrix matMelee = MMatrix::IDENTITY)
	{
		m_pTalentInfo				= inTalentInfo;
		m_nType						= inType;
		if(inObj)	m_uidOwner			= inObj->GetUID();
		if(inOtherObj)	m_uidAttacker	= inOtherObj->GetUID();
		m_vPos						= inpos;
		m_bEffectStorage			= bStorage;
		m_nDamageFlags				= nDamageFlags;

		m_matMeleeEffect			= matMelee;
	}

	virtual bool			OnEffectEvent(XModuleEffect * pOwner);
};

class XHitDamageEffectEventData : public XEffectEventData
{
public:
	_DamageInfo				m_Info;
	
	MUID					m_uidAttacker;
	MUID					m_uidTarget;

	vec3					m_vOffset;

	CSProjectileInfo*		m_pProjectileInfo;

	ProjectileColJudgmentType m_eColType;

public:
	XHitDamageEffectEventData():m_uidAttacker(MUID::ZERO), m_uidTarget(MUID::ZERO), m_pProjectileInfo(NULL), m_vOffset(0, 0, 0), m_eColType(PCJT_NONE) {}
	virtual ~XHitDamageEffectEventData() {}

	virtual bool			OnEffectEvent(XModuleEffect * pOwner);
};

class XCaptionEffectEventData : public XEffectEventData
{
public:
	GUARD_TYPE				m_eGuardType;
	
	wstring					m_strMessage;
	uint32					m_color;

	UIID					m_uiidUser;

public:
	XCaptionEffectEventData():m_eGuardType(GT_INVALID), m_uiidUser(UIID_INVALID) {}
	virtual ~XCaptionEffectEventData() {}

	virtual bool			OnEffectEvent(XModuleEffect * pOwner);
};

//////////////////////////////////////////////////////////////////////////
class XNPCInfo;
class XNonPlayer;
class XTalentHitParam;
struct _DamageInfo;
class XCaptionEffect;
struct HITEFFECT_RESULT;

//////////////////////////////////////////////////////////////////////////
/// 이펙트 출력관리 
class XModuleEffect : public XModule, public MMemPool<XModuleEffect>
{
	DECLARE_ID(XMID_EFFECT);

	friend XEffectEventData;
	friend XBuffEffectEventData;
	friend XTalentEffectEventData;
	friend XHitDamageEffectEventData;
	friend XCaptionEffectEventData;

private:
	//--------------------------------------------------------------------------
	// 이펙트 메세지 버프 관리
	class XEffectEvent
	{
	private:
		vector<XEffectEventData *>	m_vecEffectEventList;
		vector<XEffectEventData *>	m_vecSendEffectEventList;
		XModuleEffect *				m_pOwner;

	public:
		XEffectEvent(XModuleEffect * pOwner);
		~XEffectEvent();

		void						OnEvent(XEffectEventData* pEventData);
		void						update(float fDelta);
		void						sendUpdate(float fDelta);
		void						push_back(XEffectEventData* pEvent);

		int							GetEffectEventIDCount(XEffectEventID nEventID);
	};

	XEffectEvent *			m_pEffectEvent;

	MUID					m_uidPlayerIcon;

protected:
	XHeadEffect *			m_pHeadEffect;
	XBuffEffect *			m_pBuffEffect;
	XNPCIConEffect *		m_pNPCIConEffect;
	XTalentEffect *			m_pTalentEffect;

	XSwordTrail	*			m_pSwordTrail;
	XCaptionEffect*			m_pCaptionEffect;

protected:
	virtual void			OnUpdate(float fDelta);
	virtual XEventResult	OnEvent(XEvent& msg);
	virtual void			OnSubscribeEvent();

	wstring					GetDamageSoundName(_DamageInfo * pInfo);

	void					OnLoadingComplete();

	void					ChangeSwordTrailAnimation();
	void					ChangeSwordTrailMotion();

	//----------------------------------------------------------------------------------------------
	// 이펙트 처리
	void					CreateRebirthEffect();
	void					CreateCutSceneEffect();
	void					DestroyPlayerIcon();

	void					ShowDamageCaptionEffect(_DamageInfo * pInfo, vec3 vDamagePos);
	void					ShowBuffCaptionEffect( const wchar_t* szName, bool bIsDebuff =false);
	void					ShowExpCaptionEffect(int nExp);
	void					ShowDebugCaptionEffect( const wchar_t* szMessage, uint32 nColor);

	void					CreateLootableEffect();
	void					DeleteLootableEffect();

	void					PlayDamageEffect(_DamageInfo * pInfo);
	
	// 피격 Effect
	void					ShowHitAndDamage(_DamageInfo * pInfo, XObject * pAttacker, const vec3& vOffset =vec3::ZERO);
	void					ShowEmptyProjrctileEffect(XObject* pTarget, XObject* pAttacker, CSProjectileInfo* pProjectileInfo, ProjectileColJudgmentType colType);
	void					ShowPartsHit(_DamageInfo* pInfo, vec3 vHitPos, HITEFFECT_RESULT& resultHitEffect);

	// 힐 Effect
	void					ShowHeal(int nHeal, UIID nUserUIID = UIID_INVALID);
	void					ShowHPRegen( int nHeal );

	// 정신력 Effect
	void					ShowEn(int nEn);

	// 방어 Effect
	void					ShowGuardDefense(GUARD_TYPE eGuardType, int nDamage);
	//----------------------------------------------------------------------------------------------

public:
	XModuleEffect(XObject* pOwner=NULL);
	virtual ~XModuleEffect(void);

	void					SetRegisterUseEffectID(int nUseEffectID);


	//----------------------------------------------------------------------------------------------
	// 이펙트 이벤트
	void					OnEffectEvent(XEffectEventID nEffectEventID, XEffectEventData* pEffectData = NULL);
	void					OnEffectEvent(XEffectEventID nEffectEventID, int nParam1);

	//----------------------------------------------------------------------------------------------
	// Getter
	vec3					GetDamageCaptionEffectPos(vec3 vHitEffectPos);
	MMatrix					GetMeleeHitDirection(XTalentInfo* pTalentInfo);

	void					SetLootableEffectByDie();
	void					SetLootableEffectByIcon();

	void					DestroyNPCIconEffect();
	void					ReSetNPCIConEffect();

	void					ShowDamageInfo(_DamageInfo * pInfo, const MUID& uidTarget);

};

#endif // #ifndef _XMODULE_EFFECT_H