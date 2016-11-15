#pragma once

#include "../../../Game/GameCommon/XEffectBaseInvoker.h"
#include "WS_EffectManager.h"
#include "../../../Game/XCutSceneObjectController.h"
#include "../../../Game/XCutSceneObjectControllerEffect.h"

//----------------------------------------------------------------------------//
// 툴 전용 EffectInvoker                                                                     
//----------------------------------------------------------------------------//
class CORETOOL_SOULHUNT_API WS_EffectCharacterToolInvoker : public XEffectBaseInvoker
{
private:
	RActor*					m_pActor;
	WS_EffectManager*		m_pEffectManager;
	XEffectInfoMgr*			m_pEffectInfoMgr;

protected:
	virtual string			GetReserved_Param(string& strEffectName);
	virtual string			GetReserved_BoneName(string& strEffectName);
	virtual string			GetReserved_SoundName(string& strEffectName, string& strEffectparam, unsigned int nFlags);
	virtual string			GetReserved_ModelName(string& strEffectName);

	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo);
	virtual void			SetPostEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo);

public:
	WS_EffectCharacterToolInvoker();
	virtual ~WS_EffectCharacterToolInvoker();

	void					Invoke(RActor* pActor, string& strEffectName, string& strEffectParam, string& strEffectBoneName, XEffectInfoMgr* pEffectInfoMgr, WS_EffectManager* pEffectManager, vector<stControllerEffectData>* vecOutData = NULL);

	void					GetEffect_Data(CUSTOM_EFFECT_TYPE eEffectType, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData);
};

//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
// 툴 전용 TEffectCharacterToolSinglyInvoker                                                                     
//----------------------------------------------------------------------------//
class CORETOOL_SOULHUNT_API WS_EffectCharacterToolSinglyInvoker : public XEffectBaseInvoker
{
private:
	RActor*					m_pActor;
	WS_EffectManager*		m_pEffectManager;
	XEffectInfoMgr*			m_pEffectInfoMgr;

protected:
	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);

public:
	WS_EffectCharacterToolSinglyInvoker();
	virtual ~WS_EffectCharacterToolSinglyInvoker();

	void					Invoke(string& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, XEffectInfoMgr* pEffectInfoMgr, WS_EffectManager* pEffectManager, vector<stControllerEffectData>* vecOutData = NULL);
};
