#include "stdafx.h"
#include "XMeshInfoSaver.h"
#include "CSStrings.h"
#include "XStrings.h"

#define MESHINFO_XML_TAG_HITEFFECTINFO				"HitEffectInfo"

bool XMeshInfoSaver::SaveMeshInfo( CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaietElement = new MXmlElement(MESHINFO_XML_TAG_MAIET);

	for(map<tstring, CSMeshInfo>::iterator it = pMeshInfoMgr->m_mapMeshInfo.begin(); it != pMeshInfoMgr->m_mapMeshInfo.end(); it++)
	{
		MXmlElement* pMeshElement = new MXmlElement(MESHINFO_XML_TAG_MESHINFO);

		string strSaveMeshName = MLocale::ConvTCHARToAnsi(CSStrings::StringToLower((*it).second.m_strMeshName).c_str());
		_SetAttribute(pMeshElement, MESHINFO_XML_ATTR_MESHNAME, strSaveMeshName.c_str());

		SaveMeshHitInfo(&((*it).second), pMeshElement);
		SaveMeshColInfo(&((*it).second), pMeshElement);
		SaveMeshAniInfo(&((*it).second), pMeshElement);
		SaveMeshHitEffectInfo(&((*it).second), pMeshElement);
		SaveMeshBPartsInfo(&((*it).second), pMeshElement);

		pMaietElement->LinkEndChild(pMeshElement);
	}

	root->LinkEndChild(pMaietElement);

	return xml.SaveFile(MLocale::ConvTCHARToAnsi(szFileName).c_str());
}

void XMeshInfoSaver::SaveMeshHitEffectInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement )
{
	MXmlElement* pColElement = new MXmlElement(MESHINFO_XML_TAG_HITEFFECTINFO);

	int nCapsuleIndex = 0;
	for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecHitEffectInfo.begin(); itCapsule != pMeshInfo->m_vecHitEffectInfo.end(); itCapsule++)
	{
		char buf[200] = {0, };

		// Ä¸½¶
		MXmlElement* pCapsule = new MXmlElement(MESHINFO_XML_TAG_CAPSULE);

		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_ID, nCapsuleIndex);
		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_TOP_BONENAME, MLocale::ConvTCHARToAnsi((*itCapsule).m_strBoneTop.c_str()));
		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_BOTTOM_BONENAME, MLocale::ConvTCHARToAnsi((*itCapsule).m_strBoneBottom.c_str()));

		MCapsule c = (*itCapsule).m_Capsule;
		sprintf_s(buf, "%f %f %f %f %f %f %f", c.bottom.x, c.bottom.y, c.bottom.z, c.top.x, c.top.y, c.top.z, c.radius);
		_SetContents(pCapsule, buf);

		pColElement->LinkEndChild(pCapsule);

		nCapsuleIndex++;	
	}

	pElement->LinkEndChild(pColElement);
}