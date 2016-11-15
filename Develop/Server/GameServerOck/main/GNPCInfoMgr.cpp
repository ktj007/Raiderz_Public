#include "stdafx.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GConfig.h"
#include "GEntityNPC.h"
#include "AStlUtil.h"

#define NPC_XML_TAG_MAIET			"maiet"
#define NPC_XML_TAG_NPC				"NPC"

GNPCInfoMgr::GNPCInfoMgr()
{
}

GNPCInfoMgr::~GNPCInfoMgr()
{
	Clear();
}

GNPCInfo* GNPCInfoMgr::Find(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (GNPCInfo*)((*itor).second);
}

bool GNPCInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool GNPCInfoMgr::Load(const wchar_t* szFileName, int nExpectedID)
{
	InitMeshInfo();
	_VLI(FILENAME_NPCINFO);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), NPC_XML_TAG_NPC))
		{
			ParseNPC(pElement, &xml, nExpectedID);
		}
	}

	Cooking();

	return true;
}

void GNPCInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
		delete pNPCInfo;
	}

	clear();
}

void GNPCInfoMgr::Insert( GNPCInfo* pNPCInfo )
{
	if (NULL == pNPCInfo) return;

	this->insert(value_type(pNPCInfo->nID, pNPCInfo));
}

void GNPCInfoMgr::Erase( int nID )
{
	iterator itor = find(nID);
	if (itor == end()) return;

	GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
	pNPCInfo->Clear();
	delete pNPCInfo;

	erase(itor);
}

void GNPCInfoMgr::Cooking()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
		pNPCInfo->Cooking();

		for each (GNPCInfo* each in pNPCInfo->vecModes)
		{
			each->Cooking();
		}

		// 콜트 연결
		if (pNPCInfo->m_nColtLink != INVALID_ID)
		{
			GNPCInfo* pColtProtoNPC = Find(pNPCInfo->m_nColtLink);
			if (pColtProtoNPC)
			{
				pColtProtoNPC->ColtInfo.Copy(pNPCInfo->ColtInfo);
			}
		}
	}
}

bool GNPCInfoMgr::IsQuestNPC(int nID)
{
	GNPCInfo* pNPCInfo = Find(nID);
	if (NULL == pNPCInfo) return false;

	return pNPCInfo->HasQuestIE();
}
