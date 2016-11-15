#include "stdafx.h"
#include "TENPCInfo.h"

#include "RMaterialResource.h"

#define FILENAME_NPC_TOOL				"../../Data/system/npc.xml"
#define FILENAME_NPC_CHANGEMTRL_TOOL	"../../Data/system/npc_changemtrl.xml"

TENPCInfoMgr::TENPCInfoMgr()
{


}

TENPCInfoMgr::~TENPCInfoMgr()
{
	Clear();
}

void TENPCInfo::Clear()
{
	for (vector<TENPCInfo*>::iterator itor = vecModes.begin();
		itor != vecModes.end(); ++itor)
	{
		TENPCInfo* pInfo = (*itor);
		SAFE_DELETE(pInfo);
	}

	vecModes.clear();

	CSNPCInfo::Clear();
}

TENPCInfoMgr& TENPCInfoMgr::GetInstance()
{
	static TENPCInfoMgr m_Instance;
	return m_Instance;
}

TENPCInfo* TENPCInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (TENPCInfo*)((*itor).second);
}

bool TENPCInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool TENPCInfoMgr::Load()
{
	string filename = FILENAME_NPC_TOOL;
	string filename_sub = FILENAME_NPC_CHANGEMTRL_TOOL;

	MXml xml;

	if (!xml.LoadFile(filename.c_str())) 
	{
		mlog("npc.xml 파일이 이상합니다.\n");

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

	return true;
}


void TENPCInfoMgr::ParseNPC(MXmlElement* pElement, MXml* pXml)
{
	USES_CONVERSION_EX;
	string strName;
	string strValue;

	int nValue = 0;

	TENPCInfo* pParentNPCInfo = NULL;
	TENPCInfo* pNewInfo = new TENPCInfo();

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
			pNewInfo->strMeshPath += L"/";
		}
	}

	if (_Attribute(strValue,								pElement, NPC_XML_ATTR_TYPE))
	{
		for (int i = 0; i < NPC_TYPE_MAX; i++)
		{
			if (!stricmp(strValue.c_str(), W2A_EX(NPC_TYPE_STR[i], 0)))
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

void TENPCInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		TENPCInfo* pInfo = (TENPCInfo*)((*itor).second);
		pInfo->Clear();
		delete pInfo;
	}
	clear();
}