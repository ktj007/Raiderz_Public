#pragma once

#include "TMeshInfo.h"
#include "CSMeshInfo.h"

class TMeshInfoMgr;

//////////////////////////////////////////////////////////////////////////
class TMeshColInfoMgr
{
private:
	TMeshInfoMgr *				m_pMeshInfo;

protected:
	void						SetInitCapsuleData(CSColCapsule & pCapsule);
	void						SetInitBoxData(CSColBox & pBox);

public:
	TMeshColInfoMgr(TMeshInfoMgr* pMeshInfo)	{ m_pMeshInfo = pMeshInfo; }

	int							CreateCapsule(string strModelName);
	bool						CreateSimpleColCapsule(CSMeshInfo * pMeshInfo, RActor * pActor = NULL);

	int							CreateBox(string strModelName, RActor * pActor = NULL);
	bool						CreateSimpleColBox(CSMeshInfo * pMeshInfo, RActor * pActor = NULL);

	void						DelCapsule(string strModelName, int nCapsuleIndex);
	void						DelBox(string strModelName, int nBoxIndex);

	string						GetModelName(string strFileName);
	int							GetNextCapsuleIndex(string strModelName);
	int							GetNextBoxIndex(string strModelName);

	bool						RefreshMeshColInfo(string strModelName);
};