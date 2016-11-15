#include "stdafx.h"
#include "TItem.h"
#include "CSItemData.h"
#include "RMaterialResource.h"
#include "TItemData.h"

TItemManager::TItemManager() : CSItemManager()
{
}


TItemManager::~TItemManager()
{
}

CSItemData* TItemManager::NewItemData()
{
	return (new TItemData());
}

CSItemData* TItemManager::GetItemData(string name)
{
	for (CSItemDataMap::iterator itor = m_ItemDataMap.begin();
		itor != m_ItemDataMap.end(); itor++ )
	{
		CSItemData* pItemData = (*itor).second;
		if (pItemData && pItemData->m_strMeshName == name)
		{
			return pItemData;
		}
	}

	return NULL;
}

CSItemData* TItemManager::GetItemData(int id)
{
	CSItemDataMap::iterator i = m_ItemDataMap.find(id);
	if (i != m_ItemDataMap.end())
	{
		return (*i).second;
	}
	return NULL;
}

#define ITEM_XML_ATTR_MESH_PATH			"mesh_path"
#define ITEM_XML_ATTR_ICON				"icon"
bool TItemManager::ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml)
{
	CSItemManager::ParseItem(pElement, pItemData, pXml);

	// 툴에서는 문자열 테이블 참조 말고 직접 읽기
	_Attribute(pItemData->m_strName, pElement, ITEM_XML_ATTR_NAME, pXml);
	_Attribute(pItemData->m_strDesc, pElement, "desc", pXml);


	string strPath;
	if (_Attribute(strPath, pElement, ITEM_XML_ATTR_MESH_PATH))
	{
		if (!strPath.empty())
		{
			if (strPath[strPath.length()-1] != '/' || strPath[strPath.length()-1] != '\\')
				strPath = strPath + '/';

			strPath = PATH_MODEL_TOOL + strPath;

			pItemData->m_strMeshPath = strPath;
		}
	}

	_Attribute(pItemData->m_strIconName, pElement, ITEM_XML_ATTR_ICON);

	if (0 == pItemData->m_nID)
		return false;
	
	m_ItemDataMap.insert(CSItemDataMap::value_type(pItemData->m_nID, (static_cast<XItemData*>(pItemData))));
	return true;
}

void TItemManager::Save_SubMtrl(const char* filename)
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
	root->LinkEndChild(pDec);

	MXmlElement *pRootElement = new MXmlElement(NPC_XML_TAG_MAIET);

	for (CSItemDataMap::iterator itor = m_ItemDataMap.begin();
		itor != m_ItemDataMap.end(); itor++)
	{
		CSItemData* pItemData = (*itor).second;
		if (pItemData == NULL) continue;
		if (pItemData->m_vecMtrls.size() == 0) continue;

		MXmlElement *pItemElement = new MXmlElement(NPC_XML_TAG_ITEM);
		AddSubMtrl_Item(pItemElement, pItemData);
		pRootElement->LinkEndChild(pItemElement);
	}

	root->LinkEndChild(pRootElement);

	xml.SaveFile(filename);
}

bool TItemManager::AddSubMtrl_Item(MXmlElement* pItemElement, CSItemData* pItem)
{
	if (pItem == NULL)	return false;

	_SetAttribute(pItemElement,NPC_XML_ATTR_ID, pItem->m_nID);
	_SetAttribute(pItemElement,NPC_XML_ATTR_NAME, pItem->m_strName.c_str());
	//	_SetAttribute(pItemElement,NPC_XML_ATTR_MESH_NAME, pItem->m_strMeshName.c_str());

	if (!pItem->m_strNodeNames.empty())
	{
		_SetAttribute(pItemElement,NPC_XML_ATTR_MESH_NODE_NAME, pItem->m_strNodeNames[0].c_str());
	}
	

	string actormeshname;
	if (!pItem->m_strMeshPath.empty())
	{
		actormeshname = pItem->m_strMeshPath + "/";
	}
	if (pItem->m_bMeshNameBySex)
	{
		actormeshname += "hf_";
	}

	actormeshname += pItem->m_strMeshName;
	RActor pActor;
	if(pActor.Create(actormeshname.c_str()) == false)
	{
		return false;
	}

	int nCurrentID = -1;
	RActorNode *pActorNode = 0;
	for( int i = 0; i < pActor.GetActorNodeCount(); ++i )
	{
		pActorNode = pActor.GetActorNodes()[i];

		// Dummy나 Bone은 출력하지 않는다.
		if( pActorNode->m_pMeshNode->isBone() || pActorNode->m_pMeshNode->isDummyMesh() )
			continue;

		if( stricmp( pItem->m_strNodeNames[1].c_str(), pActorNode->m_strName.c_str() ) == 0 )
			nCurrentID = i;
	}

	if(nCurrentID < 0)
		return false;

	pActorNode = pActor.GetActorNodes()[nCurrentID];

	MXmlElement *pMeshElement = new MXmlElement("MESH");
	pItemElement->LinkEndChild(pMeshElement);
	_SetAttribute(pMeshElement, "index", nCurrentID);

	RActorNode* pSelectedActorNode = pActor.GetActorNode(pItem->m_strNodeNames[1].c_str());
	if (pSelectedActorNode == NULL) return false;

	// <Sub>
	_SetAttribute(pMeshElement, "SubCount", (int)pSelectedActorNode->m_vecMaterialTable.size());
	for( size_t j = 0; j < pSelectedActorNode->m_vecMaterialTable.size(); ++j )
	{
		MXmlElement *pMaterialElement = new MXmlElement("MATERIAL");
		pMeshElement->LinkEndChild(pMaterialElement);	
		_SetAttribute(pMaterialElement,"SubID", (int)j);
		_SetAttribute(pMaterialElement,"MtrlName", pSelectedActorNode->m_vecMaterialTable.at(j)->m_pMaterial->GetName());
	}

	return true;
}

CSItemData* TItemManager::MakeItemData( int id )
{
	CSItemData* pitem  = NewItemData();
	pitem->m_nID = id;

	m_ItemDataMap.insert(CSItemDataMap::value_type(id, (static_cast<XItemData*>(pitem))));

	return pitem;
}

void TItemManager::Destroy()
{
	for(TItemManager::CSItemDataMap::iterator itr = m_ItemDataMap.begin(); itr != m_ItemDataMap.end(); ++itr)
	{
		CSItemData* data = itr->second;
		SAFE_DELETE(data);
	}

	m_ItemDataMap.clear();
}

void TItemManager::Cooking()
{
	for each (const TItemManager::CSItemDataMap::value_type& each in  m_ItemDataMap)
	{
		CSItemData* data = each.second;
		if (NULL == data) continue;

		data->Cooking();
	}
}