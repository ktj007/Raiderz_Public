#pragma once

#include "VFileName.h"
#include "VInfoLoader.h"

class VMeshInfoLoader : public VInfoLoader
{
private:
	vector<FILENAME_DATA>					m_vecMeshFileList;

private:
	void									GetMeshFileLIst();

public:
	VMeshInfoLoader();
	virtual ~VMeshInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_vecMeshFileList.size(); }
	virtual LOADING_STATE					GetID() { return LS_MESH_INFO; }
};
