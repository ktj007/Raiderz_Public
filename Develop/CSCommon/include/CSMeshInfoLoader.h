#pragma once

#include "MXml.h"
#include "CSMeshInfo.h"
#include "CSCommonLib.h"

class CSMeshInfoMgr;

#define MESHINFO_XML_TAG_MAIET					"maiet"
#define MESHINFO_XML_TAG_MESHINFO				"MeshInfo"

#define MESHINFO_XML_TAG_HITINFO				"HitInfo"
#define MESHINFO_XML_TAG_COLINFO				"ColInfo"
#define MESHINFO_XML_TAG_ANIINFO				"AniInfo"

#define MESHINFO_XML_ATTR_MESHNAME				"MeshName"
#define MESHINFO_XML_ATTR_TOP_BONENAME			"TopBone"
#define MESHINFO_XML_ATTR_BOTTOM_BONENAME		"BottomBone"
#define MESHINFO_XML_ATTR_REACTION_NAME			"ReActionName"
#define MESHINFO_XML_ATTR_REACTION_WEIGHT		"ReActionWeight"
#define MESHINFO_XML_ATTR_NOT_REACTION_NAME		"NotReActionName"

class CSCOMMON_API CSMeshInfoLoader
{
public:
	virtual bool LoadMeshInfo(CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName);

	virtual void ParseMeshInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);

	void ParseMeshColInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);
	void ParseMeshHitInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);
	void ParseMeshAniInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);
	void ParseMeshBPartsInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);
};