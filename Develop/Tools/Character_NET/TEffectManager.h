#ifndef _TEFFECT_MANAGER_H
#define _TEFFECT_MANAGER_H

#include "TEffect.h"
#include "TEffectPostEffect.h"
#include "TSelfIlluminationEffect.h"
#include "TDecalEffect.h"
#include "TNoiseFadeEffect.h"
#include "TWeaponEffect.h"
#include "TFileNameSampling.h"

class TEffectManager
{
private:
	TEffectPostEffect effectPostEffect;

	vector<FILE_LIST> m_vecEffectFileList;

private:
	RMatrix		GetTerrainMatrix(const vec3& vPos, vec3 vDir, bool bAlign);

	void		SetEffectFileList();

protected:
	vector<XEffectBase*>		m_vEffectList;
	vector<XEffectBase*>		m_vWeaponEffectList;

	void		SetScaleInit(TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode);
	bool		CheckDuplicationSelfIlluminationEffect(MUID& uidOwner, string& strActorNodeName);
	bool		CheckWeaponEffectDummy(TCharacter* pSceneNode, int nIndex);
	bool		CheckWeaponEffectDummy(TCharacter* pSceneNode, string strBoneName);

public:
	TEffectManager();
	virtual ~TEffectManager();
	void		OnUpdate(float fDelta);
	void		Update(vector<XEffectBase*>& vecUpdateEffect, float fDelta);
	void		Destroy();
	void		Clear();
	size_t		GetCount();	///< 현재 이펙트의 수를 리턴한다

	void		GetEffectPureFileNameList(vector<string>& vecFileList);

	// 월드 좌표로 보이는 이펙트
	TEffect*	CreateTEffect(const string& szEffectName);
	bool		DeleteTEffect(MUID & uid, bool bImmediatlyDelete = false);				// bImmediatlyDelete이 true이면 즉시 삭제, false이면 서서히 사라짐
	TEffect*	GetTEffect(MUID& uid);
	bool		SetScaleTEffect(MUID& uid, bool bAutoScale, vec3 scale);
	XEffectBase* GetEffect(MUID& uid);
	TDecalEffect* GetTDecalEffect(MUID& uid);

	TSelfIlluminationEffect* CreateXSelfIlluminationEffect(float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax);
	TNoiseFadeEffect* CreateNoiseFadeEffect(float fStartDurationTime, float fEndDurationTime);
	TWeaponEffect* CreateWeaponEffect(const string& szEffectName, int nIndex);

	// 이펙트 붙이기
	MUID		AddTEffect(const string& szEffectName, TCharacter* pSceneNode, RActorNode* pActorNode, 
							const RMatrix& _rBaseFrame = RMatrix::IDENTITY, 
							bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, bool bWaterPos = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	void		AddTEffect(TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode,
							const RMatrix& _rBaseFrame = RMatrix::IDENTITY, 
							bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, bool bWaterPos = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	MUID		AddTEffect(const string& szEffectName, TCharacter* pSceneNode, RActorNode* pActorNode, 
							const vec3& vPos, const vec3& vDir, const vec3& vUp,
							bool bTerrainAlign = false, bool bTerrainPos = false, bool bWaterPos = false, bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	MUID		AddTEffect(const string& szEffectName, const vec3& vPos, const vec3& vDir, const vec3& vUp,
							bool bTerrainAlign = false, bool bTerrainPos = false, bool bWaterPos = false, bool bLastFramePause = false, bool bIgnoreDir = false, bool bImmediatlyDelete = false, float fDurationTime = 0.0f, float fStartVisibilityTime = 0.0f, float fEndVisibilityTime = 0.0f);
	void		AttachTEffect(TEffect* pEffect, TCharacter* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false);

	bool		SetTransform(MUID& uid, vec3& vPos, vec3& vDir, RMatrix& matResult, bool bTerrainAlign = false, bool bTerrainPos = false);

	void		SetPostEffect(BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType);
	MUID		SetSelfIlluminationEffect(string& strActorNodeName, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fDurationTime, float fMin, float fMax, bool bLoop);
	void		SetNoiseEffect(vector<string>& vecMeshNodeName, float fStartDurationTime, float fEndDurationTime, DWORD dwColor1, DWORD dwColor2, float fSize1, float fSize2, bool bInverse);
	void		SetWeaponEffect(TCharacter* pSceneNode, const string& szEffectName, int nIndex);
	void		SetWeaponEffect(TCharacter* pSceneNode, const string& szEffectName, const string& szBoneName);

	void		StopAnimationEffect();
	void		StopWepaonEffect();

	// 데칼 이펙트
	MUID		AddXDecalEffect(string strEffectName, string strBoneName, vec3 vPos, bool bFollow, BYTE byDeclaEffectType, const DWORD dwColor, const float fSize);

	MUID		GenerateLocalUID();

};

//////////////////////////////////////////////////////////////////////////
// inline
inline size_t TEffectManager::GetCount()	{ return m_vEffectList.size(); }

#endif