#include "StdAfx.h"
#include "XRecipeNPCInfoMgr.h"
#include "XRecipeNPCInfo.h"
#include "XNPCInfo.h"
#include "XFieldInfo.h"


XRecipeNPCInfoMgr::XRecipeNPCInfoMgr(void)
{

}

XRecipeNPCInfoMgr::~XRecipeNPCInfoMgr(void)
{
	Clear();
}

void XRecipeNPCInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool XRecipeNPCInfoMgr::Load( const TCHAR* szFileName )
{
	if (NULL == szFileName) return false;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(RECIPENPC_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), RECIPENPC_XML_TAG_RECIPENPC))
		{
			XRecipeNPCInfo* pNewInfo = new XRecipeNPCInfo;
			if (true == ParseRecipe(pElement, &xml, pNewInfo))
			{
				Insert(pNewInfo);
			}
			else
			{
				SAFE_DELETE(pNewInfo);
			}
		}
	}

	return true;
}

void XRecipeNPCInfoMgr::Insert( XRecipeNPCInfo* pInfo )
{
	m_mapInfo.insert(MAP_RECIPENPCINFO::value_type(pInfo->m_nRecipeID, pInfo));
}

bool XRecipeNPCInfoMgr::ParseRecipe( MXmlElement* pElement, MXml* pXml, XRecipeNPCInfo* pInfo )
{
	int nRecipeID;
	_Attribute(&nRecipeID, pElement, RECIPENPC_XML_ATTR_RECIPE);

	XRecipeNPCInfo* pExistInfo = Get(nRecipeID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		return false;
	}

	pInfo->m_nRecipeID = nRecipeID;

	for (int i = 1; MAX_RECIPENPC  >= i; i++)
	{
		char szFieldlD[16];
		char szNPCID[16];

		sprintf_s(szFieldlD,"%s%d", RECIPENPC_XML_ATTR_FIELD, i);
		sprintf_s(szNPCID,	"%s%d", RECIPENPC_XML_ATTR_NPC, i);

		int nFieldID;
		int nNPCID;
		if (false == _Attribute(nFieldID, pElement, szFieldlD)) continue;
		if (false == _Attribute(nNPCID, pElement, szNPCID)) continue;

		// 필드리스트에 포함되지 않은 필드의 NPC는 로드하지 않는다.
		if (false == info.field->GetFieldList().IsExist(nFieldID)) continue;

		pInfo->m_vecFieldNPC.push_back(pair<int,int>(nFieldID, nNPCID));
	}

	return true;
}

XRecipeNPCInfo* XRecipeNPCInfoMgr::Get( int nRecipeID )
{
	MAP_RECIPENPCINFO::iterator it = m_mapInfo.find(nRecipeID);
	if (it == m_mapInfo.end()) return NULL;

	XRecipeNPCInfo* pInfo = it->second;

	return pInfo;
}

int XRecipeNPCInfoMgr::GetNPCCount( int nRecipeID )
{
	XRecipeNPCInfo* pInfo = Get(nRecipeID);
	if (NULL == pInfo) return 0;

	return static_cast<int>(pInfo->m_vecFieldNPC.size());
}

const TCHAR* XRecipeNPCInfoMgr::GetNPCName(int nRecipeID, int nIndex)
{
	XRecipeNPCInfo* pInfo = Get(nRecipeID);
	if (NULL == pInfo) return NULL;
	if (nIndex >= static_cast<int>(pInfo->m_vecFieldNPC.size())) return NULL;

	int nNPCID = pInfo->m_vecFieldNPC[nIndex].second;

	return info.npc->GetName(nNPCID);	
}

int XRecipeNPCInfoMgr::GetNPCFieldID( int nRecipeID, int nIndex )
{
	XRecipeNPCInfo* pInfo = Get(nRecipeID);
	if (NULL == pInfo) return NULL;
	if (nIndex >= static_cast<int>(pInfo->m_vecFieldNPC.size())) return NULL;

	int nField = pInfo->m_vecFieldNPC[nIndex].first;

	return nField;
}