#pragma once

#include "XEffectBaseInvoker.h"

//----------------------------------------------------------------------------//
// Åø Àü¿ë EffectInvoker                                                                     
//----------------------------------------------------------------------------//
class TEffectCharacterToolInvoker : public XEffectBaseInvoker
{
private:
	TCharacter*				m_pActor;

protected:
	virtual string			GetReserved_Param(string& strEffectName);
	virtual string			GetReserved_BoneName(string& strEffectName);
	virtual string			GetReserved_SoundName(string& strEffectName, string& strEffectparam, unsigned int nFlags);
	virtual string			GetReserved_ModelName(string& strEffectName);

	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo);
	virtual void			SetPostEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo);
	virtual MUID			SetDecalEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo);
	virtual MUID			SetIlluminationEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo);
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo);

public:
	TEffectCharacterToolInvoker();
	virtual ~TEffectCharacterToolInvoker();

	void					Invoke(TCharacter* pActor, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData = NULL, RMatrix * pMatLocal = NULL, float fSize = 0.0f, MMatrix matWorld = MMatrix::IDENTITY);

	void					GetEffect_Data(CUSTOM_EFFECT_TYPE eEffectType, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData);
};


class TEffectCharacterToolWeaponInvoker : public XEffectBaseInvoker
{
private:
	TCharacter*				m_pActor;

protected:
	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);

public:
	TEffectCharacterToolWeaponInvoker();
	virtual ~TEffectCharacterToolWeaponInvoker();

	void					Invoke(TCharacter* pActor, string& strEffectName, string& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData = NULL);
};