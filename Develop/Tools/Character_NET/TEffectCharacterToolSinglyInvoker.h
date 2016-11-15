#pragma once

#include "TEffectCharacterToolInvoker.h"

//----------------------------------------------------------------------------//
// Åø Àü¿ë TEffectCharacterToolSinglyInvoker                                                                     
//----------------------------------------------------------------------------//
class TEffectCharacterToolSinglyInvoker : public TEffectCharacterToolInvoker
{
private:
	TCharacter*				m_pActor;

protected:
	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData);
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo) { return; }

public:
	TEffectCharacterToolSinglyInvoker();
	virtual ~TEffectCharacterToolSinglyInvoker();

	void					Invoke(string& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, float fSize = 0.0f, vector<EFFECT_RESULT_DATA>* vecOutData = NULL);
};
