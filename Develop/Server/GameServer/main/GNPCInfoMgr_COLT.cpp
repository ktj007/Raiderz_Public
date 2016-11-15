#include "stdafx.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "GDef.h"
#include "GStrings.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "MLocale.h"

// 리팩토링후 삭제 결정.
#include "GLuaThisEnchanter.h"
#include "GConst.h"
#include "GScriptGroup_NPC.h"
#include "GNPCScriptBuilder.h"
//

#define NPC_XML_TAG_MAIET			"maiet"
#define NPC_XML_TAG_NPC				"NPC"
#define NPC_XML_TAG_COLT			"COLT"
#define NPC_XML_TAG_SCRIPT			"SCRIPT"
#define NPC_XML_TAG_COMBAT			"COMBAT"
#define NPC_XML_TAG_REACTION		"REACTION"
#define NPC_XML_TAG_IDLE			"IDLE"
#define NPC_XML_TAG_AGGRO			"AGGRO"
#define NPC_XML_TAG_STRESS			"STRESS"
#define NPC_XML_TAG_PROLOGUE		"PROLOGUE"
#define NPC_XML_TAG_EPILOGUE		"EPILOGUE"


#define NPC_XML_TAG_CHECK			"CHECK"
#define NPC_XML_TAG_DEFAULT			"DEFAULT"
#define NPC_XML_TAG_ACTION			"ACTION"

#define NPC_XML_ATTR_TYPE			"type"
#define NPC_XML_ATTR_COLT_MODE					"mode"

#define NPC_XML_ATTR_MAX_DURATION		"max_duration"
#define NPC_XML_ATTR_GUARD_RATE			"guard_rate"
#define NPC_XML_ATTR_DOWNATTACK			"downattack"
#define NPC_XML_ATTR_VICTORY			"victory"
#define NPC_XML_ATTR_RATE				"rate"
#define NPC_XML_ATTR_CYCLETIME			"cycle"
#define NPC_XML_ATTR_DURATIONTIME		"duration"
#define NPC_XML_ATTR_STRESS				"stress"
#define NPC_XML_ATTR_MAXCOUNT			"max_count"
#define NPC_XML_ATTR_AUTORUN			"auto_run"
#define NPC_XML_ATTR_PARAM1				"param1"
#define NPC_XML_ATTR_PARAM2				"param2"
#define NPC_XML_ATTR_PARAM3				"param3"
#define NPC_XML_ATTR_PARAM4				"param4"

#define NPC_XML_VALUE_TYPE_NOTHING				"nothing"
#define NPC_XML_VALUE_TYPE_GROUP				"group"
#define NPC_XML_VALUE_TYPE_TALENT				"talent"
#define NPC_XML_VALUE_TYPE_LUA					"lua"
#define NPC_XML_VALUE_TYPE_MOTIONFACTOR			"motionfactor"
#define NPC_XML_VALUE_TYPE_MODE					"mode"
#define NPC_XML_VALUE_TYPE_DISTANCE				"distance"
#define NPC_XML_VALUE_TYPE_FOLLOW				"follow"
#define NPC_XML_VALUE_TYPE_ANGLE				"angle"
#define NPC_XML_VALUE_TYPE_MOVE					"move"
#define NPC_XML_VALUE_TYPE_BREAKPART			"breakpart"
#define NPC_XML_VALUE_TYPE_DAMAGE				"damage"
#define NPC_XML_VALUE_TYPE_YELL					"yell"
#define NPC_XML_VALUE_TYPE_GUARD				"guard"
#define NPC_XML_VALUE_TYPE_FLEE					"flee"
#define NPC_XML_VALUE_TYPE_ADJUST				"adjust"
#define NPC_XML_VALUE_TYPE_HP					"hp"
#define NPC_XML_VALUE_TYPE_EN					"en"
#define NPC_XML_VALUE_TYPE_SP					"sp"
#define NPC_XML_VALUE_TYPE_UNBREAKPART			"unbreakpart"
#define NPC_XML_VALUE_TYPE_ENEMY_HP				"e_hp"
#define NPC_XML_VALUE_TYPE_ENEMY_MOTIONFACTOR	"e_motionfactor"
#define NPC_XML_VALUE_TYPE_STATE				"state"
#define NPC_XML_VALUE_TYPE_STRESS				"stress"
#define NPC_XML_VALUE_TYPE_HIT_CAPSULE			 "hitcapsule"
#define NPC_XML_VALUE_TYPE_USED_TALENT			 "used_talent"
#define NPC_XML_VALUE_TYPE_GAINED_BUFF			 "gained_buff"
#define NPC_XML_VALUE_TYPE_ENEMY_GAINED_BUFF	 "e_gained_buff"
#define NPC_XML_VALUE_TYPE_REAR					 "rear"
#define NPC_XML_VALUE_TYPE_FRONT				 "front"
#define NPC_XML_VALUE_TYPE_RAGE					 "rage"
#define NPC_XML_VALUE_TYPE_CHANGE_HIT_CAPSULE	 "change_hitcapsule"
#define NPC_XML_VALUE_TYPE_SAY					 "say"
#define NPC_XML_VALUE_TYPE_BALLON				 "ballon"
#define NPC_XML_VALUE_TYPE_SHOUT				 "shout"
#define NPC_XML_VALUE_TYPE_NARRATION			 "narration"
#define NPC_XML_VALUE_TYPE_AGGRO				 "aggro"
#define NPC_XML_VALUE_TYPE_FACETO				 "face_to"


#define NPC_XML_ATTR_AGGRO_FLIP					 L"flip"
#define NPC_XML_ATTR_AGGRO_RANDOM				 L"random"
#define NPC_XML_ATTR_AGGRO_FAR					 L"far"
#define NPC_XML_ATTR_AGGRO_NEAR					 L"near"
#define NPC_XML_ATTR_AGGRO_SHORT				 L"short"

bool GNPCInfoMgr::LoadColt(GNPCInfo* pNPCInfo, GNPCScriptBuilder& nsb)
{
	MXml xml;

	wchar_t szID[512];
	swprintf_s(szID, L"%07d", pNPCInfo->nID);

	wstring strPath;
	wstring strTestPath;
	strPath += PATH_SCRIPT_NPC_COLT;
	strPath += szID;
	strTestPath = strPath;
	strTestPath += L"_debug";
	strTestPath += FILEEXT_COLT;
	strPath += FILEEXT_COLT;
	
	if (MIsExistFile(strTestPath.c_str()))
	{
		// 로컬 테스트파일이 존재함
		strPath = strTestPath;
	}
	
	if (!MIsExistFile(strPath.c_str()))
		return false;	// 파일이 존재하지 않음

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strPath.c_str()).c_str())) 
	{
		mlog3("Colt Load Failed(%d): 문법이 잘못되었습니다.\n", pNPCInfo->nID);
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), NPC_XML_TAG_COLT))
		{
			ParseCOLT(pElement, &xml, pNPCInfo);
		}
		else if (!_stricmp(pElement->Value(), NPC_XML_TAG_SCRIPT))
		{
			ParseScript(pElement, &xml, nsb);
		}
	}

	return true;
}

void GNPCInfoMgr::ParseScript( MXmlElement* pElement, MXml* pXml, GNPCScriptBuilder& nsb )
{
	const char* pszText = pElement->GetText();
	if (!pszText)	return;
	string strText = pXml->ToAnsi(pszText);
	strText += "\n";

	nsb.SetXmlLuaScript(strText.c_str());
}

void GNPCInfoMgr::ParseCOLT(MXmlElement* pElement, MXml* pXml, GNPCInfo* pNPCInfo)
{
	GColtGroupInfo& ColtInfo = pNPCInfo->ColtInfo;

	_Attribute(&pNPCInfo->fChaseMaxDurationTime,	pElement, NPC_XML_ATTR_MAX_DURATION);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_COMBAT))
		{
			ParseCOMBAT(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_REACTION))
		{
			ParseREACTION(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_IDLE))
		{
			ParseIDLE(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_AGGRO))
		{
			ParseAGGRO(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_STRESS))
		{
			ParseSTRESS(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_PROLOGUE))
		{
			ParsePROLOGUE(pChild, pXml, ColtInfo);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_EPILOGUE))
		{
			ParseEPILOGUE(pChild, pXml, ColtInfo);
		}
	}
}

void GNPCInfoMgr::ParseCOMBAT(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	int nMode = NPC_MODE_DEFAULT;
	_Attribute(&nMode,	pElement, NPC_XML_ATTR_COLT_MODE);
	VALID(nMode >= NPC_MODE_DEFAULT);
	VALID(nMode < NPC_MODE_MAX);

	GColtGroupInfo::CombatAttr attr;
	_Attribute(&attr.nDownAttackTalentID,	pElement, NPC_XML_ATTR_DOWNATTACK);
	_Attribute(&attr.nVictoryTalentID,	pElement, NPC_XML_ATTR_VICTORY);
	ai.attrCombat[nMode] = attr;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, true);
			ai.vecCombatChecks[nMode].push_back(Check);
		}
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			ParseDEFAULT(pChild, pXml, ai.checkCombatDefault[nMode], true);
		}
	}
}

void GNPCInfoMgr::ParseREACTION(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecReactionChecks.push_back(Check);
		}
	}
}

void GNPCInfoMgr::ParseAGGRO(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecAggroChecks.push_back(Check);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			ParseDEFAULT(pChild, pXml, ai.checkAggroDefault, false);
		}
	}
}

void GNPCInfoMgr::ParseSTRESS(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecStressChecks.push_back(Check);
		}
	}
}

void GNPCInfoMgr::ParsePROLOGUE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecPrologue.push_back(Check);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			ParseDEFAULT(pChild, pXml, ai.checkPrologueDefault, false);
		}
	}
}

void GNPCInfoMgr::ParseEPILOGUE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecEpilogue.push_back(Check);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			ParseDEFAULT(pChild, pXml, ai.checkEpilogueDefault, false);
		}
	}
}


void GNPCInfoMgr::ParseIDLE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* Check = new GColtCheck;
			ParseCHECK(pChild, pXml, Check, false);		// Weaken의 모든 Action은 auto_run이다.
			ai.vecIdleChecks.push_back(Check);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			ParseDEFAULT(pChild, pXml, ai.checkIdleDefault, false);
		}
	}
}

void GNPCInfoMgr::ParseCHECK(MXmlElement* pElement, MXml* pXml, GColtCheck* Check, bool bIsCombat)
{
	string strValue;

	if (_Attribute(strValue,									pElement, NPC_XML_ATTR_TYPE))
	{
		if		(strValue == NPC_XML_VALUE_TYPE_HP)					{ Check->nType = AST_HP; }
		else if (strValue == NPC_XML_VALUE_TYPE_EN)					{ Check->nType = AST_EN; }
		else if (strValue == NPC_XML_VALUE_TYPE_SP)					{ Check->nType = AST_SP; }
		else if (strValue == NPC_XML_VALUE_TYPE_UNBREAKPART)		{ Check->nType = AST_UNBREAKPART; }
		else if (strValue == NPC_XML_VALUE_TYPE_BREAKPART)			{ Check->nType = AST_BREAKPART; }
		else if (strValue == NPC_XML_VALUE_TYPE_DAMAGE)				{ Check->nType = AST_DAMAGE; }
		else if (strValue == NPC_XML_VALUE_TYPE_MOTIONFACTOR)		{ Check->nType = AST_MOTIONFACTOR; }
		else if (strValue == NPC_XML_VALUE_TYPE_MODE)				{ Check->nType = AST_MODE; }
		else if (strValue == NPC_XML_VALUE_TYPE_DISTANCE)			{ Check->nType = AST_DISTANCE; }
		else if (strValue == NPC_XML_VALUE_TYPE_ANGLE)				{ Check->nType = AST_ANGLE; }
		else if	(strValue == NPC_XML_VALUE_TYPE_ENEMY_HP)			{ Check->nType = AST_ENEMY_HP; }
		else if (strValue == NPC_XML_VALUE_TYPE_ENEMY_MOTIONFACTOR)	{ Check->nType = AST_ENEMY_MOTIONFACTOR; }
		else if (strValue == NPC_XML_VALUE_TYPE_STRESS)				{ Check->nType = AST_STRESS; }
		else if (strValue == NPC_XML_VALUE_TYPE_HIT_CAPSULE)		{ Check->nType = AST_HIT_CAPSULE; }
		else if (strValue == NPC_XML_VALUE_TYPE_USED_TALENT)		{ Check->nType = AST_USED_TALENT; }
		else if (strValue == NPC_XML_VALUE_TYPE_GAINED_BUFF)		{ Check->nType = AST_GAINED_BUFF; }
		else if (strValue == NPC_XML_VALUE_TYPE_ENEMY_GAINED_BUFF)	{ Check->nType = AST_ENEMY_GAINED_BUFF; }
		else if (strValue == NPC_XML_VALUE_TYPE_REAR)				{ Check->nType = AST_REAR; }
		else if (strValue == NPC_XML_VALUE_TYPE_FRONT)				{ Check->nType = AST_FRONT; }
		else if (strValue == NPC_XML_VALUE_TYPE_RAGE)				{ Check->nType = AST_RAGE; }
	}

	if (_Attribute(strValue, pElement, NPC_XML_ATTR_PARAM1)) 
	{
		int& nParamValue = Check->vecParams[0].second;
		Check->vecParams[0].first = true;

		switch(Check->nType)
		{
		case AST_DAMAGE:				{ nParamValue = GStrings::StringToDamageAttrib(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str());			}break;
		case AST_MOTIONFACTOR:			
		case AST_ENEMY_MOTIONFACTOR:	{ nParamValue = GStrings::StringToMotionfactorType(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str());	}break;
		default:
			nParamValue = atoi(strValue.c_str());
		}
	}
	if (_Attribute(strValue, pElement, NPC_XML_ATTR_PARAM2)) 
	{
		int& nParamValue = Check->vecParams[1].second;
		Check->vecParams[1].first = true;
		nParamValue = atoi(strValue.c_str());
	}
	if (_Attribute(Check->vecParams[2].second, pElement, NPC_XML_ATTR_PARAM3)) Check->vecParams[2].first = true;
	if (_Attribute(Check->vecParams[3].second, pElement, NPC_XML_ATTR_PARAM4)) Check->vecParams[3].first = true;
	
	_Attribute(&Check->nPickRate, pElement, NPC_XML_ATTR_RATE);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_ACTION))
		{
			GColtAction NewAction;
			ParseACTION(pChild, pXml, NewAction, bIsCombat);
			if (NewAction.bAutoRun)		Check->vecAutoRunActions.push_back(NewAction);
			else						Check->vecActions.push_back(NewAction);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_CHECK))
		{
			GColtCheck* pNewCheck = new GColtCheck;
			ParseCHECK(pChild, pXml, pNewCheck, bIsCombat);
			Check->vecChecks.push_back(pNewCheck);
		}
		else if (!_stricmp(pChild->Value(), NPC_XML_TAG_DEFAULT))
		{
			Check->pCheckDefault = new GColtCheck;
			ParseDEFAULT(pChild, pXml, *Check->pCheckDefault, bIsCombat);
		}
	}
}

void GNPCInfoMgr::ParseDEFAULT(MXmlElement* pElement, MXml* pXml, GColtCheck& Check, bool bIsCombat)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), NPC_XML_TAG_ACTION))
		{
			GColtAction NewAction;
			ParseACTION(pChild, pXml, NewAction, bIsCombat);
			if (NewAction.bAutoRun)		Check.vecAutoRunActions.push_back(NewAction);
			else						Check.vecActions.push_back(NewAction);
		}
	}
}

void GNPCInfoMgr::ParseACTION(MXmlElement* pElement, MXml* pXml, GColtAction& Action, bool bIsCombat)
{
	string strValue;

	if (_Attribute(strValue,									pElement, NPC_XML_ATTR_TYPE))
	{
		if (strValue == NPC_XML_VALUE_TYPE_NOTHING)						{ Action.nType = CAT_NOTHING; }
		else if (strValue == NPC_XML_VALUE_TYPE_GROUP)					{ Action.nType = CAT_GROUP; }
		else if (strValue == NPC_XML_VALUE_TYPE_TALENT)					{ Action.nType = CAT_TALENT; }
		else if (strValue == NPC_XML_VALUE_TYPE_LUA)					{ Action.nType = CAT_LUAFUNC; }
		else if (strValue == NPC_XML_VALUE_TYPE_BREAKPART)				{ Action.nType = CAT_BREAKPART; }
		else if (strValue == NPC_XML_VALUE_TYPE_DAMAGE)					{ Action.nType = CAT_DAMAGE; }
		else if (strValue == NPC_XML_VALUE_TYPE_MOTIONFACTOR)			{ Action.nType = CAT_MOTIONFACTOR; }
		else if (strValue == NPC_XML_VALUE_TYPE_FLEE)					{ Action.nType = CAT_FLEE; }
		else if (strValue == NPC_XML_VALUE_TYPE_YELL)					{ Action.nType = CAT_YELL; }
		else if (strValue == NPC_XML_VALUE_TYPE_GUARD)					{ Action.nType = CAT_GUARD; }
		else if (strValue == NPC_XML_VALUE_TYPE_ADJUST)					{ Action.nType = CAT_ADJUST; }
		else if (strValue == NPC_XML_VALUE_TYPE_DISTANCE)				{ Action.nType = CAT_DISTANCE; }
		else if (strValue == NPC_XML_VALUE_TYPE_FOLLOW)					{ Action.nType = CAT_FOLLOW; }
		else if (strValue == NPC_XML_VALUE_TYPE_MOVE)					{ Action.nType = CAT_MOVE; }
		else if (strValue == NPC_XML_VALUE_TYPE_CHANGE_HIT_CAPSULE)		{ Action.nType = CAT_CHANGE_HIT_CAPSULE; }
		else if (strValue == NPC_XML_VALUE_TYPE_SAY)					{ Action.nType = CAT_SAY; 	}
		else if (strValue == NPC_XML_VALUE_TYPE_BALLON)					{ Action.nType = CAT_BALLOON; 	}
		else if (strValue == NPC_XML_VALUE_TYPE_SHOUT)					{ Action.nType = CAT_SHOUT; }
		else if (strValue == NPC_XML_VALUE_TYPE_NARRATION)				{ Action.nType = CAT_NARRATION; }
		else if (strValue == NPC_XML_VALUE_TYPE_AGGRO)					{ Action.nType = CAT_AGGRO; }
		else if (strValue == NPC_XML_VALUE_TYPE_FACETO)					{ Action.nType = CAT_FACETO; }
	}

	if (_Attribute(strValue, pElement, NPC_XML_ATTR_PARAM1, pXml)) 
	{
		wstring& strParamValue = Action.qParams[0];

		switch(Action.nType)
		{
		case CAT_DAMAGE:		{ CSStrings::StringToDamageAttrib(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str(), strParamValue); }	break;
		case CAT_MOTIONFACTOR:	{ CSStrings::StringToMotionfactorType(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str(), strParamValue);	}break;
		default:
			strParamValue = MLocale::ConvAnsiToUCS2(strValue.c_str());
		}
	}
	_Attribute(Action.qParams[1], pElement, NPC_XML_ATTR_PARAM2, pXml);
	_Attribute(Action.qParams[2], pElement, NPC_XML_ATTR_PARAM3, pXml);
	_Attribute(Action.qParams[3], pElement, NPC_XML_ATTR_PARAM4, pXml);

	_Attribute(Action.nRate,							pElement, NPC_XML_ATTR_RATE);
	_Attribute(Action.nMaxCount,						pElement, NPC_XML_ATTR_MAXCOUNT);
	_Attribute(&Action.fCycleTime,						pElement, NPC_XML_ATTR_CYCLETIME);
	_Attribute(&Action.bGainStress,						pElement, NPC_XML_ATTR_STRESS);
	if (_Attribute(&Action.fDurationTime,					pElement, NPC_XML_ATTR_DURATIONTIME))
	{
		Action.bModifyDurationTime = true;
	}
	
	if (bIsCombat)
	{
		_Attribute(&Action.bAutoRun,						pElement, NPC_XML_ATTR_AUTORUN);
	}

	// 연계 액션 처리
	{
		MXmlElement* pChild = pElement->FirstChildElement();
		for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
		{
			if (!_stricmp(pChild->Value(), NPC_XML_TAG_ACTION))
			{
				GColtAction NewAction;
				ParseACTION(pChild, pXml, NewAction, bIsCombat);
				Action.vecSeqActions.push_back(NewAction);
			}
		}
	}
}
