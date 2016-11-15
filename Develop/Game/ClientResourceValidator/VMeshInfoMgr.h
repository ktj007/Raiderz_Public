#pragma once

#include "VAnimationInfoMgr.h"
#include "VFileName.h"
#include "VErrorMsg.h"
#include "CSMeshInfo.h"

class VMeshInfo
{
public: 
	string				m_strMeshName;
	string				m_strMeshFileName;
	string				m_strPathName;

	VAnimationInfoMgr	m_AnimationInfoMgr;

public:
	VMeshInfo() {}
	virtual ~VMeshInfo() {}

	void				LoadingInfo(FILENAME_DATA & stMeshName, VErrorMsg* pErrorMsg);
	void				DeleteInfo();
};

class VMeshInfoMgr
{
public:
	map<string, VMeshInfo*>		m_mapMeshInfo;

	CSMeshInfoMgr				m_MeshInfo;

	VErrorMsg					m_ErrorMsg;

public:
	VMeshInfoMgr();
	virtual ~VMeshInfoMgr();

	void				LoadingMeshInfo(FILENAME_DATA & stMeshName);
	void				LoadingMeshInfoXml();
	void				DeleteMeshInfo();

	VMeshInfo*			GetMeshInfo(string strMeshName);
};

