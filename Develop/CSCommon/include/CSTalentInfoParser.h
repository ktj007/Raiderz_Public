#pragma once

#include "CSCommonLib.h"
#include "CSTalentInfo.h"

class CSCOMMON_API CSTalentInfoWeaponAllowedParser
{
public:
	void Parse(const TCHAR* szValue, CSTalentWeaponAllowdInfo& outWeaponAllowedInfo);
};

class CSCOMMON_API CSTalentInfoParserHelper
{
public:
	struct TalentInfoID
	{
		int nID;
		int nMode;
		TalentInfoID() : nID(0), nMode(0) {}
	};

	TalentInfoID ParseTalentID(MXml* pXml, MXmlElement* pElement);
	void SetXmlElement_TalentIDAndMode(MXmlElement* pElement, int nID, int nMode);
	tstring MakeXmlValueFromMode(int nMode);
	int MakeModeFromXmlValue(tstring nValue);
};

class CSCOMMON_API CSTalentInfoParser
{
public:
private:
	void ParseInvokeBuff( CSBuffEnchantInfo& outBuffInvokeInfo, const char* pszSuffix, MXmlElement* pElement );
	void ParseWeaponAllowed(CSTalentInfo* pTalentInfo, const TCHAR* szValue);
private:
	// ParseTalentCommon에서 호출함
	void _ParseNPC( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	
	void _ParseExtraPassiveType( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseExtraActiveType( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseSkillType( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseNPCChangeMode( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseWeaponAllowed( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseFocus( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseFinishEffect( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseUsableAtDead( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
protected:
	void _ParseHit( CSTalentInfo* pTalentInfo, MXmlElement* pElement );

	virtual void ParseTalentServer(CSTalentInfo* pTalentInfo, MXml* pXml, MXmlElement* pElement);
	void ParseResist( GTalentResist& Resist, MXmlElement* pElement, const char* pszAttrResistMethod, const char* pszAttrResistType, const char* pszAttrResistDifficult );
public:
	void ParseApplyRate( MXmlElement* pElement, CSTalentWeaponApplyRate& outApplyRate );
	void ParseDamageType( MXmlElement* pElement, TALENT_DAMAGE_TYPE& outnDamageType );
	void ParseUsableType( MXmlElement* pElement, TALENT_USABLE_TYPE& outnUsableType );
	void ParseFreezeFrame( MXmlElement* pElement, bool& outbFreezeFrame );
private:
	void _ParseShoveType( MXmlElement* pElement, PC_SHOVE_TYPE& eShoveType, const char* szAttrName );
	void _ParsePCShoveType( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseNPCShoveType( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseDamageSoundAttrib( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseConvertID( MXmlElement* pElement, CSConvertIDContainer* pContainer, const char* szAttrName );
	void _ParseConvertTalent( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
	void _ParseConvertBuff( MXmlElement* pElement, CSTalentInfo* pTalentInfo );
public:
	CSTalentInfoParser() {}
	virtual ~CSTalentInfoParser() {}

private:
	TALENT_CONDITION _ParseEffectTiming(const string& strCondition);
public:
	void ParseEffectTiming(const string& strValue, TALENT_CONDITION& nValue);
	void ParseMultiEffectTiming(const string& strValue, set<TALENT_CONDITION>& setValue);
	void ParseTalentEffect(MXmlElement* pElement, CSEffectInfo& outEffect, const char* pszPrefix, const char* pszSuffix);

	void ParseTalentCommon(CSTalentInfo* pTalentInfo, MXmlElement* pElement, MXml* pXml);
};
