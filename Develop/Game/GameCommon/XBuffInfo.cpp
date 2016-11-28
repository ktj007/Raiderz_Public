#include "stdafx.h"
#include "XBuffInfo.h"
#include "MUtil.h"
#include "CSCommonParser.h"
#include "XTalentEffectInfo.h"
#include "CSStrings.h"
#include "XTalentInfoDef.h"
#include "CSBuffInfoDef.h"
#include "MLocale.h"
#include "XPartsSlotType.h"
#include "CSFormatString.h"
#include "XStrings.h"

XBuffInfo::XBuffInfo()
: m_bExistModValue(false)
, m_bLogable(true)
, m_nExtraActiveModelID(0)
, m_bShowHitReaction(false)
{

}

void XBuffInfo::InitSuffix()
{
	m_bExistModValue = m_InstantModifier.IsModified() || m_ActorModifier.IsModified() || m_PlayerModifier.IsModified();
}

bool XBuffInfo::IsInvisibilityToNPC()
{
	return IsMatchPassiveExtraAttrib(BUFPEA_INVISIBILITYTONPC);
}

bool XBuffInfo::IsInvisibility()
{
	return IsMatchPassiveExtraAttrib(BUFPEA_INVISIBILITY);
}

const TCHAR* XBuffInfo::GetName(void)
{
	return XStringResMgr::GetInstance()->GetString(m_strName);
}

const TCHAR* XBuffInfo::GetDesc(void)
{
	return XStringResMgr::GetInstance()->GetString(m_szDesc);
}

void XBuffInfo::CopyAll( CSBuffInfo* pTarget )
{
	XBuffInfo* pTargetClient = static_cast<XBuffInfo*>(pTarget);
	*pTargetClient = *this;
}

XBuffInfoMgr::~XBuffInfoMgr()
{
	Clear();
}

void XBuffInfoMgr::Clear()
{
	while(!empty()) 
	{
		XBuffInfo* pTalentInfo = static_cast<XBuffInfo*>(begin()->second);
		delete pTalentInfo;
		erase(begin());
	}
}

XBuffInfo* XBuffInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (XBuffInfo*)((*itor).second);
}

bool XBuffInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool XBuffInfoMgr::Load( const TCHAR* szBuffInfoFileName, const TCHAR* szBuffEffectFileName, const TCHAR* szEnchantBuffEffectFileName )
{
	if(LoadBuffInfo(szBuffInfoFileName) == false) return false;
	if(LoadBuffEffectInfo(szBuffEffectFileName) == false) return false;
	if(LoadEnchantBuffEffectInfo(szEnchantBuffEffectFileName) == false) return false;

	return true;
}

bool XBuffInfoMgr::LoadBuffInfo( tstring strFileName )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF))
		{
			ParseBuff(pElement, &xml);
		}
	}

	return true;
}

bool XBuffInfoMgr::LoadBuffEffectInfo( tstring strFileName )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF_EFFECT))
		{
			ParseBuffEffect(pElement, &xml);
		}
	}

	// refence
	RefenceBuffEffectInfo();

	return true;
}

bool XBuffInfoMgr::LoadEnchantBuffEffectInfo( tstring strFileName )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_EFFECT))
		{
			ParseEnchantBuffEffect(pElement, &xml);
		}
	}

	// refence
	RefenceEnchantBuffEffectInfo();

	return true;
}

void XBuffInfoMgr::ParseBuff(MXmlElement* pElement, MXml* pXml)
{
	tstring strName;
	tstring strValue;
	XBuffInfo* pNewInfo = new XBuffInfo;

	int nID = INVALID_BUFF_ID;
	_Attribute(&nID,			pElement, BUFF_XML_ATTR_ID);

	int nOverride = INVALID_ID;
	if (_Attribute(&nOverride,			pElement, BUFF_XML_ATTR_OVERRIDE))
	{
		XBuffInfo* pSourceInfo = Get(nOverride);
		if (pSourceInfo)
		{
			pSourceInfo->CopyAll(pNewInfo);
		}
		else
		{
			mlog3("invalid override buffid (source: %d, target: %d)\n", nOverride, nID);
		}
	}

	pNewInfo->m_nID = nID;

	// 클라이언트, 서버 공통으로 읽는 값
	ParseBuffCommon(pNewInfo, pElement, pXml);
	
	pNewInfo->m_strName = CSFormatString::Format(_T("BUFF_NAME_{0}"), FSParam(pNewInfo->m_nID));
	pNewInfo->m_szDesc = CSFormatString::Format(_T("BUFF_DESC_{0}"), FSParam(pNewInfo->m_nID));
	
	_Attribute(&pNewInfo->m_nStackSlot,		pElement, BUFF_XML_ATTR_STACKSLOT);
	_Attribute(&pNewInfo->m_nStackPower,	pElement, BUFF_XML_ATTR_STACKPOWER);

	int nDamageType=0;
	_Attribute(&pNewInfo->m_nMinDamage,		pElement, BUFF_XML_ATTR_MIN_DAMAGE);
	_Attribute(&pNewInfo->m_nMaxDamage,		pElement, BUFF_XML_ATTR_MAX_DAMAGE);
	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_DAMAGE_ATTRIB)) 
	{
		pNewInfo->m_nDamageAttrib = DA_NONE;
		pNewInfo->m_nDamageAttrib = (DAMAGE_ATTRIB)CSStrings::StringToDamageAttrib(strValue.c_str());
	}

	_Attribute(pNewInfo->m_strEffectName,	pElement, BUFF_XML_ATTR_EFFECTNAME);
	_Attribute(pNewInfo->m_strBoneName,		pElement, BUFF_XML_ATTR_BONENAME);
	_Attribute(pNewInfo->m_strUseAniName,	pElement, BUFF_XML_ATTR_USEANI);
	_Attribute(pNewInfo->m_strAniPostfix,	pElement, BUFF_XML_ATTR_ANI_POSTFIX);
	_Attribute(pNewInfo->m_bLogable,		pElement, BUFF_XML_ATTR_LOGABLE);	
	_Attribute(pNewInfo->m_szIcon,			pElement, BUFF_XML_ATTR_ICON);
	_Attribute(&pNewInfo->m_nExtraActiveModelID, pElement, BUFF_XML_TAG_BUFF_EXTRA_ACTIVE_MODEL_ID);
	_Attribute(pNewInfo->m_bShowHitReaction, pElement, BUFF_XML_TAG_BUFF_SHOW_HIT_REACTION);

	CSMotionFactorParser::Parse(pElement, pNewInfo->m_MotionFactorGroup);

	pNewInfo->InitSuffix();

	// 해당 ID가 이미 있음
	if(find(pNewInfo->m_nID) != end())
	{
		mlog("(%s) Buff (%d)ID Duplication\n", MLocale::ConvTCHARToAnsi(pNewInfo->m_strName.c_str()).c_str(), pNewInfo->m_nID);
		_ASSERT(0);
	}

	insert(value_type(pNewInfo->m_nID, pNewInfo));
}

void XBuffInfoMgr::ParseBuffEffect( MXmlElement* pElement, MXml* pXml )
{
	int id = -1;

	// ID
	_Attribute(&id,	pElement, BUFF_XML_ATTR_EFFECT_ID);

	// 버프 정보 가져오기
	map<int , CSBuffInfo* > ::iterator it = find(id);
	if(it == end())
	{
		// 정보가 없다.
		dlog("ParseBuffEffect() 에서 버프(%d) 정보가 없다.\n", id);
		return ;
	}

	XBuffInfo* pBuffInfo = static_cast<XBuffInfo*>(it->second);

	if(pBuffInfo->m_BuffEffect.m_nBuffID > 0)
	{
		// 이미 있네...
		dlog("ParseBuffEffect() 에서 버프(%d) 중복 의심.\n", id);
	}

	// ID 입력
	pBuffInfo->m_BuffEffect.m_nBuffID = id;

	// reference 입력
	tstring strReferenceID;
	_Attribute(strReferenceID,	pElement, BUFF_XML_ATTR_REFERENCE_ID);
	vector<tstring> vecRefList;
	XStrings::GetCommaStringSplitter(strReferenceID, vecRefList);
	GetReferenceIDList(vecRefList, pBuffInfo->m_BuffEffect.m_vecRefID);

	// 속성
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		int nValue = 0;
		string strValue;
		XBuffEffectInfo *pNewInfo = new XBuffEffectInfo;

		if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_BUFF_GAIN_EFFECT))
			pNewInfo->m_nBuffEffectType = BUFF_GAIN_EFFECT;
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_BUFF_GAIN_LOOP_EFFECT))
			pNewInfo->m_nBuffEffectType = BUFF_GAIN_LOOP_EFFECT;
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_BUFF_GAIN_RELEASE_EFFECT))
			pNewInfo->m_nBuffEffectType = BUFF_GAIN_RELEASE_EFFECT;
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_BUFF_HIT_EFFECT))
			pNewInfo->m_nBuffEffectType = BUFF_HIT_EFFECT;
		else
		{
			// 모르는 내용
			SAFE_DELETE(pNewInfo);
			mlog("알수 없는 버프 이펙트 타입\n");
			continue;
		}

		_Attribute(pNewInfo->m_strEffectName,			pAttElement, TALENT_XML_ATTR_TALENT_EFFECTNAME);
		_Attribute(pNewInfo->m_strBoneName,				pAttElement, TALENT_XML_ATTR_TALENT_BONENAME);
		_Attribute(pNewInfo->m_bBuffWeaponEffect,		pAttElement, BUFF_XML_TAG_BUFF_WEAPON_EFFECT);

		pBuffInfo->m_BuffEffect.push_back(pNewInfo);
	}
}

bool XBuffInfoMgr::ReLoad( const TCHAR* szBuffInfoFileName, const TCHAR* szBuffEffectFileName, const TCHAR* szEnchantBuffEffectFileName )
{
	Clear();

	return Load(szBuffInfoFileName, szBuffEffectFileName, szEnchantBuffEffectFileName);
}

void XBuffInfoMgr::ParseEnchantBuffEffect( MXmlElement* pElement, MXml* pXml )
{
	int id = -1;

	// ID
	_Attribute(&id,	pElement, BUFF_XML_ATTR_EFFECT_ID);

	// 버프 정보 가져오기
	map<int , CSBuffInfo* > ::iterator it = find(id);
	if(it == end())
	{
		// 정보가 없다.
		dlog("ParseEnchantBuffEffect() 에서 버프(%d) 정보가 없다.\n", id);
		return ;
	}

	XBuffInfo* pBuffInfo = static_cast<XBuffInfo*>(it->second);

	if(pBuffInfo->m_EnchantBuffEffect.m_nBuffID > 0)
	{
		// 이미 있네...
		dlog("ParseEnchantBuffEffect() 에서 버프(%d) 중복 의심.\n", id);
	}

	// ID 입력
	pBuffInfo->m_EnchantBuffEffect.m_nBuffID = id;

	// reference 입력
	tstring strReferenceID;
	_Attribute(strReferenceID,	pElement, BUFF_XML_ATTR_REFERENCE_ID);
	vector<tstring> vecRefList;
	XStrings::GetCommaStringSplitter(strReferenceID, vecRefList);
	GetReferenceIDList(vecRefList, pBuffInfo->m_EnchantBuffEffect.m_vecRefID);


	// 속성
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_DEFAULT))
		{
			ParseEnchantBuffEffectInfo(pAttElement, pXml, &pBuffInfo->m_EnchantBuffEffect.m_DefaultEffect);
		}
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_EQUIP))
		{
			XEnchantBuffEffectEquipType* pEnchantEffect = new XEnchantBuffEffectEquipType;
			
			ParseEnchantBuffEffectInfo(pAttElement, pXml, pEnchantEffect);

			pBuffInfo->m_EnchantBuffEffect.push_back(pEnchantEffect);
		}
	}
}

void XBuffInfoMgr::ParseEnchantBuffEffectInfo( MXmlElement* pElement, MXml* pXml, XEnchantBuffEffectEquipType* pEffect )
{
	// Type
	string strType;
	_Attribute(strType,	pElement, BUFF_XML_ATTR_TYPE);

	pEffect->m_eEquipType = String2EnchantBuffEffectEquipType(strType);

	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		int nValue = 0;
		string strValue;
		XEnchantBuffEffectInfo *pNewInfo = new XEnchantBuffEffectInfo;

		if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_DUMMY_EFFECT))
			pNewInfo->m_nEnchantBuffEffectType = EBET_DUMMY_EFFECT;
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_TRAIL_EFFECT))
			pNewInfo->m_nEnchantBuffEffectType = EBET_TRAIL_EFFECT;
		else if(!_stricmp(pAttElement->Value(), BUFF_XML_TAG_ENCHANT_BUFF_HIT_EFFECT))
			pNewInfo->m_nEnchantBuffEffectType = EBET_HIT_EFFECT;
		else
		{
			// 모르는 내용
			SAFE_DELETE(pNewInfo);
			mlog("알수 없는 버프 이펙트 타입\n");
			continue;
		}

		_Attribute(pNewInfo->m_strEffectName,			pAttElement, TALENT_XML_ATTR_TALENT_EFFECTNAME);
		_Attribute(pNewInfo->m_strBoneName,				pAttElement, TALENT_XML_ATTR_TALENT_BONENAME);
		_Attribute(pNewInfo->m_strTrailTexName,			pAttElement, BUFF_XML_ATTR_TRAIL_EFFECT_TEXTURE_NAME);

		pEffect->push_back(pNewInfo);
	}
}

ENCHANT_BUFF_EFFECT_EQUIP_TYPE XBuffInfoMgr::String2EnchantBuffEffectEquipType( string& strType )
{
	if(strType == "1hs")
		return EBEET_1HS;
	else if(strType == "1hb")
		return EBEET_1HB;
	else if(strType == "1hp")
		return EBEET_1HP;
	else if(strType == "2hd")
		return EBEET_2HD;
	else if(strType == "sta")
		return EBEET_STA;
	else if(strType == "arc")
		return EBEET_ARC;
	else if(strType == "2hb")
		return EBEET_2HB;
	else if(strType == "dwd")
		return EBEET_DWD;
	else if(strType == "dwp")
		return EBEET_DWP;
	else if(strType == "head")
		return EBEET_HEAD;
	else if(strType == "body")
		return EBEET_BODY;
	else if(strType == "hands")
		return EBEET_HANDS;
	else if(strType == "leg")
		return EBEET_LEG;
	else if(strType == "feet")
		return EBEET_FEET;

	return EBEET_NONE;
}

void XBuffInfoMgr::RefenceBuffEffectInfo()
{
	for(map<int , CSBuffInfo* > ::iterator it = begin(); it != end(); ++it)
	{
		XBuffInfo* pBuffInfo = static_cast<XBuffInfo*>(it->second);
		for(vector<int>::iterator itRefID = pBuffInfo->m_BuffEffect.m_vecRefID.begin(); itRefID != pBuffInfo->m_BuffEffect.m_vecRefID.end(); ++itRefID)
		{
			map<int , CSBuffInfo* > ::iterator itFind = find(*itRefID);
			if(itFind != end())
			{
				XBuffInfo* pRefBuffInfo = static_cast<XBuffInfo*>(itFind->second);
				RecursiveRefenceBuffEffectInfo(pBuffInfo, pRefBuffInfo);

				SetRefenceBuffEffectInfo(pBuffInfo, pRefBuffInfo);
			}
		}
	}
}

void XBuffInfoMgr::RefenceEnchantBuffEffectInfo()
{
	for(map<int , CSBuffInfo* > ::iterator it = begin(); it != end(); ++it)
	{
		XBuffInfo* pBuffInfo = static_cast<XBuffInfo*>(it->second);
		for(vector<int>::iterator itRefID = pBuffInfo->m_EnchantBuffEffect.m_vecRefID.begin(); itRefID != pBuffInfo->m_EnchantBuffEffect.m_vecRefID.end(); ++itRefID)
		{
			map<int , CSBuffInfo* > ::iterator itFind = find(*itRefID);
			if(itFind != end())
			{
				XBuffInfo* pRefBuffInfo = static_cast<XBuffInfo*>(itFind->second);

				RecursiveRefenceEnchantBuffEffectInfo(pBuffInfo, pRefBuffInfo);

				SetRefenceEnchantBuffEffectInfo(pBuffInfo, pRefBuffInfo);
			}
		}
	}
}

void XBuffInfoMgr::GetReferenceIDList( vector<tstring>& vecList, vector<int>& vecRefList )
{
	for(vector<tstring>::iterator it = vecList.begin(); it != vecList.end(); ++it)
	{
		int nID = _tstoi(it->c_str());
		vecRefList.push_back(nID);
	}
}

void XBuffInfoMgr::RecursiveRefenceBuffEffectInfo( XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo )
{
	if(pRefBuffInfo == NULL)
		return;

	for(vector<int>::iterator itRefID = pRefBuffInfo->m_BuffEffect.m_vecRefID.begin(); itRefID != pRefBuffInfo->m_BuffEffect.m_vecRefID.end(); ++itRefID)
	{
		map<int , CSBuffInfo* > ::iterator itFind = find(*itRefID);
		if(itFind != end())
		{
			XBuffInfo* pRefNextBuffInfo = static_cast<XBuffInfo*>(itFind->second);
			RecursiveRefenceBuffEffectInfo(pBuffInfo, pRefNextBuffInfo);

			SetRefenceBuffEffectInfo(pBuffInfo, pRefNextBuffInfo);
		}
	}
}

void XBuffInfoMgr::RecursiveRefenceEnchantBuffEffectInfo( XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo )
{
	if(pRefBuffInfo == NULL)
		return;

	for(vector<int>::iterator itRefID = pRefBuffInfo->m_EnchantBuffEffect.m_vecRefID.begin(); itRefID != pRefBuffInfo->m_EnchantBuffEffect.m_vecRefID.end(); ++itRefID)
	{
		map<int , CSBuffInfo* > ::iterator itFind = find(*itRefID);
		if(itFind != end())
		{
			XBuffInfo* pRefNextBuffInfo = static_cast<XBuffInfo*>(itFind->second);

			RecursiveRefenceEnchantBuffEffectInfo(pBuffInfo, pRefNextBuffInfo);

			SetRefenceEnchantBuffEffectInfo(pBuffInfo, pRefNextBuffInfo);
		}
	}
}

void XBuffInfoMgr::SetRefenceBuffEffectInfo( XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo )
{
	for(vector<XBuffEffectInfo *>::iterator itBuffEffectInfo = pRefBuffInfo->m_BuffEffect.begin(); itBuffEffectInfo != pRefBuffInfo->m_BuffEffect.end(); ++itBuffEffectInfo)
	{
		XBuffEffectInfo *pCopyInfo = new XBuffEffectInfo;

		*pCopyInfo = *(*itBuffEffectInfo);

		pBuffInfo->m_BuffEffect.push_back(pCopyInfo);
	}
}

void XBuffInfoMgr::SetRefenceEnchantBuffEffectInfo( XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo )
{
	pBuffInfo->m_EnchantBuffEffect.m_DefaultEffect += pRefBuffInfo->m_EnchantBuffEffect.m_DefaultEffect;

	for(vector<XEnchantBuffEffectEquipType *>::iterator itEnchantBuffEffectInfo = pRefBuffInfo->m_EnchantBuffEffect.begin(); itEnchantBuffEffectInfo != pRefBuffInfo->m_EnchantBuffEffect.end(); ++itEnchantBuffEffectInfo)
	{
		XEnchantBuffEffectEquipType *pCopyInfo = new XEnchantBuffEffectEquipType;

		*pCopyInfo = *(*itEnchantBuffEffectInfo);

		pBuffInfo->m_EnchantBuffEffect.push_back(pCopyInfo);
	}
}

//////////////////////////////////////////////////////////////////////////
// Buff Effect

//------------------------------------------------------------------------
// XBuffEffectData
int XBuffEffectData::GetEffectInfoList( BUFF_EFFECT_TYPE nType, vector<XBuffEffectInfo *> &BuffEffectInfo )
{
	int nInfoCount = 0;

	vector<XBuffEffectInfo *>::iterator itInfo = begin();
	while(itInfo != end())
	{
		if((*itInfo)->m_nBuffEffectType == nType)
		{
			BuffEffectInfo.push_back(*itInfo);
			nInfoCount++;
		}

		itInfo++;
	}

	return nInfoCount;
}

//////////////////////////////////////////////////////////////////////////
// Enchant Buff Effect

//------------------------------------------------------------------------
// XEnchantBuffEffectData
int XEnchantBuffEffectData::GetEffectInfoList( ENCHANT_BUFF_EFFECT_TYPE nType, WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType, vector<XEnchantBuffEffectInfo *> &BuffEffectInfo )
{
	for(vector<XEnchantBuffEffectEquipType *>::iterator itInfo = begin(); itInfo != end(); ++itInfo)
	{
		int nInfoCount = (*itInfo)->GetEffectInfoList(nType, eRType, eLType, nPartsSlotType, BuffEffectInfo);
		if(nInfoCount != -1)
		{
			return nInfoCount;
		}
	}

	return m_DefaultEffect.GetEffectInfoList(nType, eRType, eLType, nPartsSlotType, BuffEffectInfo);
}

tstring XEnchantBuffEffectData::GetAllDummyEffectName()
{
	//for(vector<XEnchantBuffEffectInfo *>::iterator itInfo = begin(); itInfo != end(); ++itInfo)
	//{
	//	if((*itInfo)->m_strBoneName == _T(""))
	//	{
	//		return (*itInfo)->m_strEffectName;
	//	}
	//}

	return _T("");
}

//------------------------------------------------------------------------
// XEnchantBuffEffectEquipType
int XEnchantBuffEffectEquipType::GetEffectInfoList( ENCHANT_BUFF_EFFECT_TYPE nType, WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType, vector<XEnchantBuffEffectInfo *> &BuffEffectInfo )
{
	ENCHANT_BUFF_EFFECT_EQUIP_TYPE eEquipType = Slot2EquipType(eRType, eLType, nPartsSlotType);

	if(m_eEquipType != EBEET_NONE && m_eEquipType != eEquipType)
		return -1;

	int nInfoCount = 0;

	for(vector<XEnchantBuffEffectInfo *>::iterator itInfo = begin(); itInfo != end(); ++itInfo)
	{
		if((*itInfo)->m_nEnchantBuffEffectType == nType)
		{
			BuffEffectInfo.push_back(*itInfo);
			nInfoCount++;
		}
	}

	return nInfoCount;
}

ENCHANT_BUFF_EFFECT_EQUIP_TYPE XEnchantBuffEffectEquipType::Slot2EquipType( WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType )
{
	switch(nPartsSlotType)
	{
	case PLAYER_PARTS_SLOT_HEAD	: return EBEET_HEAD;
	case PLAYER_PARTS_SLOT_HANDS: return EBEET_HANDS;
	case PLAYER_PARTS_SLOT_BODY	: return EBEET_BODY;
	case PLAYER_PARTS_SLOT_LEG	: return EBEET_LEG;
	case PLAYER_PARTS_SLOT_FEET	: return EBEET_FEET;

	case PLAYER_PARTS_SLOT_RWEAPON:	return WeaponType2EquipType(eRType);
	case PLAYER_PARTS_SLOT_LWEAPON:	return WeaponType2EquipType(eLType);
		break;
	}

	return EBEET_NONE;
}

ENCHANT_BUFF_EFFECT_EQUIP_TYPE XEnchantBuffEffectEquipType::WeaponType2EquipType( WEAPON_TYPE nWeapoinType )
{
	switch(nWeapoinType)
	{
	case WEAPON_1H_SLASH:	return EBEET_1HS;
	case WEAPON_1H_BLUNT:	return EBEET_1HB;
	case WEAPON_1H_PIERCE:	return EBEET_1HP;
	case WEAPON_TWO_HANDED:	return EBEET_2HD;
	case WEAPON_STAFF:		return EBEET_STA;
	case WEAPON_ARCHERY:	return EBEET_ARC;
	case WEAPON_2H_BLUNT:	return EBEET_2HB;
	case WEAPON_DUAL_WIELD:	return EBEET_DWD;
	case WEAPON_DUAL_PIERCE:	return EBEET_DWP;
	}

	return EBEET_NONE;
}
