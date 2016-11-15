#include "stdafx.h"
#include "TMeshHitInfo.h"
#include "MXml.h"
#include "RMesh.h"
#include "RMeshCollision.h"
#include "MCollision.h"

using namespace rs3;

int TMeshHitInfoMgr::CreateCapsule( string strModelName, int nGroupID )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return -1;
	}

	// 캡슐 ID를 받는다.
	int nCapsuleID				= GetNextCapsuleIndex(strModelName, nGroupID);
	if(nCapsuleID == -1)
	{
		return -1;
	}

	// 같은 그룹 ID를 찾는다.
	CSHitGroup * pPartsHitInfo	= GetHitGroup(strModelName, nGroupID);
	if(pPartsHitInfo)
	{
		// 캡슐 생성
		CSHitCapsule PartsHitCapsule;
		PartsHitCapsule.m_nHitEffectCapsuleID	= -1;
		SetInitCapsuleData(PartsHitCapsule);

		(*pPartsHitInfo).m_vecHitCapsules.push_back(PartsHitCapsule);
		return nCapsuleID;
	}

	// 그룹이 없다.
	// 그룹 생성
	CSHitGroup PartsHitInfo;

	// 캡슐 생성
	CSHitCapsule PartsHitCapsule;
	PartsHitCapsule.m_nHitEffectCapsuleID	= -1;
	SetInitCapsuleData(PartsHitCapsule);
	PartsHitInfo.m_vecHitCapsules.push_back(PartsHitCapsule);

	pMeshInfo->m_mapHitGroupInfo.insert(map<int, CSHitGroup>::value_type(nGroupID, PartsHitInfo));
	return nCapsuleID;
}

int TMeshHitInfoMgr::CreateGroup( string strModelName )
{
	CSMeshInfo * pHitInfo			= m_pMeshInfo->GetInfo(GetModelName(strModelName));

	if(pHitInfo == NULL)
	{
		System::Windows::Forms::MessageBox::Show(gcnew String("모델 정보가 없습니다. (예, npc.xml)"), "판정 캡슐 에러");
		return -1;
	}

	// 새로운 그룹 번호를 가져온다.
	int nGroupID					= GetNextGroupIndex(strModelName);
	if(nGroupID == -1)
	{
		return -1;
	}

	// 그룹이 없다.
	// 그룹 생성
	CSHitGroup PartsHitInfo;
	PartsHitInfo.m_nGroupID = nGroupID;
	pHitInfo->m_mapHitGroupInfo.insert(map<int, CSHitGroup>::value_type(nGroupID, PartsHitInfo));

	return nGroupID;
}

bool TMeshHitInfoMgr::IsCapsuleID( CSHitGroup &pPartsHitInfo, int nCapsuleIndex )
{
	int nIndex = 0;
	vector<CSHitCapsule>::iterator itFind = pPartsHitInfo.m_vecHitCapsules.begin();
	while(itFind != pPartsHitInfo.m_vecHitCapsules.end())
	{
		if(nIndex == nCapsuleIndex)
		{
			return true;
		}

		itFind++;
		nIndex++;
	}

	return false;
}

void TMeshHitInfoMgr::DelGroup( string strModelName, int nGroupIndex )
{
	CSMeshInfo * pMeshInfo			= m_pMeshInfo->GetInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return;
	}

	for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
		itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
	{
		// 같은 그룹 ID를 찾는다.
		if(itParts->first == nGroupIndex)
		{
			pMeshInfo->m_mapHitGroupInfo.erase(itParts);
			return;
		}
	}
}

void TMeshHitInfoMgr::DelCapsule( string strModelName, int nGroupIndex, int nCapsuleIndex )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return;
	}

	for(map<int, CSHitGroup>::iterator itParts = pMeshInfo->m_mapHitGroupInfo.begin();
		itParts != pMeshInfo->m_mapHitGroupInfo.end(); itParts++)
	{
		// 같은 그룹 ID를 찾는다.
		if(itParts->first == nGroupIndex)
		{
			int nCIndex = 0;
			for(vector<CSHitCapsule>::iterator itCapsule = itParts->second.m_vecHitCapsules.begin();
				itCapsule != itParts->second.m_vecHitCapsules.end(); itCapsule++)
			{
				if(nCIndex == nCapsuleIndex)
				{
					itParts->second.m_vecHitCapsules.erase(itCapsule);
					return;
				}

				nCIndex++;
			}
		}
	}
}

string TMeshHitInfoMgr::GetModelName( string strFileName )
{
	return (strFileName.substr(strFileName.rfind("/") + 1, strFileName.rfind(".") - strFileName.rfind("/") - 1));
}

void TMeshHitInfoMgr::SetInitCapsuleData( CSHitCapsule & pCapsule )
{
	pCapsule.m_Capsule.bottom.Set(0, 0, HUMAN_RADIUS);
	pCapsule.m_Capsule.top.Set(0, 0, HUMAN_TOP);
	pCapsule.m_Capsule.radius = HUMAN_RADIUS;
	pCapsule.m_Capsule.CalcHeight();
}

int TMeshHitInfoMgr::GetNextGroupIndex( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo != NULL)
	{
		return pMeshInfo->m_mapHitGroupInfo.size();
	}

	return -1;
}

int TMeshHitInfoMgr::GetNextCapsuleIndex( string strModelName, int nGroupID )
{
	CSHitGroup * pPartsHitInfo = GetHitGroup(strModelName, nGroupID);

	if(pPartsHitInfo)
	{
		return pPartsHitInfo->m_vecHitCapsules.size();
	}

	return -1;
}

CSHitGroup * TMeshHitInfoMgr::GetHitGroup( string strModelName, int nGroupIndex )
{
	CSMeshInfo * pMeshInfo = m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo != NULL)
	{
		return pMeshInfo->FineHitGroup(nGroupIndex);
	}

	return NULL;
}

CSHitCapsule * TMeshHitInfoMgr::GetHitCapsule( CSHitGroup * pInfo, int nCapsuleIndex )
{
	int nIndex = 0;
	for(vector<CSHitCapsule>::iterator itCapsule = pInfo->m_vecHitCapsules.begin();
		itCapsule != pInfo->m_vecHitCapsules.end(); itCapsule++)
	{
		if(nIndex == nCapsuleIndex)
		{
			return &(*itCapsule);
		}

		nIndex++;
	}

	return NULL;
}

CSHitCapsule * TMeshHitInfoMgr::GetHitCapsule( string strModelName, int nGroupIndex, int nCapsuleIndex )
{
	CSHitGroup * pPartsHitInfo = GetHitGroup(strModelName, nGroupIndex);
	
	if(pPartsHitInfo)
	{
		return GetHitCapsule(pPartsHitInfo, nCapsuleIndex);
	}

	return NULL;
}

bool TMeshHitInfoMgr::CreateSimpleHitCapsule( CSMeshInfo * pMeshInfo, RActor * pActor )
{
	// 그룹 생성
	// Simplae은 0번이다.
	CSHitGroup HitInfo;
	HitInfo.m_nGroupID = 0;

	// 캡슐 생성
	CSHitCapsule HitCapsule;
	HitCapsule.m_nHitEffectCapsuleID	= -1;

	//------------------------------------------------------------------------
	//string fn = "../../Data/Model/Monster/" + pMeshHitInfo->m_strMeshName + "/" + pMeshHitInfo->m_strMeshName + ".elu";

	if(pActor)
	{
		RBox b;
		if(pActor->m_pMesh->GetStaticSkeletonBoundingBox(b))
		{
			if (b.maxx <= -FLT_MAX)
			{
				b.vmax.Set(0, 0, 0);
				b.vmin.Set(0, 0, 0);
			}
			RVector c = b.GetCenter();

			float rad, bot, top;

			rad = b.maxx - c.x;
			if (rad < b.maxy - c.y)
				rad = b.maxy - c.y;

			rad *= 1.2f;

			bot = rad;
			if (bot > c.z)
				bot = c.z;

			top = b.maxz - rad;
			if (top < c.z)
				top = c.z;

			top += top * 0.1f;

			HitCapsule.m_Capsule.bottom.Set(0, 0, bot);
			HitCapsule.m_Capsule.top.Set(0, 0, top);
			HitCapsule.m_Capsule.radius = rad;
			HitCapsule.m_Capsule.CalcHeight();
		}
		else
		{
			SetInitCapsuleData(HitCapsule);
		}
	}
	else
	{
		SetInitCapsuleData(HitCapsule);
	}

	HitInfo.m_vecHitCapsules.push_back(HitCapsule);
	pMeshInfo->m_mapHitGroupInfo.insert(map<int, CSHitGroup>::value_type(HitInfo.m_nGroupID, HitInfo));

	return true;
}

bool TMeshHitInfoMgr::IsHitGroup( string strModelName, int nGroupIndex )
{
	CSHitGroup * pHitGroup			= GetHitGroup(strModelName, nGroupIndex);
	if(pHitGroup)
	{
		return true;
	}

	return false;
}

int TMeshHitInfoMgr::CheckNewModelListCount( string strModelFolder )
{
	int nNewModelCount = 0;
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};
	BOOL bRet = TRUE;

	vector<string> vFileList;

	string strReadFileList = strModelFolder + "*.*";
	hDir = FindFirstFile(strReadFileList.c_str(), &DirData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if (DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			if ((strcmp(DirData.cFileName, ".") != 0) &&
				(strcmp(DirData.cFileName, "..") != 0) &&
				(strcmp(DirData.cFileName, ".svn") != 0)) 
			{
				vFileList.push_back(DirData.cFileName);
			}
		}
		bRet = FindNextFile(hDir, &DirData);
	}
	FindClose(hDir);

	for (size_t i=0; i<vFileList.size(); i++)
	{
		string fn = strModelFolder + vFileList[i] + "/" + vFileList[i] + ".elu";

		RActor* a = new RActor;
		a->Create(fn.c_str());

		if (a->m_pMesh == NULL) 
		{
			SAFE_DELETE(a);
			mlog("에러 : %s Mesh가 없어요", fn.c_str());
			continue;
		}

		if(m_pMeshInfo->GetMeshInfo(vFileList[i]) == NULL)
		{
			// 없다.
			nNewModelCount++;
		}

		a->Destroy();
		SAFE_DELETE(a);
	}

	return nNewModelCount;
}

bool TMeshHitInfoMgr::RefreshMeshHitInfo(string strModelName)
{
	strModelName = CSStrings::StringToLower(strModelName);

	CSMeshInfo * pMeshInfo			= m_pMeshInfo->GetInfo(strModelName);
	CSHitGroup * pHitGroup			= GetHitGroup(strModelName, 0);
	if(pHitGroup == NULL)
	{
		if(pMeshInfo == NULL)
		{
			CSMeshInfo info;
			info.m_strMeshName = strModelName;
			m_pMeshInfo->m_mapMeshInfo.insert(map<string, CSMeshInfo>::value_type(strModelName, info));
			pMeshInfo = m_pMeshInfo->GetInfo(strModelName);
		}

		CreateSimpleHitCapsule(pMeshInfo);

		return true;
	}

	return false;
}