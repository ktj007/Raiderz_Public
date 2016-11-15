#pragma once

#include "MXml.h"
#include "CSMeshInfo.h"
#include "CSCommonLib.h"

class CSMeshInfoMgr;

#define MESHINFO_XML_TAG_MAIET					"maiet"
#define MESHINFO_XML_TAG_MESHINFO				"MeshInfo"

#define MESHINFO_XML_TAG_CAPSULE				"Capsule"
#define MESHINFO_XML_TAG_BOX					"Box"
#define MESHINFO_XML_ATTR_ID					"ID"
#define MESHINFO_XML_ATTR_MESHNAME				"MeshName"
#define MESHINFO_XML_ATTR_TOP_BONENAME			"TopBone"
#define MESHINFO_XML_ATTR_BOTTOM_BONENAME		"BottomBone"
#define MESHINFO_XML_ATTR_REACTION_NAME			"ReActionName"
#define MESHINFO_XML_ATTR_REACTION_WEIGHT		"ReActionWeight"
#define MESHINFO_XML_ATTR_NOT_REACTION_NAME		"NotReActionName"

class CSCOMMON_API CSMeshInfoSaver
{
protected:
	void SaveMeshColInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement);
	void SaveMeshHitInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement);
	void SaveMeshAniInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement);
	void SaveMeshBPartsInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement);

public:
	virtual bool SaveMeshInfo(CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName);
};