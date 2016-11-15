#pragma once

#include "VDataValidator.h"
#include "XEffectBaseInvoker.h"

class VEffectDataValidator;

typedef void(*EFFECTHANDLER_VALIDATOR) (VEffectDataValidator&, XEffectInfo *, XBaseEffectInfoItem*, VValidatorInfoMgr*, string );

#define EFFECTYPE_STRING_CAST_EFFECT						"캐스트"
#define EFFECTYPE_STRING_USE_EFFECT							"사용"
#define EFFECTYPE_STRING_ATTACK_EFFECT						"피격"
#define EFFECTYPE_STRING_MISS_EFFECT						"실패"
#define EFFECTYPE_STRING_SHOCK_EFFECT						"충격"
#define EFFECTYPE_STRING_DURATION_EFFECT					"지속"
#define EFFECTYPE_STRING_DEFENSE_EFFECT						"방어"

#define BUFF_XML_TAG_BUFF_GAIN_EFFECT						"GainEffect"
#define BUFF_XML_TAG_BUFF_HIT_EFFECT						"HitEffect"

class VEffectDataValidator : VDataValidator
{
protected:
	class XEffectDataInvoker : public XEffectBaseInvoker
	{
	public:
		bool							CheckReservedEffect(string& strEffectName);
	};

public:
	int									m_nEffectInfoCount;
	int									m_nTalentEffectInfoCount;
	int									m_nBuffEffectInfoCount;

	VErrorMsg							m_EffectInfoValidatorError;
	VErrorMsg							m_TalentEffectInfoValidatorError;
	VErrorMsg							m_BuffEffectInfoValidatorError;

	XEffectDataInvoker					m_CheckEffectInstance;

private:
	string								GetTextTalentEffectType(TALENT_EFFECT_TYPE nEffectType);
	string								GetTextBuffEffectType(BUFF_EFFECT_TYPE nBuffType);

protected:
	map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR> m_mapEffectHandlers_validator;

public:
	VEffectDataValidator();
	virtual ~VEffectDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckEffectInfo(XEffectInfo * pEffectInfo, VValidatorInfoMgr* vInfoMgr);
	void								CheckTalentEffectInfo(XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr);
	void								CheckBuffEffectInfo(XBuffInfo* pBuffInfo, VValidatorInfoMgr* vInfoMgr);

	virtual int							GetCheckCount() { return m_nEffectInfoCount + m_nTalentEffectInfoCount + m_nBuffEffectInfoCount; }

	static void							OnEffect_Sound(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_Model(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_Camera(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_PostEffect(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_DecalEffect(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_IlluminationEffect(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_LightEffect(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);
	static void							OnEffect_AlphaTestFadeEffect(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError);

};
