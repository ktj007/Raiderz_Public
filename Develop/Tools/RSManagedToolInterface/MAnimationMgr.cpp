#include "stdafx.h"
#include "MAnimationMgr.h"

using namespace RSManaged;
using namespace System;


MAnimationMgr::MAnimationMgr()
{
	m_pAnimationMgr = new rs3::RAnimationMgr();
}

MAnimationMgr::~MAnimationMgr()
{
	delete m_pAnimationMgr;
}

void MAnimationMgr::DelAll()
{
	m_pAnimationMgr->DelAll();
}

void MAnimationMgr::ReloadAll()
{
	m_pAnimationMgr->ReloadAll();
}

bool MAnimationMgr::LoadAnimationFileList(System::String* strFileName)
{
	char* szFileName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strFileName);

	return m_pAnimationMgr->LoadAnimationFileList(szFileName);
}

void MAnimationMgr::Add(System::String* strName, System::String* strFileName,int nID, int nMotionTypeID)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strName);
	char* szFileName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strFileName);

	m_pAnimationMgr->Add(szName, szFileName, nID, nMotionTypeID);
}
