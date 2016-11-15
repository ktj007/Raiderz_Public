#ifndef _XGAME_STATE_SWITCH_RESOURCE_MANAGER_H
#define _XGAME_STATE_SWITCH_RESOURCE_MANAGER_H

#pragma once

struct stResourceBackUp
{
	RMesh*		pMesh;
	wstring		strMeshName;
	wstring		strModelName;
};

class XGameStateSwitchResourceMgr
{
private:
	vector<stResourceBackUp>	m_vecResourceBackUp;

public:
	XGameStateSwitchResourceMgr();
	virtual ~XGameStateSwitchResourceMgr();

	void						AddObjectResourceReferenCount();
	void						SubObjectResourceReferenCount();

	wstring						GetObjectResourceName(const wstring& strModelName);						
};
#endif