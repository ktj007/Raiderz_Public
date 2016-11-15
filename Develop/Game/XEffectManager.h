#ifndef _XEFFECT_MANAGER_H
#define _XEFFECT_MANAGER_H

#include "XEffect.h"
#include "XActor.h"
#include "RActorNode.h"
#include "XEffectPreLoaderResourceMgr.h"
#include "XEffectWaitActorLoadingComplete.h"
#include "XEffectPostEffect.h"
#include "XCommonData.h"
#include "XDecalEffect.h"
#include "RDecalPolygonSceneNode.h"
#include "XHitEffectSelecter.h"
#include "XSelfIlluminationEffect.h"
#include "XLightEffect.h"
#include "XNoiseFadeEffect.h"
#include "XWeaponEffect.h"
#include "XPEEdgeMgr.h"

class XCharacter;

struct stEffectReserveData
{
	XEffectBase*			pEffect;
	EFFECT_CREATE_DATA		effectData;
};

class XEffectManager
{
	friend XEffectWaitActorLoadingComplete;

private:
	XEffectPreLoaderResourceMgr effectPreLoader;
	XEffectWaitActorLoadingComplete effectWait;
	XEffectPostEffect effectPostEffect;
	RDecalPolygonSceneNode* m_pEffectDecalPolygon;
	XHitEffectSelecter	m_HitEffectSelecter;
	XPEEdgeMgr m_PEEdgeMgr;

	vector<stEffectReserveData>	m_vecEffectReserve;

private:
	void		UpdateXEffect(float fDelta);
	void		UpdataXEffectReserve();

	void		AddXEffectReserve(XEffectBase* pEffect, EFFECT_CREATE_DATA& effectCreaetData);
	void		DelXEffectReserve(MUID& uid);

	MUID		AddXEffect(XEffect* pEffect, EFFECT_CREATE_DATA& effectCreateData);
	MUID		AddXWeaponEffect(XWeaponEffect* pEffect, EFFECT_CREATE_DATA& effectCreateData);

	bool		CheckDuplicationSelfIlluminationEffect(MUID& uidOwner, wstring& strActorNodeName);
	bool		CheckWeaponEffectDummy(XCharacter* pActor, wstring& strBoneName);

	void		AttachSceneNode(RSceneNode* pEffectInst, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false);

protected:
	vector<XEffectBase*>		m_vEffectList;

public:
	XEffectManager();
	virtual ~XEffectManager();
	virtual void		OnUpdate(float fDelta);
	virtual void		Destroy();
	virtual void		Clear();
	
	virtual XEffect*	CreateXEffect(const wstring& szEffectName);
	virtual XDecalEffect*	CreateXDecalEffect(const wstring& szEffectName, BYTE byDeclaEffectType, const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ);
	virtual XSelfIlluminationEffect* CreateXSelfIlluminationEffect(float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax);
	virtual XLightEffect* CreateXLightEffect();
	virtual XNoiseFadeEffect* CreateNoiseFadeEffect(float fStartDurationTime, float fEndDurationTime);
	virtual XWeaponEffect* CreateWeaponEffect(const wstring& szEffectName);
	
	virtual void		AttachXEffect(XModelEffect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false);
	virtual void		AttachXDecalEffect(XDecalEffect* pDecalEffect, RSceneNode* pSceneNode, RActorNode* pActorNode);

	size_t		GetCount();	///< 현재 이펙트의 수를 리턴한다

	// 월드 좌표로 보이는 이펙트
	bool		DeleteXEffect(MUID & uid, bool bImmediatlyDelete = false);				// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	
	XEffectBase* GetEffect(MUID& uid);
	XEffect*	GetXEffect(MUID& uid);
	XModelEffect*	GetXModelEffect(MUID& uid);
	XDecalEffect* GetXDecalEffect(MUID& uid);
	XLightEffect* GetXLightEffect(MUID& uid);
	XWeaponEffect* GetXWeaponEffect(MUID& uid);

	void		SetScaleInit(XEffect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode);
	bool		SetScaleXEffect(MUID& uid, bool bAutoScale, vec3 scale);
	bool		SetScaleXEffect(XEffect* pEffect, bool bAutoScale, vec3 scale);

	// 이펙트 붙이기
	MUID		AddXEffect(const wstring& szEffectName, XCharacter* pActor, RActorNode* pActorNode, 
							const RMatrix& _rBaseFrame = RMatrix::IDENTITY, 
							bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f,
							float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	MUID		AddXEffect(EFFECT_CREATE_DATA& effectCreateData);

	// 데칼 이펙트
	MUID		AddXDecalEffect(EFFECT_CREATE_DATA& effectCreateData, BYTE byDeclaEffectType, const DWORD dwColor, const float fSizeX, const float fSizeY, const float fSizeZ);

	// 효과 이펙트
	void		SetPostEffect(BYTE byPostEffectType, XObject* pObject, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType);

	// Illumination 이펙트
	MUID		AddXSelfIllumination(EFFECT_CREATE_DATA& effectCreateData, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax, bool bLoop);

	// Light 이펙트
	MUID		AddXLightEffect(XObject* pObject, wstring& strBoneName, float fRadius, float fAttenuationStart, float fIntensity, const DWORD dwColor, float fDurationTime, bool bFollow, RMatrix _rWorld = RMatrix::IDENTITY);

	// Noise Fade 이펙트
	void		SetXNoiseFadeEffect(XObject* pObject, vector<tstring> vecMeshNodeName, float fStartDurationTime, float fEndDurationTime, DWORD dwColor1, DWORD dwColor2, float fSize1, float fSize2, bool bInverse);

	// 검 이펙트
	MUID		AddXWeaponEffect(EFFECT_CREATE_DATA& effectCreateData);

	// 외과선
	void		EnablePEEdge(MUID& uid, const RVector4& _color, float _fTickNess, float _fBlurFactor, bool _bProjection);
	void		DisablePEEdge(MUID& uid);

	// 기타...
	MUID		AddEffectWaiting(EFFECT_CREATE_DATA& effectWaitingData);
	MUID		AddWeaponEffectWaiting(EFFECT_CREATE_DATA& effectWaitingData);
	void		AddHitInfo(XTalentHitParam* pHitInfo, unsigned int nDelayTime);

	bool		SetTransform(MUID& uid, vec3& vPos, vec3& vDir, vec3& vUp, RMatrix& matOutResult, bool bTerrainAlign = false, bool bTerrainPos = false);

	void		PreLoaderXEffect(const wstring& strEffectName);

	void		ActorLoadingComplete(MUID uidObject);
	void		ActorChangeMotion(MUID uidActor);

	void		StartHitEffectSelect(XTalentHitParam* pHitParam);

};

//////////////////////////////////////////////////////////////////////////
// inline
inline size_t XEffectManager::GetCount()	{ return m_vEffectList.size(); }

#endif