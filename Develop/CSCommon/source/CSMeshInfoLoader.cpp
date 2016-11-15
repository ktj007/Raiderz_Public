#include "stdafx.h"
#include "CSMeshInfoLoader.h"
#include "MXml.h"
#include "CSStrings.h"
#include "MLocale.h"


#define MESHINFO_XML_TAG_GROUP					"Group"
#define MESHINFO_XML_TAG_CAPSULE				"Capsule"
#define MESHINFO_XML_TAG_BOX					"Box"
#define MESHINFO_XML_TAG_ANIMATION				"Animation"

#define MESHINFO_XML_ATTR_ID					"ID"
#define MESHINFO_XML_ATTR_COLINDEX				"ColIndex"
#define MESHINFO_XML_ATTR_NAME					"name"
#define MESHINFO_XML_ATTR_TIME					"time"
#define MESHINFO_XML_ATTR_ACTIVE_TIME_BEGIN		"ActiveTimeBegin"
#define MESHINFO_XML_ATTR_ACTIVE_TIME_END		"ActiveTimeEnd"
#define MESHINFO_XML_ATTR_LINKED_BPART			"LinkedBPart"

bool CSMeshInfoLoader::LoadMeshInfo( CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName )
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
			if (!_Attribute(strTempMeshName, pElement, MESHINFO_XML_ATTR_MESHNAME)) 
				continue;

			meshInfo.m_strMeshName = CSStrings::StringToLower(strTempMeshName);

			ParseMeshInfo(&meshInfo, &xml, pElement);

			pMeshInfoMgr->m_mapMeshInfo.insert(map<tstring, CSMeshInfo>::value_type(meshInfo.m_strMeshName, meshInfo));
		}
	}

	return true;
}

void CSMeshInfoLoader::ParseMeshInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
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
		else if (!_stricmp(pChild->Value(), "BPartsInfo"))
		{
			ParseMeshBPartsInfo(pMeshInfo, pXml, pChild);
		}
	}
}

void CSMeshInfoLoader::ParseMeshColInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{	
	for(MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		if(!_stricmp(pChild->Value(), MESHINFO_XML_TAG_CAPSULE))
		{
			CSColCapsule colCapsule;

			_Attribute(colCapsule.m_strBoneTop, pChild, MESHINFO_XML_ATTR_TOP_BONENAME);
			_Attribute(colCapsule.m_strBoneBottom, pChild, MESHINFO_XML_ATTR_BOTTOM_BONENAME);

			char buf[200];
			MCapsule c;

			_Contents(buf, pChild);

			sscanf(buf, "%f %f %f %f %f %f %f", &c.bottom.x, &c.bottom.y, &c.bottom.z, &c.top.x, &c.top.y, &c.top.z, &c.radius);
			c.CalcHeight();

			colCapsule.m_Capsule = c;

			pMeshInfo->m_vecColCapsules.push_back(colCapsule);
		}
		else if(!_stricmp(pChild->Value(), MESHINFO_XML_TAG_BOX))
		{
			CSColBox colBox;
			colBox.m_matWorld.MakeIdentity();

			char buf[2048];
			_Contents(buf, pChild);

			sscanf(buf, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
				&colBox.m_matWorld._11, &colBox.m_matWorld._12, &colBox.m_matWorld._13,
				&colBox.m_matWorld._21, &colBox.m_matWorld._22, &colBox.m_matWorld._23,
				&colBox.m_matWorld._31, &colBox.m_matWorld._32, &colBox.m_matWorld._33,
				&colBox.m_matWorld._41, &colBox.m_matWorld._42, &colBox.m_matWorld._43,
				&colBox.vmax.x, &colBox.vmax.y, &colBox.vmax.z, 
				&colBox.vmin.x, &colBox.vmin.y, &colBox.vmin.z);

			pMeshInfo->m_vecColBox.push_back(colBox);
		}
	}
}

void CSMeshInfoLoader::ParseMeshHitInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{
	std::map<int, CSHitGroup> mapHitcapsuleGroup;

	for(MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		// ±×·ì ID
		CSHitGroup PartsInfo;
		_Attribute(PartsInfo.m_nGroupID, pChild, MESHINFO_XML_ATTR_ID);

		for(MXmlElement* pCapsule = pChild->FirstChildElement(MESHINFO_XML_TAG_CAPSULE); pCapsule != NULL; pCapsule = pCapsule->NextSiblingElement())
		{
			CSHitCapsule PartsCapsule;

			// Ãæµ¹ Ä¸½¶ ID
			_Attribute(PartsCapsule.m_nHitEffectCapsuleID, pCapsule, MESHINFO_XML_ATTR_COLINDEX);

			_Attribute(PartsCapsule.nLinkedBPart, pCapsule, MESHINFO_XML_ATTR_LINKED_BPART);

			// ActiveTime
			float fActiveTimeBegin = 0.0f, fActiveTimeEnd = 0.0f;

			_Attribute(&fActiveTimeBegin, pCapsule, MESHINFO_XML_ATTR_ACTIVE_TIME_BEGIN);
			_Attribute(&fActiveTimeEnd, pCapsule, MESHINFO_XML_ATTR_ACTIVE_TIME_END);
			PartsCapsule.SetActiveTime(fActiveTimeBegin, fActiveTimeEnd);

			char buf[200];
			MCapsule c;

			_Contents(buf, pCapsule);

			sscanf(buf, "%f %f %f %f %f %f %f", &c.bottom.x, &c.bottom.y, &c.bottom.z, &c.top.x, &c.top.y, &c.top.z, &c.radius);
			c.CalcHeight();

			PartsCapsule.m_Capsule = c;
			PartsInfo.m_vecHitCapsules.push_back(PartsCapsule);
		}

		if (PartsInfo.m_nGroupID < 0)
			continue;

		mapHitcapsuleGroup.insert(std::make_pair(PartsInfo.m_nGroupID, PartsInfo));
	}

	if (mapHitcapsuleGroup.empty())
		return;

	int nMaxHitCapsuleGroupID = -1;
	for each (pair<int, CSHitGroup> each in mapHitcapsuleGroup)
	{
		nMaxHitCapsuleGroupID = max(each.first, nMaxHitCapsuleGroupID);
	}

	if (nMaxHitCapsuleGroupID < 0)
		return;

	//pMeshInfo->m_mapHitGroupInfo.resize(nMaxHitCapsuleGroupID+1);

	// Ä¸½¶±×·ìº¤ÅÍ¿¡ ºóÆ´¾øÀÌ ³Ö±â
	for each (pair<int, CSHitGroup> each in mapHitcapsuleGroup)
	{
		pMeshInfo->m_mapHitGroupInfo[each.first] = each.second;
	}
}

void CSMeshInfoLoader::ParseMeshAniInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{
	for(MXmlElement* pChild = pElement->FirstChildElement(MESHINFO_XML_TAG_ANIMATION); pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		CSAniInfo AniInfo;
		_Attribute(AniInfo.m_strAnimationName , pChild, MESHINFO_XML_ATTR_NAME);
		_Attribute(&AniInfo.m_fAnimationTime, pChild, MESHINFO_XML_ATTR_TIME);

		pMeshInfo->m_vecAniInfo.push_back(AniInfo);
	}
}

void CSMeshInfoLoader::ParseMeshBPartsInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement )
{
	for(MXmlElement* pChild = pElement->FirstChildElement("bpart"); pChild != NULL; pChild=pChild->NextSiblingElement())
	{
		int id = -1;
		_Attribute(id, pChild, "id");
		if (id >=1 && id <= 4)
		{
			_Attribute(pMeshInfo->m_strBPartsStartSound[id-1], pChild, "StartSound", pXml);
			_Attribute(pMeshInfo->m_strBPartsCollisionSound[id-1], pChild, "CollisionSound", pXml);
		}
	}
}
