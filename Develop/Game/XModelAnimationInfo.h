#pragma once
#include "XAnimationInfoMgr.h"

class XModelAnimationInfo
{
private:
	map<wstring, XAnimationInfoMgr>	m_mapAnimationInfoMgr;

private:
	XAnimationInfoMgr *				FindAnimationInfoMgr(const wstring& strMeshName);
public:
	XModelAnimationInfo();
	virtual ~XModelAnimationInfo();

	XAnimationInfoMgr *				GetAnimationInfoMgr(const wstring& strMeshName);
};