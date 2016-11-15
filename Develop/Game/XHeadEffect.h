#ifndef _XHEAD_EFFECT_H
#define	_XHEAD_EFFECT_H

#include "XEffectInvoker.h"

//////////////////////////////////////////////////////////////////////////
enum HEAD_EFFECT_TYPE
{
	HET_NONE		= -1,
	HET_STUN		= 0,			// 스턴
	HET_SLEEP,						// 졸음
	HET_LOOTABLE,					// 루팅
};

// 기본 본
#define HEAD_EFFECT_BASE_BONE			L"dummy_ef_head"
#define HEAD_EFFECT_BASE_DUMMY			L"Bip01 HeadNub"
#define LOOT_EFFECT_BASE_DUMMY			L"dummy_ef_loot"

// 기본 이펙트
#define STUN_EFFECT_NAME				L"ef_faint"
#define LOOTABLE_EFFECT_NAME			L"ef_loot"

// 이펙트 데이터 관리
struct stHeadEffectInfo
{
	HEAD_EFFECT_TYPE	eEffectType;
	MUID				uidEffect;
};
//////////////////////////////////////////////////////////////////////////
class XHeadEffect
{
private:
	class LootableEffect
	{
	private:
		MUID					m_uidEffect;

	public:
		LootableEffect() {}
		virtual ~LootableEffect() {}

		virtual void			CreateLootableEffect(XObject* pOwner);
		virtual	void			DeleteLootableEffect();

		virtual bool			CheckDuplicationLootableEffect(HEAD_EFFECT_TYPE eEffectType);
	};

	class BPartNPCLootableEffect : public LootableEffect
	{
	private:
		vector<EFFECT_RESULT_DATA>	m_vecEffectList;

	public:
		BPartNPCLootableEffect() : LootableEffect() {}
		virtual ~BPartNPCLootableEffect() {}

		virtual void			CreateLootableEffect(XObject* pOwner);
		virtual	void			DeleteLootableEffect();

		virtual bool			CheckDuplicationLootableEffect(HEAD_EFFECT_TYPE eEffectType);
	};


private:
	vector<stHeadEffectInfo>	m_vecHeadEffectList;
	set<HEAD_EFFECT_TYPE>		m_setEffectType;

	XObject*					m_pObject;
	LootableEffect*				m_pLootableEffect;

	bool						m_bActorLoadingComplete;

private:
	void						GetHeadEffectBone(XCharacter** pOutActor, RActorNode** pOutActoreNode);

	bool						CheckDuplicationHeadEffect(HEAD_EFFECT_TYPE eEffectType);
	bool						CheckActorLoadingComplete(HEAD_EFFECT_TYPE eEffectType);

	void						DeleteAllHeadEffect();

	// 각 Head Effect
	MUID						CreateStunEffect(XCharacter* pActor, RActorNode* pActorNode);
	MUID						CreateSleepEffect(XCharacter* pActor, RActorNode* pActorNode);

public:
	XHeadEffect(XObject* pOwner);
	virtual ~XHeadEffect();

	void						CreateHeadEffect(HEAD_EFFECT_TYPE eEffectType);
	void						DeleteHeadEffect(HEAD_EFFECT_TYPE eEffectType);

	void						Update(float fDelta);

	void						OnLoadingComplete();

};

#endif // _XHEAD_EFFECT_H
