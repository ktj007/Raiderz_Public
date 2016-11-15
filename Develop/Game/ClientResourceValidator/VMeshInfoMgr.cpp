#include "stdafx.h"
#include "VMeshInfoMgr.h"
#include "XMeshInfoLoader.h"

//////////////////////////////////////////////////////////////////////////
void VMeshInfo::LoadingInfo( FILENAME_DATA & stMeshName, VErrorMsg* pErrorMsg )
{
	m_strMeshName = stMeshName.strPureFileName;
	m_strMeshFileName = stMeshName.strFileName;
	m_strPathName = stMeshName.strPath;

	string strFileName = stMeshName.strPath + stMeshName.strFileName;
	m_AnimationInfoMgr.LoadInfo(strFileName, pErrorMsg);
}

void VMeshInfo::DeleteInfo()
{
	m_AnimationInfoMgr.DeleteInfo();
}
//////////////////////////////////////////////////////////////////////////
VMeshInfoMgr::VMeshInfoMgr()
{

}

VMeshInfoMgr::~VMeshInfoMgr()
{

}

void VMeshInfoMgr::LoadingMeshInfo( FILENAME_DATA & stMeshName )
{
	// 존재하는지?
	map<string, VMeshInfo*>::iterator itFind = m_mapMeshInfo.find(stMeshName.strPureFileName);
	if(itFind != m_mapMeshInfo.end())
	{
		// 있네...
		return;
	}

	VMeshInfo* pMeshInfo = new VMeshInfo;
	pMeshInfo->LoadingInfo(stMeshName, &m_ErrorMsg);

	m_mapMeshInfo.insert(map<string, VMeshInfo*>::value_type(stMeshName.strPureFileName, pMeshInfo));
}

void VMeshInfoMgr::DeleteMeshInfo()
{
	for(map<string, VMeshInfo*>::iterator it = m_mapMeshInfo.begin(); it != m_mapMeshInfo.end(); ++it)
	{
		it->second->DeleteInfo();
		SAFE_DELETE(it->second);
	}
}

VMeshInfo* VMeshInfoMgr::GetMeshInfo( string strMeshName )
{
	// 소문자로...
	_strlwr( (char*)strMeshName.c_str());	

	map<string, VMeshInfo*>::iterator itFind = m_mapMeshInfo.find(strMeshName);
	if(itFind != m_mapMeshInfo.end())
		return itFind->second;

	return NULL;
}	

void VMeshInfoMgr::LoadingMeshInfoXml()
{
	// mesh_info.xml 로딩
	// 메쉬 정보 로드
	XMeshInfoLoader meshInfoLoader;
	meshInfoLoader.LoadMeshInfo(&m_MeshInfo, V_FILENAME_TOOL_MESH_INFO);
}
