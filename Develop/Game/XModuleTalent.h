#ifndef _XMODULE_TALENT_H
#define _XMODULE_TALENT_H

#include "XModule.h"
#include <list>
using namespace std;

#include "XTalent.h"
#include "XMeleeTalent.h"
#include "XArcheryTalent.h"
#include "XMagicTalent.h"
#include "XExtraActiveTalent.h"
#include "XItemTalent.h"
#include "XGuardTalent.h"
#include "XHitFrameFreezer.h"

class XTalent;
class XTalentInfo;
class XTalentEffectDataMgr;
class XTestFrame;
class XPlayFrame;
class XBackViewCameraSceneNode;


//////////////////////////////////////////////////////////////////////////
struct HITEFFECT_RESULT
{
	MMatrix				matHitEffect;
	MMatrix				matBloodEffect;

	HITEFFECT_RESULT()
	{
		matHitEffect.MakeIdentity();
		matHitEffect.MakeIdentity();
	}
};

//////////////////////////////////////////////////////////////////////////
/// 현재 탤런트 타입에 맞게 동작을 할 수 있도록 탤런트 타입별 로직 분화
class XTalentFactory
{
public:
	static XBaseTalent* NewTalent(XObject* pOwner, XTalentInfo* pTalentInfo);
};


class XModuleTalent : public XModule, public MMemPool<XModuleTalent>
{
private:
	DECLARE_ID(XMID_TALENT);

	// 탤런트 리팩토링관련 추가 ----
	XBaseTalent*			m_pCurrentTalent;

	XHitFrameFreezer		m_HitFrameFreezer;

	MMatrix					m_matServerStartTalent;

private:
	MMatrix					CalHitEffectMatrix(vec3 vHitPosition, vec3 vAttackerDir, MMatrix matMeleeHitDir);
	MMatrix					CalBloodEffectMatrix(vec3 vHitPosition, vec3 vAttackerDir, MMatrix matMeleeHitDir);

protected:	
	virtual void			OnFinalized();
	virtual XEventResult	OnEvent(XEvent& msg);
	virtual void			OnSubscribeEvent();
	virtual void			OnUpdate(float fDelta);
	

	void					ChangeNPCMode();
	void					HitCreateEffect(int nTalentID, XTalentInfo* pTalentInfo, XObject* pAttackObject, vec3 vHitPosition, HITEFFECT_RESULT& resultHitEffect, bool bSec = false, unsigned int nDamageFlags = 0);
	void					DefenseCreateEffect(int nTalentID, XTalentInfo* pTalentInfo, XObject* pTargetObject, vec3 vHitPosition);

	bool					ChangeTalentStrategy(XTalentInfo* pTalentInfo);

	void					SetNPCGuardIcon(XTalentInfo* pTalentInfo);

public:
	XModuleTalent(XObject* pOwner=NULL);
	virtual ~XModuleTalent(void);
	void					HitEffect(int nTalentID, XObject* pAttackObject, vec3 vHitPosition, HITEFFECT_RESULT& resultHitEffect, bool bSec = false, unsigned int nDamageFlags = 0);
	void					DefenseEffect(int nTalentID, XObject* pAttackObject, vec3 vHitPosition);
	//MUID					CreateTalentEffect(wstring szEffectName, 
	//											RActor* pActor, 
	//											RActorNode* pActorNode, 
	//											vec3& vOffsetPos, vec3& vDir, 
	//											bool bFollow,
	//											bool bLastFramePause = false, float scale = 1.0f);

	bool					Start(int nTalentID);
	bool					Act();
	void					ReCastingTalent(int nTalentID, int nStep = 0);
	void					ExtraAct();
	void					ExtraAct2();
	void					ExtraAct3();

	void					OnHitTarget(int nAttackedTalentID);
	///
	void					SetTarget(MUID uidTarget, int nGroupID = -1, int nDummyID = -1, vec3 vTargetPos = vec3::ZERO)		{ m_pCurrentTalent->SetTarget(uidTarget, nGroupID, nDummyID, vTargetPos); }
	bool					Cancel()						{ return m_pCurrentTalent->Cancel(); }
	void					RequestActSpell()				{ m_pCurrentTalent->RequestActSpell(); }
	void					ForceTerminate()				{ m_pCurrentTalent->ForceTerminate(); }
	void					SetProjectileTargetPos(vec3 vTargetPos) { m_pCurrentTalent->SetProjectileTargetPos(vTargetPos); }

	bool					IsActive()						{ return m_pCurrentTalent->IsActive(); }
	XTalentInfo*			GetInfo()						{ return m_pCurrentTalent->GetInfo(); }
	TALENT_PHASE			GetPhase()						{ return m_pCurrentTalent->GetPhase(); }
	XBaseTalent*			GetTalent()						{ return m_pCurrentTalent; }

	bool					IsTalentColliable();
	void					Reload();

	void					SetTalentServerStartMatrix(MMatrix matStart)	{ m_matServerStartTalent = matStart; }
	MMatrix					GetTalentServerStartMatrix()					{ return m_matServerStartTalent; }
};


#endif