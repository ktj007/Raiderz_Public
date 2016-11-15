#include "stdafx.h"
#include "XStrings.h"
#include "XDef.h"
#include "atlstr.h"
#include "CSSoul.h"
#include "CSTalentInfo.h"
#include "MFileSystem.h"
#include "MStringUtil.h"
#include "CSQuestInfo.h"
#include "CSFactionInfo.h"
#include "CSFormatString.h"
#include "CLangInfo.h"

vector<tstring>	XStrings::m_vecStrChatDrainType;
vector<tstring>	XStrings::m_vecStrItemTierKey;
vector<tstring>	XStrings::m_vecStrWeaponTypeKey;
vector<tstring>	XStrings::m_vecStrArmorTypeKey;
vector<tstring>	XStrings::m_vecStrSoundType;
vector<tstring>	XStrings::m_vecStrVoiceType;				
vector<tstring>	XStrings::m_vecStrVoiceSay;				
vector<tstring>	XStrings::m_vecKorStrFactionRelationType;

bool XStrings::Init()
{
	if (false == CSStrings::Init()) return false;

	m_vecStrChatDrainType.clear();
	m_vecStrChatDrainType.push_back(_T("HP"));
	m_vecStrChatDrainType.push_back(_T("EN"));
	m_vecStrChatDrainType.push_back(_T("STA"));
	if (TALENT_DRAIN_MAX != m_vecStrChatDrainType.size()) return false;

	m_vecStrItemTierKey.clear();
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_VERY_COMMON"));
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_COMMON"));
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_RARE"));
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_TREASURE"));
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_LEGENDARY"));
	m_vecStrItemTierKey.push_back(_T("ITEM_TIER_EPIC"));
	if (ITEM_TIER_MAX != m_vecStrItemTierKey.size()) return false;


	m_vecStrWeaponTypeKey.clear();
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_NONE"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_1H_SLASH"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_1H_BLUNT"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_1H_PIERCE"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_TWO_HANDED"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_STAFF"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_ARCHERY"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_2H_BLUNT"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_DUAL_WIELD"));
	m_vecStrWeaponTypeKey.push_back(_T("WEAPON_DUAL_PIERCE"));
	if (WEAPON_MAX != m_vecStrWeaponTypeKey.size()) return false;

	m_vecStrArmorTypeKey.clear();
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_NONE"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_CLOTH"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_LIGHT"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_MEDIUM"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_HEAVY"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_SHIELD"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_ACCESSORY"));
	m_vecStrArmorTypeKey.push_back(_T("ARMOR_AMULET"));
	if (ARMOR_MAX != m_vecStrArmorTypeKey.size()) return false;

	m_vecStrSoundType.clear();
	m_vecStrSoundType.reserve(SOUND_TYPE_MAX);
	m_vecStrSoundType.push_back(_T("universal"));
	m_vecStrSoundType.push_back(_T("cloth"));
	m_vecStrSoundType.push_back(_T("leather"));
	m_vecStrSoundType.push_back(_T("chain"));
	m_vecStrSoundType.push_back(_T("iron"));
	m_vecStrSoundType.push_back(_T("wood"));
	m_vecStrSoundType.push_back(_T("cutlery"));
	m_vecStrSoundType.push_back(_T("paper"));
	m_vecStrSoundType.push_back(_T("stone"));
	m_vecStrSoundType.push_back(_T("liquid"));
	m_vecStrSoundType.push_back(_T("jewel"));
	m_vecStrSoundType.push_back(_T("organ"));
	m_vecStrSoundType.push_back(_T("hammer"));
	m_vecStrSoundType.push_back(_T("coin"));
	if (SOUND_TYPE_MAX != m_vecStrSoundType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrVoiceType.clear();
	m_vecStrVoiceType.push_back(_T("male1"));
	m_vecStrVoiceType.push_back(_T("soldiermale1"));
	m_vecStrVoiceType.push_back(_T("renasianmale1"));
	m_vecStrVoiceType.push_back(_T("male2"));
	m_vecStrVoiceType.push_back(_T("soldiermale2"));
	m_vecStrVoiceType.push_back(_T("renasianmale2"));
	m_vecStrVoiceType.push_back(_T("male3"));
	m_vecStrVoiceType.push_back(_T("soldiermale3"));
	m_vecStrVoiceType.push_back(_T("renasianmale3"));
	m_vecStrVoiceType.push_back(_T("male4"));
	m_vecStrVoiceType.push_back(_T("soldiermale4"));
	m_vecStrVoiceType.push_back(_T("renasianmale4"));
	m_vecStrVoiceType.push_back(_T("female1"));
	m_vecStrVoiceType.push_back(_T("soldierfemale1"));
	m_vecStrVoiceType.push_back(_T("renasianfemale1"));
	m_vecStrVoiceType.push_back(_T("female2"));
	m_vecStrVoiceType.push_back(_T("soldierfemale2"));
	m_vecStrVoiceType.push_back(_T("renasianfemale2"));
	m_vecStrVoiceType.push_back(_T("female3"));
	m_vecStrVoiceType.push_back(_T("soldierfemale3"));
	m_vecStrVoiceType.push_back(_T("renasianfemale3"));
	m_vecStrVoiceType.push_back(_T("female4"));
	m_vecStrVoiceType.push_back(_T("soldierfemale4"));
	m_vecStrVoiceType.push_back(_T("renasianfemale4"));
	m_vecStrVoiceType.push_back(_T("fatmale1"));
	m_vecStrVoiceType.push_back(_T("fatmale2"));
	m_vecStrVoiceType.push_back(_T("fatfemale"));
	if (VOICE_TYPE_MAX != m_vecStrVoiceType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrVoiceSay.clear();
	m_vecStrVoiceSay.push_back(_T("hello1"));
	m_vecStrVoiceSay.push_back(_T("hello2"));
	m_vecStrVoiceSay.push_back(_T("hello3"));
	m_vecStrVoiceSay.push_back(_T("bye1"));
	m_vecStrVoiceSay.push_back(_T("bye2"));
	m_vecStrVoiceSay.push_back(_T("bye3"));
	m_vecStrVoiceSay.push_back(_T("bye4"));
	m_vecStrVoiceSay.push_back(_T("shop1"));
	m_vecStrVoiceSay.push_back(_T("shop2"));
	m_vecStrVoiceSay.push_back(_T("shop3"));
	m_vecStrVoiceSay.push_back(_T("quest1"));
	m_vecStrVoiceSay.push_back(_T("quest2"));
	m_vecStrVoiceSay.push_back(_T("quest3"));
	m_vecStrVoiceSay.push_back(_T("report1"));
	m_vecStrVoiceSay.push_back(_T("report2"));
	m_vecStrVoiceSay.push_back(_T("report3"));
	m_vecStrVoiceSay.push_back(_T("craft1"));
	m_vecStrVoiceSay.push_back(_T("craft2"));
	m_vecStrVoiceSay.push_back(_T("craft3"));
	m_vecStrVoiceSay.push_back(_T("sigh1"));
	m_vecStrVoiceSay.push_back(_T("sigh2"));
	m_vecStrVoiceSay.push_back(_T("laugh"));
	m_vecStrVoiceSay.push_back(_T("cry"));
	m_vecStrVoiceSay.push_back(_T("cheer"));
	if (VOICE_SAY_MAX != m_vecStrVoiceSay.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecKorStrFactionRelationType.clear();
	m_vecKorStrFactionRelationType.reserve(FRT_MAX);
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_NONE"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_WORST"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_BAD"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_NORMAL"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_GOOD"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_EXCELLENT"));
	m_vecKorStrFactionRelationType.push_back(_T("FACTION_TRUSTY"));
	if (FRT_MAX != m_vecKorStrFactionRelationType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	return true;
}

const TCHAR* XStrings::DrainTypeToChatString(int nDrainType)
{
	return XGetStr(EnumToString(nDrainType, m_vecStrChatDrainType));
}

tstring XStrings::Sex( SEX nSex )
{
	if (nSex == SEX_FEMALE) return XStringResMgr::GetInstance()->GetString(_T("FEMALE"));
	return XStringResMgr::GetInstance()->GetString(_T("MALE"));
}

tstring XStrings::Race( RACE nRace )
{
	if (nRace == RACE_HUMAN) return XStringResMgr::GetInstance()->GetString(_T("HUMAN"));
	return XStringResMgr::GetInstance()->GetString(_T("HUMAN"));
}

void XStrings::SaveCommandResultTable()
{
	MFile mf;
	if (!mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_COMMAND_RESULT_TABLE_H).c_str())) return;

	unsigned int nLength = mf.GetLength();
	if (nLength <= 0) return;

	char* szBuffer = new char[nLength+1];
	char* szNextToken;
	if (true == mf.Read(szBuffer, nLength))
	{
		MXml xml;

		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
		xml.Doc()->LinkEndChild(pDec);
		MXmlElement* pElement = new MXmlElement( "maiet" );
		xml.Doc()->LinkEndChild(pElement);

		char* szLine = strtok_s(szBuffer, "\n", &szNextToken);
		char* szLineNextToken = NULL;
		while(NULL != szLine)
		{
			if (NULL != strstr(szLine, "}"))
			{
				break;
			}

			if (NULL != strstr(szLine, "="))
			{
				MXmlElement* pResultElement = new MXmlElement( "RESULT" );
				pElement->LinkEndChild(pResultElement);

				char* szCode = strtok_s(szLine, "=", &szLineNextToken);
				if (NULL == szCode) break;
				char* szKey = strtok_s(NULL, ",", &szLineNextToken);		
				if (NULL == szKey) break;
				char* szTemp = strtok_s(NULL, " ", &szLineNextToken);
				char* szString = strtok_s(NULL, "|", &szLineNextToken);		
				if (NULL == szString) break;

				pResultElement->SetAttribute("code",	cml2::TrimBlanks(szCode));
				pResultElement->SetAttribute("key",		cml2::TrimBlanks(szKey));
				pResultElement->SetAttribute("string",	cml2::TrimBlanks(szString));
			}		

			szLine = strtok_s(NULL, "\n", &szNextToken);
		}

		xml.Doc()->SaveFile(MLocale::ConvTCHARToAnsi(PATH_LANG_KOREAN FILENAME_RESULT_STRINGS).c_str());
	}

	SAFE_DELETE_ARRAY(szBuffer);	
}

void XStrings::SaveCommandResultSoundTable()
{
	MFile mf;
	if (!mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_COMMAND_RESULT_TABLE_H).c_str())) return;

	unsigned int nLength = mf.GetLength();
	if (nLength <= 0) return;

	char* szBuffer = new char[nLength+1];
	char* szNextToken;
	if (true == mf.Read(szBuffer, nLength))
	{
		MXml xml;

		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
		xml.Doc()->LinkEndChild(pDec);
		MXmlElement* pElement = new MXmlElement( "maiet" );
		xml.Doc()->LinkEndChild(pElement);

		char* szLine = strtok_s(szBuffer, "\n", &szNextToken);
		char* szLineNextToken = NULL;
		while(NULL != szLine)
		{
			if (NULL != strstr(szLine, "}"))	break;

			if (NULL != strstr(szLine, "="))
			{
				char* szCode = strtok_s(szLine, "=", &szLineNextToken);
				if (NULL == szCode) break;
				char* szKey = strtok_s(NULL, ",", &szLineNextToken);		
				if (NULL == szKey) break;
				char* temp = strtok_s(NULL, "|", &szLineNextToken);
				char* szString = szLineNextToken;					
				if (!strcmp("", szString)) continue;

				MXmlElement* pResultElement = new MXmlElement( "RESULT" );
				pElement->LinkEndChild(pResultElement);

				pResultElement->SetAttribute("code",	cml2::TrimBlanks(szCode));
				pResultElement->SetAttribute("key",		cml2::TrimBlanks(szKey));
				pResultElement->SetAttribute("string",	cml2::TrimBlanks(szString));
			}		

			szLine = strtok_s(NULL, "\n", &szNextToken);
		}

		xml.Doc()->SaveFile(MLocale::ConvTCHARToAnsi(PATH_LANG_KOREAN FILENAME_RESULT_SOUND).c_str());
	}

	SAFE_DELETE_ARRAY(szBuffer);	
}

const TCHAR* XStrings::WeaponTypeKey( int nWeaponType )
{
	return EnumToString(nWeaponType, m_vecStrWeaponTypeKey);
}

const TCHAR* XStrings::ItemTierKey( int nItemTier )
{
	return EnumToString(nItemTier, m_vecStrItemTierKey);
}

const TCHAR* XStrings::ArmorTypeKey( int nArmorType )
{
	return EnumToString(nArmorType, m_vecStrArmorTypeKey);
}

int XStrings::StringToSoundType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrSoundType);
}

const TCHAR* XStrings::SoundTypeToString(int nSoundType)
{
	return (m_vecStrSoundType[nSoundType]).c_str();
}

int XStrings::StringToVoiceType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrVoiceType);
}

const TCHAR* XStrings::VoiceTypeToString(int nVoiceType)
{
	return (m_vecStrVoiceType[nVoiceType]).c_str();
}

const TCHAR* XStrings::VoiceSayToString(int nVoiceSay)
{
	return (m_vecStrVoiceSay[nVoiceSay]).c_str();
}

const TCHAR* XStrings::FacionRelationTypeToString( int nType )
{
	return XGetStr(EnumToString(nType, m_vecKorStrFactionRelationType));
}

void XStrings::GetCommaStringSplitter( const tstring& strValue, vector<tstring>& vecOut )
{
	tstring t = strValue;
	tstring motion, motionret = _T("");
	while(1)
	{
		int f = t.find(_T(","));
		if (f != wstring::npos)
		{
			tstring motion = t.substr(0, f);
			vecOut.push_back(motion);

			t = t.substr(f+1);
		}
		else
		{
			vecOut.push_back(t);
			return;
		}
	}
}

/////////////////////////////////////////////////////

XStringResMgr* XStringResMgr::GetInstance()
{
	static XStringResMgr m_stMgr;
	return &m_stMgr;
}

XStringResMgr::XStringResMgr() : CStringResMgr()
{
}

XStringResMgr::~XStringResMgr()
{
	Clear();
}

bool XStringResMgr::OnInit(const CLangNode& node)
{
	CStringRes<int>* pTable;

	result_map::iterator it = m_mapResultStrings.find(MLocale::ConvUTF16ToTCHAR(node.strLocale.c_str()));
	
	if (it == m_mapResultStrings.end())
	{
		 pTable = new CStringRes<int>(_T("RESULT"));
		 m_mapResultStrings.insert(make_pair(MLocale::ConvUTF16ToTCHAR(node.strLocale.c_str()), pTable));
	}
	else
	{
		pTable = it->second;
	}

	
	tstring strFileName = MLocale::ConvUTF16ToTCHAR(node.strPath.c_str()) + FILENAME_RESULT_STRINGS;

	if (!pTable->Load(strFileName.c_str()))
	{
		mlog("Error!! - %s Load Failed\n", MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str());
		return false;
	}	

	return true;
}

void XStringResMgr::Clear(void)
{
	__super::Clear();

	for each(result_map::value_type table in m_mapResultStrings)
	{
		delete table.second;
	}

	m_mapResultStrings.clear();
}

const TCHAR* XStringResMgr::GetCmdResultString(int nKey)
{
	result_map::iterator it = m_mapResultStrings.find(m_strLocale);

	if (it == m_mapResultStrings.end())
	{
		return _T("");
	}

	CStringRes<int>* pTable = it->second;
	return pTable->GetStr(nKey);
}

void XStringResMgr::TransKeyStr( TCHAR* poutDestStr, const int nDestStrSize, const TCHAR* szSrcStr )
{
	const TCHAR* szString = GetStringIncludedKeyString(szSrcStr);

	if (szString)
	{
		_tcscpy_s(poutDestStr, nDestStrSize, szString);
	}
	else
	{
		_tcscpy_s(poutDestStr, nDestStrSize, szSrcStr);
	}
}

void XStringResMgr::TransKeyStr( tstring& outDestStr, const TCHAR* szSrcStr )
{
	const TCHAR* szString = GetStringIncludedKeyString(szSrcStr);
	if (szString)
	{
		outDestStr = szString;
	}
	else
	{
		outDestStr = szSrcStr;
	}
}

tstring XStringResMgr::TransKeyStrWithParam(tstring strKeyWithParam)
{
	// "$$MESSAGE_KEY {param=test1|test2|test3}" - 형식 문자열을 파싱한다.

	tstring strParam;
	tstring strKey;

	tr1::tregex pattern(_T(" *\\{param=(.*)\\}"));
	tr1::match_results<tstring::const_iterator> result;

	if (tr1::regex_search(strKeyWithParam, result, pattern))
	{
		strParam = result[1];
		strKey = tr1::regex_replace(strKeyWithParam, pattern, tstring(_T("")));
	}
	else
	{
		strKey = strKeyWithParam;
	}

		
	tstring strResult;
	TransKeyStr(strResult, strKey.c_str());	

	return CSFormatString::Format(strResult, FSParam(strParam));
}

const TCHAR* XStringResMgr::GetStringIncludedKeyString( const TCHAR* szKey )
{
	if ( szKey == NULL)
		return NULL;

	size_t nSrcLen = _tcslen(szKey);
	if (nSrcLen <= 2 ||
		(szKey[0] != _T('$')) ||
		(szKey[1] != _T('$')))
	{
		return NULL;
	}

	TCHAR szRawKey[256];
	_tcscpy_s(szRawKey, &szKey[2]);

	return GetString(szRawKey);	
}
