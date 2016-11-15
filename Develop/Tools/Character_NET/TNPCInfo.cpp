#include "stdafx.h"
#include "TNPCInfo.h"

#include "RMaterialResource.h"

TNPCInfoMgr::TNPCInfoMgr()
{


}

TNPCInfoMgr::~TNPCInfoMgr()
{
	Clear();
}

void TNPCInfo::Clear()
{
	for (vector<TNPCInfo*>::iterator itor = vecModes.begin();
		itor != vecModes.end(); ++itor)
	{
		TNPCInfo* pInfo = (*itor);
		SAFE_DELETE(pInfo);
	}

	vecModes.clear();

	for (vector<TNPCMtrl*>::iterator itor = vecMtrls.begin();
		itor != vecMtrls.end(); ++itor)
	{
		TNPCMtrl* pInfo = (*itor);
		SAFE_DELETE(pInfo);
	}

	vecMtrls.clear();


	for (int i = 0; i < MESH_PARTS_COUNT; i++) nPartsItemID[i] = 0;

	CSNPCInfo::Clear();
}

TNPCInfoMgr& TNPCInfoMgr::GetInstance()
{
	static TNPCInfoMgr m_Instance;
	return m_Instance;
}

TNPCInfo* TNPCInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (TNPCInfo*)((*itor).second);
}

bool TNPCInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool TNPCInfoMgr::Load()
{
	string filename = FILENAME_NPC_TOOL;
	string filename_sub = FILENAME_NPC_CHANGEMTRL_TOOL;

	MXml xml;

	if (!xml.LoadFile(filename.c_str())) 
	{
		if (System::Windows::Forms::MessageBox::Show("npc.xml 파일이 이상합니다.", "파일 이상."
			,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
		{
		}

		return false;
	}

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
	if( !xml2.LoadFile(filename_sub.c_str())) 
		return true;//false;

	docHandle = xml2.DocHandle();
	pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), NPC_XML_TAG_NPC))
		{
			ParseNPC_SubMtrl(pElement, &xml2);
		}
	}

	return true;
}


void TNPCInfoMgr::ParseNPC(MXmlElement* pElement, MXml* pXml)
{
	string strName;
	string strValue;

	int nValue = 0;

	TNPCInfo* pParentNPCInfo = NULL;
	TNPCInfo* pNewInfo = new TNPCInfo();

	_Attribute(&pNewInfo->nID,						pElement, NPC_XML_ATTR_ID);

	pParentNPCInfo = Get(pNewInfo->nID);

	_Attribute(pNewInfo->strName,					pElement, NPC_XML_ATTR_NAME, pXml);
	_Attribute(&pNewInfo->nMaxHP,					pElement, NPC_XML_ATTR_MAX_HP);
	_Attribute(&pNewInfo->nAP,						pElement, NPC_XML_ATTR_AP);
	_Attribute(pNewInfo->strMeshName,				pElement, NPC_XML_ATTR_MESH_NAME);
	_Attribute(pNewInfo->strMeshPath,				pElement, NPC_XML_ATTR_MESH_PATH);
	if (pNewInfo->strMeshPath.length() > 0)
	{
		if (pNewInfo->strMeshPath[pNewInfo->strMeshPath.length()-1] != '/' &&
			pNewInfo->strMeshPath[pNewInfo->strMeshPath.length()-1] != '\\')
		{
			pNewInfo->strMeshPath += "/";
		}
	}

	if (_Attribute(strValue,								pElement, NPC_XML_ATTR_TYPE))
	{
		for (int i = 0; i < NPC_TYPE_MAX; i++)
		{
			if (!stricmp(strValue.c_str(), NPC_TYPE_STR[i]))
			{
				pNewInfo->nNpcType = NPC_TYPE(i);
				break;
			}
		}
	}


	_Attribute(&pNewInfo->bHuge,					pElement, NPC_XML_ATTR_HUGE);
	_Attribute(&pNewInfo->fScale,					pElement, NPC_XML_ATTR_SCALE);

	int nMode = 0;
	_Attribute(&nMode,					pElement, NPC_XML_ATTR_MODE);
	pNewInfo->nMode = (NPC_MODE)nMode;

	_Attribute(&pNewInfo->bInteract,				pElement, NPC_XML_ATTR_INTERACT);
	_Attribute(&pNewInfo->bShowName,				pElement, NPC_XML_ATTR_SHOW_NAME);
	_Attribute(&pNewInfo->bAlignTerrain,			pElement, NPC_XML_ATTR_ALIGN_TERRAIN);
	_Attribute(pNewInfo->strTitleBone,				pElement, NPC_XML_ATTR_TITLE_BONE);
	_Attribute(&pNewInfo->fTitleHeight,				pElement, NPC_XML_ATTR_TITLE_HEIGHT);
	_Attribute(pNewInfo->strClan,					pElement, NPC_XML_ATTR_CLAN);

	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_HITTEST_INDEX))
	{
		pNewInfo->nHitTestIndex = (uint8)nValue;
	}

	unsigned texColor = GetColorAttribute(pElement, NPC_XML_ATTR_TEX_COLOR);
	if (texColor > 0)
	{
		pNewInfo->m_bUseTexColor = true;
		pNewInfo->nTexColor = texColor;
	}

	unsigned skinColor = GetColorAttribute(pElement, NPC_XML_ATTR_SKIN_COLOR);
	if (skinColor > 0)
	{
		pNewInfo->nSkinColor = skinColor;
	}
	unsigned hairColor = GetColorAttribute(pElement, NPC_XML_ATTR_HAIR_COLOR);
	if (hairColor > 0)
	{
		pNewInfo->nHairColor = hairColor;
	}

	_Attribute(pNewInfo->strTexSkin,				pElement, NPC_XML_ATTR_TEX_SKIN);
	_Attribute(pNewInfo->strBloodEffectName,		pElement, NPC_XML_ATTR_BLOOD_EFFECT_NAME);
	_Attribute(pNewInfo->strInteractionLootAni,		pElement, NPC_XML_ATTR_INTERACTION_LOOT_ANI);
	_Attribute(&pNewInfo->fInteractionLootTime,	pElement, NPC_XML_ATTR_INTERACTION_LOOT_TIME);

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

void TNPCInfoMgr::ParseNPC_SubMtrl(MXmlElement* pElement, MXml* pXml)
{
	string strName;
	string strValue;

	TNPCInfo* pParentNPCInfo = NULL;
	int nNPCID;
	string strMtrlName;

	_Attribute(&nNPCID,	pElement, NPC_XML_ATTR_ID);
	pParentNPCInfo = Get(nNPCID);

	// <NPC id="21" Name="코볼드 샘플" Model="Monster" MeshName="kobold_pawn">
	MXmlElement* pMeshElement = pElement->FirstChildElement();
	for( pMeshElement; pMeshElement != NULL; pMeshElement=pMeshElement->NextSiblingElement() )
	{
		// <MESH index="0" SubCount="1">
		TNPCMtrl* newMtrl = new TNPCMtrl;
		_Attribute(&newMtrl->nID, pMeshElement, "index");
		_Attribute(&newMtrl->nSubCount, pMeshElement, "SubCount");

		MXmlElement* pMtrlElement = pMeshElement->FirstChildElement();
		for( pMtrlElement; pMtrlElement != NULL; pMtrlElement=pMtrlElement->NextSiblingElement() )
		{
			_Attribute(strMtrlName, pMtrlElement, "MtrlName");
			newMtrl->vecSubMtrls.push_back(strMtrlName);
		}

		pParentNPCInfo->vecMtrls.push_back(newMtrl);	
	}
}

void TNPCInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		TNPCInfo* pInfo = (TNPCInfo*)((*itor).second);
		pInfo->Clear();
		delete pInfo;
	}
	clear();
}

void TNPCInfoMgr::ParseNPC_Parts(TNPCInfo* pNPCInfo, MXmlElement* pElement, MXml* pXml )
{
	//string strPath;
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

	_Attribute(&pNPCInfo->nPartsItemID[0],		pElement, NPC_XML_ATTR_PARTS_HAIR);
	_Attribute(&pNPCInfo->nPartsItemID[1],		pElement, NPC_XML_ATTR_PARTS_FACE);
	_Attribute(&pNPCInfo->nPartsItemID[2],		pElement, NPC_XML_ATTR_PARTS_HEAD);
	_Attribute(&pNPCInfo->nPartsItemID[3],		pElement, NPC_XML_ATTR_PARTS_CHEST);
	_Attribute(&pNPCInfo->nPartsItemID[4],		pElement, NPC_XML_ATTR_PARTS_LEG);
	_Attribute(&pNPCInfo->nPartsItemID[5],		pElement, NPC_XML_ATTR_PARTS_HANDS);
	_Attribute(&pNPCInfo->nPartsItemID[6],		pElement, NPC_XML_ATTR_PARTS_FEET);
	_Attribute(&pNPCInfo->nPartsItemID[7],		pElement, NPC_XML_ATTR_PARTS_WEAPON_R);
	_Attribute(&pNPCInfo->nPartsItemID[8],		pElement, NPC_XML_ATTR_PARTS_WEAPON_L);

	for (int i = 0; i < TNPCInfo::MESH_PARTS_COUNT; i ++)
	{
		if (pNPCInfo->nPartsItemID[i] > 0) 
		{
			pNPCInfo->m_bUseMeshParts = true;
			break;
		}
	}
}

void TNPCInfoMgr::Save_SubMtrl(const char* filename)
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
	root->LinkEndChild(pDec);

	MXmlElement *pRootElement = new MXmlElement(NPC_XML_TAG_MAIET);

	for (iterator itor = begin();
		itor != end(); itor++)
	{
		//CSNPCInfo* pItemData = (*itor).second;
		//if (pItemData == NULL) continue;
		//if (pItemData->m_vecMtrls.size() == 0) continue;

		//MXmlElement *pItemElement = new MXmlElement(NPC_XML_TAG_ITEM);
		//AddSubMtrl_Item(pItemElement, pItemData);
		//pRootElement->LinkEndChild(pItemElement);
	}

	root->LinkEndChild(pRootElement);

	xml.SaveFile(filename);
}

bool TNPCInfoMgr::AddSubMtrl_NPC(MXmlElement* pNPCElement, int nNPCID)
{
	//	if(pNPCInfo->bIsSubMtrl == false)	// 재질 변경 내역이 없으면 패스
	//		continue;

	TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(nNPCID);
	if (pNPCInfo == NULL)	return false;

	_SetAttribute(pNPCElement,NPC_XML_ATTR_ID, pNPCInfo->nID);
	_SetAttribute(pNPCElement,NPC_XML_ATTR_NAME, pNPCInfo->strName.c_str());
	//	_SetAttribute(pNPCElement,NPC_XML_ATTR_MODEL, pNPCInfo->strModelFolder.c_str());
	_SetAttribute(pNPCElement,NPC_XML_ATTR_MESH_NAME, pNPCInfo->strMeshName.c_str());	// .elu file name

	RActor pActor;
	if(pActor.Create(pNPCInfo->strMeshName.c_str()) == false)
	{
		return false;
	}
	//	MXmlElement *pMeshListElement = new MXmlElement("MESHLIST");
	//	pNPCElement->LinkEndChild(pMeshListElement);
	//	_SetAttribute(pMeshListElement,"Count", m_pActor->GetActorNodeCount());
	for( int i = 0; i < pActor.GetActorNodeCount(); ++i )
	{
		RActorNode *pActorNode = pActor.GetActorNodes()[i];
		// Dummy나 Bone은 출력하지 않는다.
		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
			continue;

		MXmlElement *pMeshElement = new MXmlElement("MESH");
		pNPCElement->LinkEndChild(pMeshElement);
		_SetAttribute(pMeshElement, "index", i);

		// <Sub>
		_SetAttribute(pMeshElement, "SubCount", pActorNode->m_pMeshNode->m_nMtrlCnt);
		for( int j = 0; j < pActorNode->m_pMeshNode->m_nMtrlCnt; ++j )
		{
			MXmlElement *pMaterialElement = new MXmlElement("MATERIAL");
			pMeshElement->LinkEndChild(pMaterialElement);	
			_SetAttribute(pMaterialElement,"SubID", j);
			_SetAttribute(pMaterialElement,"MtrlName", pActorNode->m_vecMaterialTable[j]->m_pMaterial->GetName());
		}
	}

	return true;
}

unsigned int TNPCInfoMgr::GetColorAttribute( MXmlElement* pElement, const char* szAttrName )
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

bool TNPCInfoMgr::CheckMotionInfoModify()
{
	Clear();

	return Load();
}