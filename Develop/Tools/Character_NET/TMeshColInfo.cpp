#include "stdafx.h"
#include "TMeshColInfo.h"
#include "MXml.h"
#include "RMesh.h"
#include "RMeshCollision.h"
#include "MCollision.h"

using namespace rs3;

int TMeshColInfoMgr::CreateCapsule( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return -1;
	}

	// 캡슐 ID를 받는다.
	int nCapsuleID					= GetNextCapsuleIndex(strModelName);
	if(nCapsuleID == -1)
	{
		return -1;
	}

	// 캡슐 생성
	CSColCapsule NewColCapsule;
	SetInitCapsuleData(NewColCapsule);
	pMeshInfo->m_vecColCapsules.push_back(NewColCapsule);

	return nCapsuleID;
}

bool TMeshColInfoMgr::CreateSimpleColCapsule( CSMeshInfo * pMeshInfo, RActor * pActor )
{
	// 캡슐 생성
	CSColCapsule ColCapsule;

	//------------------------------------------------------------------------
	if(pActor)
	{
		RBox b;
		if(pActor->m_pMesh->GetStaticSkeletonBoundingBox(b))
		{
			// 바운드 박스가 있다면...
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

			ColCapsule.m_Capsule.bottom.Set(0, 0, bot);
			ColCapsule.m_Capsule.top.Set(0, 0, top);
			ColCapsule.m_Capsule.radius = rad;
			ColCapsule.m_Capsule.CalcHeight();
		}
		else
		{
			// 없으면 기본으로
			SetInitCapsuleData(ColCapsule);
		}
	}
	else
	{
		SetInitCapsuleData(ColCapsule);
	}

	pMeshInfo->m_vecColCapsules.push_back(ColCapsule);

	return true;
}

void TMeshColInfoMgr::DelCapsule( string strModelName, int nCapsuleIndex )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return;
	}

	int nCIndex = 0;
	for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecColCapsules.begin();
		itCapsule != pMeshInfo->m_vecColCapsules.end(); itCapsule++)
	{
		if(nCIndex == nCapsuleIndex)
		{
			pMeshInfo->m_vecColCapsules.erase(itCapsule);
			return;
		}

		nCIndex++;
	}
}

string TMeshColInfoMgr::GetModelName( string strFileName )
{
	return (strFileName.substr(strFileName.rfind("/") + 1, strFileName.rfind(".") - strFileName.rfind("/") - 1));
}

int TMeshColInfoMgr::GetNextCapsuleIndex( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo != NULL)
	{
		return pMeshInfo->m_vecColCapsules.size();
	}

	return -1;
}

void TMeshColInfoMgr::SetInitCapsuleData( CSColCapsule & pCapsule )
{
	pCapsule.m_Capsule.bottom.Set(0, 0, HUMAN_RADIUS);
	pCapsule.m_Capsule.top.Set(0, 0, HUMAN_TOP);
	pCapsule.m_Capsule.radius = HUMAN_RADIUS;
	pCapsule.m_Capsule.CalcHeight();
}


bool TMeshColInfoMgr::RefreshMeshColInfo(string strModelName)
{
	strModelName = CSStrings::StringToLower(strModelName);

	CSMeshInfo * pMeshMeshInfo	= m_pMeshInfo->GetInfo(strModelName);

	if(pMeshMeshInfo == NULL)
	{
		CSMeshInfo info;
		info.m_strMeshName = strModelName;
		m_pMeshInfo->m_mapMeshInfo.insert(map<string, CSMeshInfo>::value_type(strModelName, info));
		pMeshMeshInfo = m_pMeshInfo->GetInfo(strModelName);

		CreateSimpleColCapsule(pMeshMeshInfo);

		return true;
	}

	return false;
}

int TMeshColInfoMgr::CreateBox( string strModelName, RActor * pActor /*= NULL*/ )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return -1;
	}

	// 박스 ID를 받는다.
	int nBoxID					= GetNextBoxIndex(strModelName);
	if(nBoxID == -1)
	{
		return -1;
	}

	// 박스 생성
	CreateSimpleColBox(pMeshInfo, pActor);

	return nBoxID;
}

bool TMeshColInfoMgr::CreateSimpleColBox( CSMeshInfo * pMeshInfo, RActor * pActor /*= NULL*/ )
{
	// 박스 생성
	CSColBox ColBox;

	//------------------------------------------------------------------------
	if(pActor)
	{
		if(pActor->m_pMesh->GetStaticSkeletonBoundingBox(ColBox))
		{
			ColBox.m_matWorld.MakeIdentity();
		}
		else
		{
			// 없으면 기본으로
			SetInitBoxData(ColBox);
		}
	}
	else
	{
		SetInitBoxData(ColBox);
	}

	pMeshInfo->m_vecColBox.push_back(ColBox);

	return true;
}

void TMeshColInfoMgr::SetInitBoxData( CSColBox & pBox )
{
	MBox initBox;
	pBox.vmin = vec3(-HUMAN_TOP, -HUMAN_TOP, -HUMAN_TOP);
	pBox.vmax = vec3(HUMAN_TOP, HUMAN_TOP, HUMAN_TOP);
	pBox.m_matWorld.MakeIdentity();
}

int TMeshColInfoMgr::GetNextBoxIndex( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo != NULL)
	{
		return pMeshInfo->m_vecColBox.size() + COLLISION_BOX_START_INDEX;
	}

	return -1;
}

void TMeshColInfoMgr::DelBox( string strModelName, int nBoxIndex )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return;
	}

	int nCIndex = COLLISION_BOX_START_INDEX;
	for(vector<CSColBox>::iterator itBox = pMeshInfo->m_vecColBox.begin();
		itBox != pMeshInfo->m_vecColBox.end(); itBox++)
	{
		if(nCIndex == nBoxIndex)
		{
			pMeshInfo->m_vecColBox.erase(itBox);
			return;
		}

		nCIndex++;
	}
}
