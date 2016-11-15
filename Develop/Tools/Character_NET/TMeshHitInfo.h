#pragma once

#include "TMeshInfo.h"
#include "CSMeshInfo.h"

class TMeshInfoMgr;

//////////////////////////////////////////////////////////////////////////
class TMeshHitInfoMgr
{
private:
	TMeshInfoMgr *				m_pMeshInfo;

protected:

	bool						IsGroupID(int nGroupIndex);
	bool						IsCapsuleID(CSHitGroup &pPartsHitInfo, int nCapsuleIndex);

	void						SetInitCapsuleData(CSHitCapsule & pCapsule);

	CSHitGroup *				GetHitGroup(string strModelName, int nGroupIndex);
	CSHitCapsule *				GetHitCapsule(CSHitGroup * pInfo, int nCapsuleIndex);
	CSHitCapsule *				GetHitCapsule(string strModelName, int nGroupIndex, int nCapsuleIndex);

	int							CheckNewModelListCount(string strModelFolder);

public:
	TMeshHitInfoMgr(TMeshInfoMgr* pMeshInfo)	{ m_pMeshInfo = pMeshInfo; }

	int							CreateGroup(string strModelName);
	int							CreateCapsule(string strModelName, int nGroupID);
	bool						CreateSimpleHitCapsule(CSMeshInfo * pMeshInfo, RActor * pActor = NULL);

	void						DelGroup(string strModelName, int nGroupIndex);
	void						DelCapsule(string strModelName, int nGroupIndex, int nCapsuleIndex);

	string						GetModelName(string strFileName);
	int							GetNextGroupIndex(string strModelName);
	int							GetNextCapsuleIndex(string strModelName, int nGroupIndex);

	bool						IsHitGroup(string strModelName, int nGroupIndex);

	bool						RefreshMeshHitInfo(string strModelName);
};