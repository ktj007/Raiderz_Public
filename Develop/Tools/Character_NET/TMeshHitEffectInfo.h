#pragma once

#include "TMeshInfo.h"
#include "CSMeshInfo.h"

class TMeshInfoMgr;

//////////////////////////////////////////////////////////////////////////
class TMeshHitEffectInfoMgr
{
private:
	TMeshInfoMgr *				m_pMeshInfo;

protected:
	void						SetInitCapsuleData(CSColCapsule & pCapsule);

public:
	TMeshHitEffectInfoMgr(TMeshInfoMgr* pMeshInfo)	{ m_pMeshInfo = pMeshInfo; }

	int							CraateHitEffectCapsule(string strModelName);
	bool						CreateSimpleColCapsule(CSMeshInfo * pMeshInfo, RActor * pActor = NULL);

	void						DelHitEffectCapsule(string strModelName, int nCapsuleIndex);

	string						GetModelName(string strFileName);
	int							GetNextHitEffectCapsuleIndex(string strModelName);
};