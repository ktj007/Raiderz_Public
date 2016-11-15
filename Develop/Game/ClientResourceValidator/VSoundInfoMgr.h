#pragma once

#include "RSample.h"
#include "VFileName.h"
#include "VErrorMsg.h"
#include "CStringResMgr.h"

class VSoundResult
{
private:
	vector<string>						m_vecSoundResult;

public:
	VSoundResult()						{}
	virtual ~VSoundResult()				{}

	bool								LoadInfo();
	bool								CheckSoundDataValidator(string strSoundName);
};

class VSoundInfoMgr
{
public:
	vector<FILENAME_DATA>				m_vecSoundFileList;
	map< string, RSampleInfo>			m_mapSoundInfo;
	VSoundResult						m_ResultMsg;

	VErrorMsg							m_ErrorMsg;

private:
	void								GetSoundFileLIst();

public:
	VSoundInfoMgr();
	virtual ~VSoundInfoMgr();

	bool								LoadingInfo();

	RSampleInfo*						GetSampleInfo(string& strName);
};

