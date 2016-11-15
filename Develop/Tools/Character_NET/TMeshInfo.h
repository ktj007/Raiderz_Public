#pragma once

#include "TMeshHitInfo.h"
#include "TMeshColInfo.h"
#include "TMeshAniInfo.h"
#include "TMeshHitEffectInfo.h"
#include "CSMeshInfo.h"
//////////////////////////////////////////////////////////////////////////
//struct FILE_LIST
//{
//	string					strPath;
//	string					strFileName;
//};
struct FILE_LIST;

class TMeshInfoMgr : public CSMeshInfoMgr
{
private:
	TMeshHitInfoMgr *		m_pMeshHitInfo;
	TMeshColInfoMgr *		m_pMeshColInfo;
	TMeshAniInfoMgr *		m_pMeshAniInfo;
	TMeshHitEffectInfoMgr * m_pMeshHitEffectInfo;

	vector<FILE_LIST>		m_vecFileList;

private:
	bool					CheckModelList(string strModelName);
	
	string					GetModelDirPath(string strModelName);
	vector<FILE_LIST>&		GetModelFileList();

protected:
	TMeshInfoMgr();

public:
	virtual ~TMeshInfoMgr();

	void					LoadMeshInfo();
	void					Save();

	bool					CreateMeshInfo(string strModelName);
	bool					RefreshMeshInfo(string strModelName);

	TMeshHitInfoMgr *		GetMeshHitInfo() { return m_pMeshHitInfo; }
	TMeshColInfoMgr *		GetMeshColInfo() { return m_pMeshColInfo; }
	TMeshAniInfoMgr *		GetMeshAniInfo() { return m_pMeshAniInfo; }
	TMeshHitEffectInfoMgr * GetMeshHitEffectInfo() { return m_pMeshHitEffectInfo; }

	CSMeshInfo*				GetMeshInfo(const string& key);

	bool					UseModelHitColCapsule(string strModelName);

static	TMeshInfoMgr&		GetInstance();
};