#ifndef _WS_EFFECT_MANAGER_H
#define _WS_EFFECT_MANAGER_H

#include "WS_Effect.h"
#include "WS_EffectPostEffect.h"

class CORETOOL_SOULHUNT_API WS_EffectManager
{
private:
	WS_EffectPostEffect effectPostEffect;

private:
	RMatrix		GetTerrainAlignMatrix(const vec3& vPos, const vec3& vDir);
	MUID		GenerateLocalUID();

protected:
	vector<WS_Effect*>		m_vEffectList;

	void		SetScaleInit(WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode);

public:
	WS_EffectManager();
	virtual ~WS_EffectManager();
	void		OnUpdate(float fDelta);
	void		Destroy();
	void		Clear();
	size_t		GetCount();	///< 현재 이펙트의 수를 리턴한다

	// 월드 좌표로 보이는 이펙트
	WS_Effect*	CreateWSEffect(const string& szEffectName);
	bool		DeleteWSEffect(MUID & uid, bool bImmediatlyDelete = false);				// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	WS_Effect*	GetWSEffect(MUID& uid);
	bool		SetScaleWSEffect(MUID& uid, bool bAutoScale, vec3 scale);

	// 이펙트 붙이기
	MUID		AddWSEffect(const string& szEffectName, RSceneNode* pSceneNode, RActorNode* pActorNode, 
							const RMatrix& _rBaseFrame = RMatrix::IDENTITY, 
							bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	void		AddWSEffect(WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, 
							const RMatrix& _rBaseFrame = RMatrix::IDENTITY, 
							bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	MUID		AddWSEffect(const string& szEffectName, RSceneNode* pSceneNode, RActorNode* pActorNode, 
							const vec3& vPos, const vec3& vDir, const vec3& vUp,
							bool bTerrainAlign = false, bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	MUID		AddWSEffect(const string& szEffectName, const vec3& vPos, const vec3& vDir, const vec3& vUp,
							bool bTerrainAlign = false, bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	void		AttachWSEffect(WS_Effect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false);

	bool		SetTransform(MUID& uid, vec3& vPos, vec3& vDir, RMatrix& matResult, bool bTerrainAlign = false);

	void		SetPostEffect(BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);

	void		ClearPostEffect();
};

//////////////////////////////////////////////////////////////////////////
// inline
inline size_t WS_EffectManager::GetCount()	{ return m_vEffectList.size(); }

#endif