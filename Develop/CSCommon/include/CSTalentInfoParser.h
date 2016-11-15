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
	CSTalentInfoParser() {}
	virtual ~CSTalentInfoParser() {}

	void ParseEffectTiming(const string& strValue, TALENT_CONDITION& nValue);
	void ParseTalentEffect(MXmlElement* pElement, CSEffectInfo& outEffect, const char* pszPrefix, const char* pszSuffix);

	void ParseTalentCommon(CSTalentInfo* pTalentInfo, MXmlElement* pElement, MXml* pXml);
};
