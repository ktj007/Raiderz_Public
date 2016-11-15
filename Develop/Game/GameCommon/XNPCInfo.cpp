#include "stdafx.h"
#include "XNPCInfo.h"
#include "XStrings.h"
#include "CSMeshInfo.h"
#include "MLocale.h"
#include "CSFormatString.h"

XNPCInfoMgr::XNPCInfoMgr()
{


}

XNPCInfoMgr::~XNPCInfoMgr()
{
	Clear();
}

void XNPCInfo::Clear()
{
	for (vector<XNPCInfo*>::iterator itor = vecModes.begin();
		itor != vecModes.end(); ++itor)
	{
		XNPCInfo* pInfo = (*itor);
		SAFE_DELETE(pInfo);
	}

	vecModes.clear();

	for (vector<XNPCMtrl*>::iterator itor = vecMtrls.begin();
		itor != vecMtrls.end(); ++itor)
	{
		XNPCMtrl* pInfo = (*itor);
		SAFE_DELETE(pInfo);
	}

	vecMtrls.clear();


	for (int i = 0; i < NPC_PARTS_SLOT_MAX; i++) nPartsItemID[i] = 0;

	CSNPCInfo::Clear();
}

bool XNPCInfo::IsRotationTalent( int nTalentID )
{
	if(nTalentID == nLRotationTalentID ||
		nTalentID == nRRotationTalentID)
		return true;

	return false;
}

const TCHAR* XNPCInfo::GetName(void)
{
	return XStringResMgr::GetInstance()->GetString(strName);
}

const TCHAR* XNPCInfo::GetClan(void)
{
	return XStringResMgr::GetInstance()->GetString(strClan);
}

void XNPCInfo::Cooking(CSMeshInfoMgr* pMeshInfoMgr)
{
	tstring nn = strMeshName.substr(strMeshName.rfind(_T("/")) + 1, strMeshName.rfind(_T(".")) - strMeshName.rfind(_T("/")) - 1);
	this->pMeshInfo = NULL;
	
	if (pMeshInfoMgr) this->pMeshInfo = pMeshInfoMgr->GetInfo(nn);
}

bool XNPCInfo::IsShowIndicator()
{
	return (m_bShowIndicator && nNpcType == NPC_TYPE_MONSTER);
}

XNPCInfo* XNPCInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (XNPCInfo*)((*itor).second);
}

bool XNPCInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool XNPCInfoMgr::Load(const TCHAR* szFileName, const TCHAR* szFileName_Sub, CSMeshInfoMgr* pMeshInfoMgr)
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), NPC_XML_TAG_NPC))
		{
			ParseNPC(pElement, &xml);
		}
	}

	MXml xml2;

	// NPC_submtrl
	if( xml2.LoadFile(MLocale::ConvTCHARToAnsi(szFileName_Sub).c_str())) 
	{
		docHandle = xml2.DocHandle();
		pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp(pElement->Value(), NPC_XML_TAG_NPC))
			{
				ParseNPC_SubMtrl(pElement, &xml2);
			}
		}
	}

	Cooking(pMeshInfoMgr);

	return true;
}


void XNPCInfoMgr::ParseNPC(MXmlElement* pElement, MXml* pXml)
{
	tstring strName;
	tstring strValue;

	int nValue = 0;

	XNPCInfo* pParentNPCInfo = NULL;
	XNPCInfo* pNewInfo = new XNPCInfo();

	_Attribute(&pNewInfo->nID,						pElement, NPC_XML_ATTR_ID);

	pParentNPCInfo = Get(pNewInfo->nID);

	_Attribute(&pNewInfo->nMaxHP,					pElement, NPC_XML_ATTR_MAX_HP);
	_Attribute(&pNewInfo->nAP,						pElement, NPC_XML_ATTR_AP);
	_Attribute(pNewInfo->strMeshName,				pElement, NPC_XML_ATTR_MESH_NAME, pXml);
	_Attribute(pNewInfo->strMeshPath,				pElement, NPC_XML_ATTR_MESH_PATH, pXml);
	if (pNewInfo->strMeshPath.length() > 0)
	{
		if (pNewInfo->strMeshPath[pNewInfo->strMeshPath.length()-1] != _T('/') &&
			pNewInfo->strMeshPath[pNewInfo->strMeshPath.length()-1] != _T('\\'))
		{
			pNewInfo->strMeshPath += _T("/");
		}
	}

	if (_Attribute(strValue,						pElement, NPC_XML_ATTR_TYPE))
	{
		for (int i = 0; i < NPC_TYPE_MAX; i++)
		{
			if (!_tcsicmp(strValue.c_str(), NPC_TYPE_STR[i]))
			{
				pNewInfo->nNpcType = NPC_TYPE(i);
				break;
			}
		}
	}

	if (_Attribute(strValue, pElement, NPC_XML_ATTR_GATHER_TYPE))
	{
		pNewInfo->nGatherType = (GATHER_TYPE)CSStrings::StringToGatherType(strValue.c_str());
	}

	_Attribute(&pNewInfo->bShowEnemyInfo, pElement, NPC_XML_ATTR_SHOW_ENEMYINFO, true);

	if (_Attribute(strValue,								pElement, NPC_XML_ATTR_ARMOR_TYPE))
	{
		for (int i = 0; i < ARMOR_ATT_MAX; i++)
		{
			if (!_tcsicmp(strValue.c_str(), ARMOR_ATTRIB_STR[i]))
			{
				pNewInfo->nArmorType = ARMOR_ATTRIB(i);
				break;
			}
		}
	}

	int nGrade = DEFAULT_NPC_GRADE;
	_Attribute(&nGrade,								pElement, NPC_XML_ATTR_GRADE);
	pNewInfo->nGrade = nGrade; 
	
	_Attribute(&pNewInfo->bHuge,					pElement, NPC_XML_ATTR_HUGE);
	_Attribute(&pNewInfo->fScale,					pElement, NPC_XML_ATTR_SCALE);
	_Attribute(&pNewInfo->bInteract,				pElement, NPC_XML_ATTR_INTERACT);
	_Attribute(&pNewInfo->bShowName,				pElement, NPC_XML_ATTR_SHOW_NAME);
	_Attribute(&pNewInfo->bAlignTerrain,			pElement, NPC_XML_ATTR_ALIGN_TERRAIN);
	_Attribute(&pNewInfo->bAlignEnemy,				pElement, NPC_XML_ATTR_ALIGN_ENEMY);
	_Attribute(pNewInfo->strTitleBone,				pElement, NPC_XML_ATTR_TITLE_BONE, pXml);
	_Attribute(&pNewInfo->fTitleHeight,				pElement, NPC_XML_ATTR_TITLE_HEIGHT);


	_Attribute(&pNewInfo->bUseDieAni,				pElement, NPC_XML_ATTR_USE_DIE_ANIMATION);
	_Attribute(&pNewInfo->bShowHpBar,				pElement, NPC_XML_ATTR_SHOW_HP_BAR);

	tstring strRarity;
	_Attribute( strRarity, pElement, NPC_XML_ATTR_RARITY);
	if ( strRarity.empty() == false)
	{
		if ( strRarity == _T("common"))					pNewInfo->nRarity = NPC_RARITY_COMMON;
		else if ( strRarity == _T("rare"))				pNewInfo->nRarity = NPC_RARITY_RARE;
		else if ( strRarity == _T("epic"))				pNewInfo->nRarity = NPC_RARITY_EPIC;
	}

	_Attribute(pNewInfo->strAniPrefix,				pElement, NPC_XML_ATTR_ANI_PREFIX, pXml);

	if (_Attribute(&nValue,					pElement, NPC_XML_ATTR_MODE))
	{
		bool bExistMode = false;
		for (int i = 0; i < NPC_MODE_MAX; i++)
		{
			if (NPC_MODE(i) == nValue)
			{
				bExistMode = true;
				pNewInfo->nMode = NPC_MODE(i);
			}
		}
		_ASSERT(bExistMode == true);
	}


	tstring preFix;
	if (0 < pNewInfo->nMode)
	{
		preFix = CSFormatString::Format(_T("_{0}"), FSParam(pNewInfo->nMode));
	}

	pNewInfo->strName = CSFormatString::Format(_T("NPC_NAME_{0}{1}"), FSParam(pNewInfo->nID, preFix));	// 문자열테이블 키
	pNewInfo->strClan = CSFormatString::Format(_T("NPC_CLAN_{0}{1}"), FSParam(pNewInfo->nID, preFix));	// 문자열테이블 키



	if (true == _Attribute(strValue,		pElement, NPC_XML_ATTR_AA, pXml))
	{
		pNewInfo->nAA = static_cast<NPC_AA_TYPE>(CSStrings::StringToAAType(strValue.c_str()));
	}

	float fRotateSpeed = 0.0f;
	_Attribute(&fRotateSpeed, pElement, NPC_XML_ATTR_ROTATE_SPEED);
	fRotateSpeed = (fRotateSpeed < 0.8f) ? 0.8f : fRotateSpeed;
	pNewInfo->fRotateSpeed = 360.0f / fRotateSpeed;

	_Attribute(&pNewInfo->m_bInteractionRotate, pElement, NPC_XML_ATTR_INTERACTIONROTATE, true);

	if (_Attribute(&nValue, pElement, NPC_XML_ATTR_HITTEST_INDEX))
	{
		pNewInfo->nHitTestIndex = (uint8)nValue;
	}

	unsigned int texColor = GetColorAttribute(pElement, NPC_XML_ATTR_TEX_COLOR);
	if (texColor > 0)
	{
		pNewInfo->m_bUseTexColor = true;
		pNewInfo->nTexColor = texColor;
	}

	unsigned int skinColor = GetColorAttribute(pElement, NPC_XML_ATTR_SKIN_COLOR);
	if (skinColor > 0)
	{
		pNewInfo->nSkinColor = skinColor;
	}
	unsigned int hairColor = GetColorAttribute(pElement, NPC_XML_ATTR_HAIR_COLOR);
	if (hairColor > 0)
	{
		pNewInfo->nHairColor = hairColor;
	}

	_Attribute(pNewInfo->strTexSkin,				pElement, NPC_XML_ATTR_TEX_SKIN, pXml);
	_Attribute(pNewInfo->strBloodEffectName,		pElement, NPC_XML_ATTR_BLOOD_EFFECT_NAME, pXml);
	_Attribute(pNewInfo->strInteractionLootAni,		pElement, NPC_XML_ATTR_INTERACTION_LOOT_ANI, pXml);
	_Attribute(&pNewInfo->fInteractionLootTime,		pElement, NPC_XML_ATTR_INTERACTION_LOOT_TIME);
	_Attribute(&pNewInfo->nInteractionDistance,		pElement, NPC_XML_ATTR_INTERACTION_DISTANCE);
	_Attribute(&pNewInfo->bDynamicShadow,			pElement, NPC_XML_ATTR_DYNAMIC_SHADOW);

	_Attribute(pNewInfo->SoundInfo.HitSound1,		pElement, NPC_XML_ATTR_HITSOUND1);
	_Attribute(pNewInfo->SoundInfo.HitSound2,		pElement, NPC_XML_ATTR_HITSOUND2);
	_Attribute(pNewInfo->SoundInfo.HitSound3,		pElement, NPC_XML_ATTR_HITSOUND3);	
	_Attribute(pNewInfo->SoundInfo.DyingSound,		pElement, NPC_XML_ATTR_DYINGSOUND);	
	_Attribute(pNewInfo->SoundInfo.EntitySound,		pElement, NPC_XML_ATTR_ENTITYSOUND);	
	_Attribute(pNewInfo->SoundInfo.CombatBGM,		pElement, NPC_XML_ATTR_COMBATBGM);	
	_Attribute(pNewInfo->SoundInfo.Aggro,			pElement, NPC_XML_ATTR_AGGROSOUND);	

	_Attribute(&pNewInfo->bPartsMature[0],			pElement, NPC_XML_ATTR_BPARTS_MATURE01, false);
	_Attribute(&pNewInfo->bPartsMature[1],			pElement, NPC_XML_ATTR_BPARTS_MATURE02, false);
	_Attribute(&pNewInfo->bPartsMature[2],			pElement, NPC_XML_ATTR_BPARTS_MATURE03, false);
	_Attribute(&pNewInfo->bPartsMature[3],			pElement, NPC_XML_ATTR_BPARTS_MATURE04, false);

	_Attribute(&pNewInfo->m_bColTestObject,			pElement, NPC_XML_ATTR_COL_TEST_OBJECT, true);

	_Attribute(&pNewInfo->m_nFactionLossID,			pElement, NPC_XML_ATTR_FACTION_LOSS_ID);

	_Attribute(&pNewInfo->m_bShowTargetInfo,		pElement, NPC_XML_ATTR_SHOW_TARGEINFO);

	_Attribute(&pNewInfo->nLRotationTalentID,		pElement, NPC_XML_ATTR_L_ROTATION_ID);
	_Attribute(&pNewInfo->nRRotationTalentID,		pElement, NPC_XML_ATTR_R_ROTATION_ID);

	if(_Attribute( strValue,						pElement, NPC_XML_ATTR_VOICE_TYPE))
	{
		pNewInfo->eVoiceType = static_cast<INTERACTION_VOICE_TYPE>(XStrings::StringToVoiceType(strValue.c_str()));
	}

	if(_Attribute( strValue,						pElement, "IType1", "none"))
	{
		pNewInfo->eIT = static_cast<INTERACTION_TYPE>(XStrings::StringToIElementType(strValue.c_str()));
	}

	if (_Attribute(strValue,		pElement, NPC_XML_ATTR_ATTACKABLE, pXml))
	{
		pNewInfo->nInitAttackable = static_cast<NPC_ATTACKABLE_TYPE>(CSStrings::StringToAttackableType(strValue.c_str()));
	}	

	_Attribute(pNewInfo->strHitBPartsEffectName[0],	pElement, NPC_XML_ATTR_BPARTS_HIT_EFFECT_1);
	_Attribute(pNewInfo->strHitBPartsEffectName[1],	pElement, NPC_XML_ATTR_BPARTS_HIT_EFFECT_2);
	_Attribute(pNewInfo->strHitBPartsEffectName[2],	pElement, NPC_XML_ATTR_BPARTS_HIT_EFFECT_3);
	_Attribute(pNewInfo->strHitBPartsEffectName[3],	pElement, NPC_XML_ATTR_BPARTS_HIT_EFFECT_4);

	_Attribute(pNewInfo->strIdleAni,				pElement, NPC_XML_ATTR_NPC_IDLE_ANIMATION_NAME);

	_Attribute(pNewInfo->bShowAggroInfo,			pElement, NPC_XML_ATTR_SHOW_AGGRO_INFO);
	_Attribute(pNewInfo->m_bShowIndicator,			pElement, NPC_XML_ATTR_SHOW_INDICATOR);

	_Attribute(&pNewInfo->nIItemID,					pElement, NPC_XML_ATTR_IITEMID);

	ParseNPC_Parts(pNewInfo, pElement, pXml);

	if (pParentNPCInfo == NULL) 
	{
		insert(value_type(pNewInfo->nID, pNewInfo));
	}
	else
	{
		size_t nModeSize = pParentNPCInfo->vecModes.size();
		if (pNewInfo->nMode-1 == nModeSize && pNewInfo->nID == pParentNPCInfo->nID)
		{
			pParentNPCInfo->vecModes.push_back(pNewInfo);
		}
		else
		{
			delete pNewInfo;
		}
	}
}

void XNPCInfoMgr::ParseNPC_SubMtrl(MXmlElement* pElement, MXml* pXml)
{
	tstring strName;
	tstring strValue;

	XNPCInfo* pParentNPCInfo = NULL;
	int nNPCID;
	int nMtrlID;

	_Attribute(&nNPCID,	pElement, NPC_XML_ATTR_ID);
	pParentNPCInfo = Get(nNPCID);
	if (pParentNPCInfo == NULL) return;

	// <NPC id="21" Name="코볼드 샘플" Model="Monster" MeshName="kobold_pawn">
	MXmlElement* pMeshElement = pElement->FirstChildElement();
	for( pMeshElement; pMeshElement != NULL; pMeshElement=pMeshElement->NextSiblingElement() )
	{
		// <MESH index="0" SubCount="1">
		XNPCMtrl* newMtrl = new XNPCMtrl;
		_Attribute(&newMtrl->nID, pMeshElement, "index");
		_Attribute(&newMtrl->nSubCount, pMeshElement, "SubCount");

		MXmlElement* pMtrlElement = pMeshElement->FirstChildElement();
		for( pMtrlElement; pMtrlElement != NULL; pMtrlElement=pMtrlElement->NextSiblingElement() )
		{
			// <MATERIAL SubID="0" MtrlID="0" />
			_Attribute(&nMtrlID, pMtrlElement, "MtrlID");
			newMtrl->vecSubMtrls.push_back(nMtrlID);
		}

		pParentNPCInfo->vecMtrls.push_back(newMtrl);	
	}
}

void XNPCInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		XNPCInfo* pInfo = (XNPCInfo*)((*itor).second);
		pInfo->Clear();
		delete pInfo;
	}
	clear();
}

void XNPCInfoMgr::ParseNPC_Parts(XNPCInfo* pNPCInfo, MXmlElement* pElement, MXml* pXml )
{
	//tstring strPath;
	//if (_Attribute(strPath, pElement, NPC_XML_ATTR_MESH_PARTS_PATH))
	//{
	//	if (!strPath.empty())
	//	{
	//		if (strPath[strPath.length()-1] != '/' || strPath[strPath.length()-1] != '\\')
	//			strPath = strPath + '/';

	//		strPath = PATH_MODEL + strPath;

	//		pNPCInfo->strPartsPath = strPath;
	//	}
	//}

	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_HAIR],		pElement, NPC_XML_ATTR_PARTS_HAIR);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_FACE],		pElement, NPC_XML_ATTR_PARTS_FACE);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_HEAD],		pElement, NPC_XML_ATTR_PARTS_HEAD);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_CHEST],		pElement, NPC_XML_ATTR_PARTS_CHEST);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LEG],		pElement, NPC_XML_ATTR_PARTS_LEG);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_HANDS],		pElement, NPC_XML_ATTR_PARTS_HANDS);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_FEET],		pElement, NPC_XML_ATTR_PARTS_FEET);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON],		pElement, NPC_XML_ATTR_PARTS_WEAPON_L);
	_Attribute(&pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON],		pElement, NPC_XML_ATTR_PARTS_WEAPON_R);
	_ASSERT(NPC_PARTS_SLOT_MAX > 8);

	for (int i = 0; i < NPC_PARTS_SLOT_MAX; i ++)
	{
		if (pNPCInfo->nPartsItemID[i] > 0) 
		{
			pNPCInfo->m_bUseMeshParts = true;
			break;
		}
	}
}

unsigned int XNPCInfoMgr::GetColorAttribute( MXmlElement* pElement, const char* szAttrName )
{
	unsigned int nRet = 0;

	char buff[512];
	if (_Attribute(buff, pElement, szAttrName))
	{
		int color = strtol(buff, NULL, 16);		// XML에 16진수로 저장되어있다.
		if (color >= 0)
		{
			nRet = 0xFF000000 | color;
		}
	}
	return nRet;
}

bool XNPCInfoMgr::Reload( const TCHAR* szFileName, const TCHAR* szFileName_Sub, CSMeshInfoMgr* pMeshInfoMgr )
{
	Clear();

	return Load(szFileName, szFileName_Sub, pMeshInfoMgr);
}

void XNPCInfoMgr::Cooking(CSMeshInfoMgr* pMeshInfoMgr)
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		XNPCInfo* pInfo = (XNPCInfo*)((*itor).second);
		pInfo->Cooking(pMeshInfoMgr);
	}

}

const TCHAR* XNPCInfoMgr::GetName( int nID )
{
	XNPCInfo* pInfo = Get(nID);
	if (NULL == pInfo) return NULL;

	return pInfo->GetName();
}

int XNPCInfoMgr::GetID( const TCHAR* pName )
{
	if (NULL == pName) return 0;

	for each (const map<int, CSNPCInfo*>::value_type& val in *this)
	{
		XNPCInfo* pNPCInfo = (XNPCInfo*)val.second;
		const TCHAR* token = _tcsstr(pNPCInfo->GetName(), pName);
		int result = (int)(token - pNPCInfo->GetName());
		if (token != NULL && result >= 0)
		{
			token = _tcsstr(pNPCInfo->GetName(), _T("샘플"));
			if (token == NULL)
			{
				return pNPCInfo->nID;
			}
		}
	}

	return 0;
}
