#include "stdafx.h"
#include "XMeshInfoLoader.h"
#include "CSMeshInfo.h"
#include "CSStrings.h"
#include "XStrings.h"

#define MESHINFO_XML_TAG_HITEFFECTINFO				"HitEffectInfo"

bool XMeshInfoLoader::LoadMeshInfo( CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(MESHINFO_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), MESHINFO_XML_TAG_MESHINFO))
		{
			tstring strTempMeshName;
			CSMeshInfo meshInfo;
			if (_Attribute(strTempMeshName, pElement, MESHINFO_XML_ATTR_MESHNAME))
				meshInfo.m_strMeshName = CSStrings::StringToLower(strTempMeshName);

			int tempBPartCount = 0;
			if(_Attribute(tempBPartCount, pElement, MESHINFO_XML_ATTR_BPARTCOUNT))
				meshInfo.m_iMeshBPartCount = tempBPartCount;

			//tstring strReAction;
			//_Attribute(strReAction, pElement, MESHINFO_XML_ATTR_REACTION_NAME); 
			//meshInfo.m_strReActionAnimationName = CSStrings::StringToLower(strReAction);
			//_Attribute(&meshInfo.m_fReActionBlendWeight, pElement, MESHINFO_XML_ATTR_REACTION_WEIGHT);

			//string strNotReAction;
			//_Attribute(strNotReAction, pElement, MESHINFO_XML_ATTR_NOT_REACTION_NAME); 
			//XStrings::SetAnimationLinkListText(MLocale::ConvAnsiToTCHAR(strNotReAction.c_str()), meshInfo.m_vecNotReActionAnimationList);

			//meshInfo.m_setNotReActionAnimationList.clear();
			//for (size_t i = 0; i < meshInfo.m_vecNotReActionAnimationList.size(); i++)
			//{
			//	meshInfo.m_setNotReActionAnimationList.insert(meshInfo.m_vecNotReActionAnimationList[i]);
			//}

			ParseMeshInfo(&meshInfo, &xml, pElement);

			pMeshInfoMgr->m_mapMeshInfo.insert(map<tstring, CSMeshInfo>::value_type(meshInfo.m_strMeshName, meshInfo));
		}
	}

	return true;
}

void XMeshInfoLoader::ParseMeshInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for(pChild; pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), MESHINFO_XML_TAG_HITINFO))
		{
			ParseMeshHitInfo(pMeshInfo, pXml, pChild);
		}
		else if (!_stricmp(pChild->Value(), MESHINFO_XML_TAG_COLINFO))
		{
			ParseMeshColInfo(pMeshInfo, pXml, pChild);
		}
		else if (!_stricmp(pChild->Value(), MESHINFO_XML_TAG_ANIINFO))
		{
			ParseMeshAniInfo(pMeshInfo, pXml, pChild);
		}
		else if(!_stricmp(pChild->Value(), MESHINFO_XML_TAG_HITEFFECTINFO))
		{
			ParseMeshHitEffectInfo(pMeshInfo, pXml, pChild);
		}
		else if (!_stricmp(pChild->Value(), "BPartsInfo"))
		{
			ParseMeshBPartsInfo(pMeshInfo, pXml, pChild);
		}
	}
}

void XMeshInfoLoader::ParseMeshHitEffectInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{
	for(MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		CSColCapsule hitEffectCapsule;

		_Attribute(hitEffectCapsule.m_strBoneTop, pChild, MESHINFO_XML_ATTR_TOP_BONENAME);
		_Attribute(hitEffectCapsule.m_strBoneBottom, pChild, MESHINFO_XML_ATTR_BOTTOM_BONENAME);

		char buf[200];
		MCapsule c;

		_Contents(buf, pChild);

		sscanf_s(buf, "%f %f %f %f %f %f %f", &c.bottom.x, &c.bottom.y, &c.bottom.z, &c.top.x, &c.top.y, &c.top.z, &c.radius);
		c.CalcHeight();

		hitEffectCapsule.m_Capsule = c;

		pMeshInfo->m_vecHitEffectInfo.push_back(hitEffectCapsule);
	}
}
