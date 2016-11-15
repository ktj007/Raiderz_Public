#include "stdafx.h"
#include "TMeshInfo.h"
#include "MXml.h"
#include "XMeshInfoLoader.h"
#include "XMeshInfoSaver.h"
#include "TFileNameSampling.h"

using namespace rs3;
using namespace System::IO;

TMeshInfoMgr::TMeshInfoMgr()
{
	m_pMeshHitInfo		= NULL;
	m_pMeshColInfo		= NULL;
	m_pMeshAniInfo		= NULL;
	m_pMeshHitEffectInfo = NULL;

	m_pMeshHitInfo		= new TMeshHitInfoMgr(this);
	m_pMeshColInfo		= new TMeshColInfoMgr(this);
	m_pMeshAniInfo		= new TMeshAniInfoMgr(this);
	m_pMeshHitEffectInfo = new TMeshHitEffectInfoMgr(this);
}

TMeshInfoMgr::~TMeshInfoMgr()
{
	SAFE_DELETE(m_pMeshHitInfo);
	SAFE_DELETE(m_pMeshColInfo);
	SAFE_DELETE(m_pMeshAniInfo);
	SAFE_DELETE(m_pMeshHitEffectInfo);

	m_vecFileList.clear();
}

TMeshInfoMgr& TMeshInfoMgr::GetInstance()
{
	static TMeshInfoMgr instance;
	return instance;
}

void TMeshInfoMgr::LoadMeshInfo()
{
	XMeshInfoLoader meshInfoLoader;
	meshInfoLoader.LoadMeshInfo(this, SERVER_FILENAME_TOOL_MESH_INFO);
}

#define  MESHINFO_XML_TAG_MESHINFO		"MeshInfo"

void TMeshInfoMgr::Save()
{
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	vector<FILE_LIST>& vFileList = GetModelFileList();

	//------------------------------------------------------------------------
	// 데이터 정리
	for(vector<FILE_LIST>::iterator itFile = vFileList.begin(); itFile != vFileList.end(); itFile++)
	{
		RefreshMeshInfo((*itFile).strPureFileName);
	}

	//------------------------------------------------------------------------
	// 저장
	XMeshInfoSaver meshInfoSaver;
	meshInfoSaver.SaveMeshInfo(this, FILENAME_TOOL_MESH_INFO);

	// 서버에도 쓰므로 복사
	File::Copy(FILENAME_TOOL_MESH_INFO, SERVER_FILENAME_TOOL_MESH_INFO, true);
}

vector<FILE_LIST>& TMeshInfoMgr::GetModelFileList()
{
	if(m_vecFileList.size() <= 0)
	{
		//------------------------------------------------------------------------
		// 파일 리스트 가져오기
		TFileNameSampling::GetFileList("../../Data/Model/Monster/", m_vecFileList, NULL, NULL, FA_DIRECTORY);
		TFileNameSampling::GetFileList("../../Data/Model/NPC/", m_vecFileList, NULL, NULL, FA_DIRECTORY);
		TFileNameSampling::GetFileList("../../Data/Model/Player/", m_vecFileList, NULL, NULL, FA_DIRECTORY);
		TFileNameSampling::GetFileList("../../Data/Model/MapObject/", m_vecFileList, NULL, NULL, FA_FILE);
	}

	return m_vecFileList;
}

bool TMeshInfoMgr::RefreshMeshInfo(string strModelName)
{
	if(UseModelHitColCapsule(strModelName))
	{
		bool bHitRefresh = m_pMeshHitInfo->RefreshMeshHitInfo(strModelName);
		bool bColRefresh = m_pMeshColInfo->RefreshMeshColInfo(strModelName);
		bool bAniRefresh = m_pMeshAniInfo->RefreshMeshAniInfo(strModelName, GetModelDirPath(strModelName));

		if (bHitRefresh || bColRefresh || bAniRefresh)
			return true;
	}

	return false;
}

bool TMeshInfoMgr::CheckModelList( string strModelName )
{
	vector<FILE_LIST>& vFileList = GetModelFileList();
	for(vector<FILE_LIST>::iterator itFileList = vFileList.begin(); itFileList != vFileList.end(); ++itFileList)
	{
		if(itFileList->strPureFileName == strModelName)
		{
			return true;
		}
	}

	return false;
}


bool TMeshInfoMgr::UseModelHitColCapsule( string strModelName )
{
	// 이 모델리스트에 있는가?
	if(CheckModelList(strModelName))
	{
		return true;
	}

	return false;
}

string TMeshInfoMgr::GetModelDirPath( string strModelName )
{
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	vector<FILE_LIST>& vFileList = GetModelFileList();

	//------------------------------------------------------------------------
	// 체크
	for(vector<FILE_LIST>::iterator itFile = vFileList.begin(); itFile != vFileList.end(); itFile++)
	{
		if((*itFile).strPureFileName == strModelName)
		{
			return (*itFile).strPath;
		}
	}

	return "";
}

bool TMeshInfoMgr::CreateMeshInfo( string strModelName )
{
	if(GetInfo(strModelName) != NULL)
	{
		// 이미 존재한다.
		return false;
	}

	CSMeshInfo info;
	info.m_strMeshName = strModelName;
	m_mapMeshInfo.insert(map<string, CSMeshInfo>::value_type(strModelName, info));

	return true;
}

CSMeshInfo* TMeshInfoMgr::GetMeshInfo( const string& key )
{
	return GetInfo(key);
}