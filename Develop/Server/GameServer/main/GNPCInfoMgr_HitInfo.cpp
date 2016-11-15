#include "stdafx.h"
#include "GNPCInfoMgr.h"
#include "GDef.h"
#include "CSMeshInfoLoader.h"

#define HITINFO_XML_TAG_PARTSHIT_INFO			L"PartsHitInfo"
#define HITINFO_XML_ATTR_PARTSHIT_GROUP			L"Group"
#define HITINFO_XML_ATTR_PARTSHIT_ID			L"ID"
#define HITINFO_XML_ATTR_COLLISION_INDEX		L"ColIndex"

void GNPCInfoMgr::InitMeshInfo()
{
	CSMeshInfoLoader meshInfoLoader;
	if(meshInfoLoader.LoadMeshInfo(&m_MeshInfoMgr, FILENAME_MESH_INFO) == false)
	{
		mlog3("Failed Loading Mesh Hit Info : %s\n", MLocale::ConvUTF16ToAnsi(FILENAME_MESH_INFO).c_str());
		return;
	}
}


