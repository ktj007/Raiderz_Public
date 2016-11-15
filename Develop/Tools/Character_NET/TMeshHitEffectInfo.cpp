#include "stdafx.h"
#include "TMeshHitEffectInfo.h"
#include "MXml.h"
#include "RMesh.h"
#include "RMeshCollision.h"
#include "MCollision.h"

using namespace rs3;

bool TMeshHitEffectInfoMgr::CreateSimpleColCapsule( CSMeshInfo * pMeshInfo, RActor * pActor )
{
	// Ä¸½¶ »ý¼º
	CSColCapsule HitEffectCapsule;

	//------------------------------------------------------------------------
	if(pActor)
	{
		RBox b;
		if(pActor->m_pMesh->GetStaticSkeletonBoundingBox(b))
		{
			// ¹Ù¿îµå ¹Ú½º°¡ ÀÖ´Ù¸é...
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

			HitEffectCapsule.m_Capsule.bottom.Set(0, 0, bot);
			HitEffectCapsule.m_Capsule.top.Set(0, 0, top);
			HitEffectCapsule.m_Capsule.radius = rad;
			HitEffectCapsule.m_Capsule.CalcHeight();
		}
		else
		{
			// ¾øÀ¸¸é ±âº»À¸·Î
			SetInitCapsuleData(HitEffectCapsule);
		}
	}
	else
	{
		SetInitCapsuleData(HitEffectCapsule);
	}

	pMeshInfo->m_vecHitEffectInfo.push_back(HitEffectCapsule);

	return true;
}

string TMeshHitEffectInfoMgr::GetModelName( string strFileName )
{
	return (strFileName.substr(strFileName.rfind("/") + 1, strFileName.rfind(".") - strFileName.rfind("/") - 1));
}

void TMeshHitEffectInfoMgr::SetInitCapsuleData( CSColCapsule & pCapsule )
{
	pCapsule.m_Capsule.bottom.Set(0, 0, HUMAN_RADIUS);
	pCapsule.m_Capsule.top.Set(0, 0, HUMAN_TOP);
	pCapsule.m_Capsule.radius = HUMAN_RADIUS;
	pCapsule.m_Capsule.CalcHeight();
}

int TMeshHitEffectInfoMgr::CraateHitEffectCapsule( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return -1;
	}

	// Ä¸½¶ ID¸¦ ¹Þ´Â´Ù.
	int nCapsuleID					= GetNextHitEffectCapsuleIndex(strModelName);
	if(nCapsuleID == -1)
	{
		return -1;
	}

	// Ä¸½¶ »ý¼º
	CSColCapsule NewHitEffectCapsule;
	SetInitCapsuleData(NewHitEffectCapsule);
	pMeshInfo->m_vecHitEffectInfo.push_back(NewHitEffectCapsule);

	return nCapsuleID;
}

int TMeshHitEffectInfoMgr::GetNextHitEffectCapsuleIndex( string strModelName )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo != NULL)
	{
		return pMeshInfo->m_vecHitEffectInfo.size();
	}

	return -1;
}

void TMeshHitEffectInfoMgr::DelHitEffectCapsule( string strModelName, int nCapsuleIndex )
{
	CSMeshInfo * pMeshInfo		= m_pMeshInfo->GetMeshInfo(GetModelName(strModelName));

	if(pMeshInfo == NULL)
	{
		return;
	}

	int nCIndex = 0;
	for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecHitEffectInfo.begin();
		itCapsule != pMeshInfo->m_vecHitEffectInfo.end(); itCapsule++)
	{
		if(nCIndex == nCapsuleIndex)
		{
			pMeshInfo->m_vecHitEffectInfo.erase(itCapsule);
			return;
		}

		nCIndex++;
	}
}