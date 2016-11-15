#include "stdafx.h"
#include "VMeshInfoLoader.h"
#include "VMeshInfoMgr.h"

VMeshInfoLoader::VMeshInfoLoader()
{

}

VMeshInfoLoader::~VMeshInfoLoader()
{

}

void VMeshInfoLoader::Init()
{
	// 메쉬 리스트 가져오기
	GetMeshFileLIst();
}

void VMeshInfoLoader::GetMeshFileLIst()
{
	m_vecMeshFileList.clear();

	vector<string> vecAddString;
	vecAddString.push_back("elu");

	vector<string> vecSubString;
	vecSubString.push_back("elu.ani");
	vecSubString.push_back("elu.animation.xml");
	vecSubString.push_back("elu.animationevent.xml");
	vecSubString.push_back("elu.animationInfo.xml");
	vecSubString.push_back("elu.xml");
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	VFileName::GetFileList(V_PATH_MONSTER_TOOL, m_vecMeshFileList, &vecAddString, &vecSubString, FA_FILE);
	VFileName::GetFileList(V_PATH_NPC_TOOL, m_vecMeshFileList, &vecAddString, &vecSubString, FA_FILE);
	VFileName::GetFileList(V_PATH_PLAYER_TOOL, m_vecMeshFileList, &vecAddString, &vecSubString, FA_FILE);
	VFileName::GetFileList(V_PATH_MAPOBJ_TOOL, m_vecMeshFileList, &vecAddString, &vecSubString, FA_FILE);
	VFileName::GetFileList(V_PATH_WEAPONS_TOOL, m_vecMeshFileList, &vecAddString, &vecSubString, FA_FILE);
}

void VMeshInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	// 로딩
	int nCompleteCount = 0;
	int nMeshCount = m_vecMeshFileList.size();

	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(vector<FILENAME_DATA>::iterator it = m_vecMeshFileList.begin(); it != m_vecMeshFileList.end(); ++it)
	{
		pValidatorInfoMgr->m_pMeshInfoMgr->LoadingMeshInfo(*it);

		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nMeshInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, nMeshCount);

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Mesh Info Loading... {0}%", nMeshInfoCompletePercent));
	}

	pValidatorInfoMgr->m_pMeshInfoMgr->LoadingMeshInfoXml();

	// 완료
	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Mesh Info Loading Complete."));
}