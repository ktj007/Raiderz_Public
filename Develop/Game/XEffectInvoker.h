#ifndef _XEFFECT_INVOKER_H
#define _XEFFECT_INVOKER_H

#pragma once

#include "XEffectBaseInvoker.h"
#include "XCommonData.h"

class XEffectInvoker : public XEffectBaseInvoker
{
protected:
	XObject	*				m_pOwner;
	XObject *				m_pDestObject;

private:
	bool					CheckUseMotionBlur(BYTE nPostEffecType, float fPlayTime, float fScatTime, float fSpeed, int nDistance, BYTE nScarFactorType, BYTE nUseType);

	tstring					GetArmorTypeSoundName(ARMOR_ATTRIB nArmorType);
	tstring					GetSoundTypeSoundName(SOUND_TYPE nSoundType);

protected:
	virtual wstring			GetReserved_Param(wstring& strEffectName);
	virtual wstring			GetReserved_BoneName(wstring& strEffectName);
	virtual wstring			GetReserved_SoundName(wstring& strEffectName, wstring& strEffectparam, unsigned int nFlags);
	virtual wstring			GetReserved_ModelName(wstring& strEffectName);

	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo);
	virtual void			SetPostEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo);
	virtual MUID			SetDecalEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo);
	virtual MUID			SetIlluminationEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo);
	virtual MUID			SetLightEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_LightEffect& lightInfo);
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo);

	bool					CheckActorLoadingComplete_Effect_Model(EFFECT_CREATE_DATA& _effectData, MUID& uidOut);
	bool					CheckPlayCameraEffect();
	bool					CheckInWater(vec3& vPos, float fHeightMin, float fHeightMax);


	wstring					GetHitSoundName( wstring strPrefix, wstring strPostfix, XObject* pAttacker, XObject* pVictim, XTalentInfo* pAttackTalentInfo, unsigned int nDamageFlags);
	wstring					GetGuardSoundName( wstring strPrefix, XObject* pAttacker, XObject* pVictim);

public:
	XEffectInvoker();
	virtual ~XEffectInvoker(void);

	void					Invoke(XObject* pSourceObject, wstring& strEffectName, wstring& strEffectParam, wstring& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData = NULL, XObject* pDestObject = NULL, EFFECT_POST_TRANSFORM* pPostData = NULL);

	void					GetEffect_Data(CUSTOM_EFFECT_TYPE eEffectType, XObject* pObject, XObject* pDestObject, wstring& strEffectName, wstring& strEffectParam, wstring& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData);
};

//////////////////////////////////////////////////////////////////////////
// 간단하게 이펙트 일반화를 사용하기 위한거
class XEffectSinglyInvoker : public XEffectInvoker
{
protected:
	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo);
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo) { return; }

public:
	XEffectSinglyInvoker();
	virtual ~XEffectSinglyInvoker(void);

	void					Invoke(wstring& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, vector<EFFECT_RESULT_DATA>* vecOutData = NULL, EFFECT_POST_TRANSFORM* pPostData = NULL);
};

//////////////////////////////////////////////////////////////////////////
// 미리 이펙트 내용을 로딩하기 위한거... 주로 Effect_Model을 사용하게 됨
class XEffectAdvenceInvoker : public XEffectInvoker
{
protected:
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);

	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData)			{ return MUID::ZERO; }
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo)			{}
	virtual void			SetPostEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo) {}
	virtual MUID			SetDecalEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo) { return MUID::ZERO; }
	virtual MUID			SetIlluminationEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo) { return MUID::ZERO; }
	virtual MUID			SetLightEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_LightEffect& lightInfo) { return MUID::ZERO; }
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo) { return; }

public:
	XEffectAdvenceInvoker();
	virtual ~XEffectAdvenceInvoker(void);

	void					Invoke(wstring& strEffectName);
};

//////////////////////////////////////////////////////////////////////////
// 칼 이펙트를 위한거... 칼 붙여진 노드를 모두 찾아서 붙인다.
class XEffectWeaponInvoker : public XEffectInvoker
{
private:
	void					SetWeaponEffectInvoker(EFFECT_TRANSEMISION_DATA& _data, const float fEffectDurationTime, vector<EFFECT_RESULT_DATA>* vecOutData);
	void					SetWeaponEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model * pEffectInfoItem_Model, vector<EFFECT_RESULT_DATA>* vecOutData);

	void					SetWeaponEffectAllDummyNode(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model * pEffectInfoItem_Model, vector<EFFECT_RESULT_DATA>* vecOutData);
protected:
	// 좀 다릅니다.
	MUID					SetModel(EFFECT_TRANSEMISION_DATA& _data, bool bFollow, bool bTerrainAlign, bool bTerrainPos, bool bWaterPos, bool bAlign, bool bAutoScale, bool bImmediatlyDelete, vec3 vScale, float fDurationTime, float fStartVisibilityTime, float fEndVisibilityTime, bool bLastFramePause);

	bool					CheckActorLoadingComplete_Effect_Weapon(EFFECT_CREATE_DATA& _effectData, MUID& uidOut);

public:
	XEffectWeaponInvoker();
	virtual ~XEffectWeaponInvoker(void);

	void					Invoke(XObject* pSourceObject, wstring& strEffectName, wstring& strBoneName, int nPartsSlotType, vector<EFFECT_RESULT_DATA>* vecOutData, XObject* pDestObject = NULL);

};

//////////////////////////////////////////////////////////////////////////
// 클라이언트에서만 이펙트 삭제
class XEffectDelete
{
public:
	static bool				Del(CUSTOM_EFFECT_TYPE eType, MUID& uidEffect);
	static void				DelList(vector<EFFECT_RESULT_DATA>& effectList);
};

#endif
