#include "stdafx.h"
#include "GActorMeshInfo.h"
#include "CSDef.h"
#include "GMath.h"

GActorMeshInfo::GActorMeshInfo() : m_bSimple(false), m_fMaxRadius(DEFAULT_ENTITY_RADIUS), m_fMinRadius(DEFAULT_ENTITY_RADIUS)
{

}

void GActorMeshInfo::Init( CSMeshInfo* pMeshInfo, float fScale )
{
	m_bSimple = false;
	m_fMaxRadius = DEFAULT_ENTITY_RADIUS;
	m_fMinRadius = DEFAULT_ENTITY_RADIUS;

	if (pMeshInfo == NULL)
	{
		SetDefault(fScale);
		return;
	}

	if (pMeshInfo->m_mapHitGroupInfo.empty() ||
		pMeshInfo->m_mapHitGroupInfo[0].m_vecHitCapsules.empty())
	{
		SetDefault(fScale);
		return;
	}

	m_MeshInfo = *pMeshInfo;
	
	if (fScale != 1.0f)
	{
		for(map<int, CSHitGroup>::iterator itHitGroup = m_MeshInfo.m_mapHitGroupInfo.begin(); itHitGroup != m_MeshInfo.m_mapHitGroupInfo.end(); ++itHitGroup)
		{
			CSHitGroup& hit_group = itHitGroup->second;
			for (size_t j = 0; j < hit_group.m_vecHitCapsules.size(); ++j)
			{
				hit_group.m_vecHitCapsules[j].m_Capsule.Scale(fScale);
			}
		}
	}

	if (m_MeshInfo.m_mapHitGroupInfo.size() == 1)
	{
		if (m_MeshInfo.m_mapHitGroupInfo[0].m_vecHitCapsules.size() == 1)
		{
			MCapsule cap = m_MeshInfo.m_mapHitGroupInfo[0].m_vecHitCapsules[0].m_Capsule;

			bool bCapsuleIsCylinder = false;

			if (MMath::Equals(cap.bottom.x, cap.top.x, 10.0f) &&
				MMath::Equals(cap.bottom.y, cap.top.y, 10.0f))
			{
				bCapsuleIsCylinder = true;
			}
			

			if (bCapsuleIsCylinder)
			{
				SetSimpleCapsule(m_MeshInfo.m_mapHitGroupInfo[0].m_vecHitCapsules[0].m_Capsule);
			}
		}
	}

	SetupRadius();
}

void GActorMeshInfo::SetSimpleCapsule( MCapsule cap )
{
	m_bSimple = true;
	m_capSimple = cap;

}

void GActorMeshInfo::SetDefault( float fScale )
{
	MCapsule default_capsule = MCapsule(vec3(0,0,0), vec3(0,0,150), DEFAULT_ENTITY_RADIUS);
	default_capsule.Scale(fScale);
	SetSimpleCapsule(default_capsule);
}

void GActorMeshInfo::SetForPlayer()
{
	MCapsule default_capsule = MCapsule(vec3(0,0,0), vec3(0,0,150), DEFAULT_ENTITY_RADIUS);
	SetSimpleCapsule(default_capsule);
}

void GActorMeshInfo::SetupRadius()
{
	if (IsSimple())
	{
		float cap_min_y = min(m_capSimple.bottom.y, m_capSimple.top.y);
		float cap_max_y = abs(cap_min_y);

		float fMaxRadius = cap_max_y + m_capSimple.radius;
		float fMinRadius = GMath::GetActorMinRadiusFromCapsule(m_capSimple);
		SetRadius(fMinRadius, fMaxRadius);
		return;
	}

	float max_y = -999999.0f;
	float radius = 0.0f;
	float fMinRadius = DEFAULT_ENTITY_RADIUS;

	if (m_MeshInfo.m_mapHitGroupInfo.empty()) return;

	int nGroupIndex = 0;
	for(map<int, CSHitGroup>::iterator itHitGroup = m_MeshInfo.m_mapHitGroupInfo.begin(); itHitGroup != m_MeshInfo.m_mapHitGroupInfo.end(); ++itHitGroup)
	{
		CSHitGroup& hit_group = itHitGroup->second;
		for (size_t i = 0; i < hit_group.m_vecHitCapsules.size(); ++i)
		{
			CSHitCapsule& hitCapsule = hit_group.m_vecHitCapsules[i];

			if (nGroupIndex == 0 && i == 0)
			{
				fMinRadius = GMath::GetActorMinRadiusFromCapsule(hitCapsule.m_Capsule);
			}

			// 항상 유효시간만 체크 - ex)보르고 - OTL
			if (hitCapsule.IsAlwaysActive() == false) continue;

			MCapsule cap = hitCapsule.m_Capsule;
			float cap_min_y = min(cap.bottom.y, cap.top.y);
			float cap_max_y = abs(cap_min_y);

			if (cap_max_y > max_y)
			{
				max_y = cap_max_y;
				radius = cap.radius;
			}
		}

		++nGroupIndex;
	}

	float fMaxRadius = max(0.0f, max_y + radius);

	SetRadius(fMinRadius, fMaxRadius);
}

float GActorMeshInfo::GetAnimationTime( wstring strAnimationName )
{
	for(vector<CSAniInfo>::iterator itAni = m_MeshInfo.m_vecAniInfo.begin(); itAni != m_MeshInfo.m_vecAniInfo.end(); itAni++)
	{
		if(strAnimationName == (*itAni).m_strAnimationName)
		{
			return (*itAni).m_fAnimationTime;
		}
	}

	return 0.0f;
}

size_t GActorMeshInfo::GetHitGroupCount()
{
	return m_MeshInfo.m_mapHitGroupInfo.size();
}

bool GActorMeshInfo::HasHitGroup(int nGroupIndex)
{
	return (m_MeshInfo.m_mapHitGroupInfo.find(nGroupIndex) != m_MeshInfo.m_mapHitGroupInfo.end());
}

CSHitGroup& GActorMeshInfo::GetHitGroup( int nGroupIndex/*=0*/ )
{
	return m_MeshInfo.m_mapHitGroupInfo[nGroupIndex];
}

void GActorMeshInfo::ReserveHitGroupForTest( int nGroupCount/*=0*/ )
{
	m_MeshInfo.m_mapHitGroupInfo.clear();
	for(int i = 0; i < nGroupCount; ++i)
	{
		CSHitGroup hitGroup;
		m_MeshInfo.m_mapHitGroupInfo.insert(map<int, CSHitGroup>::value_type(i, hitGroup));
	}
}

void GActorMeshInfo::SetRadius( float fMinRadius, float fMaxRadius )
{
	if (fMinRadius < 0.0f) fMinRadius = 0.0f;
	if (fMaxRadius < 0.0f) fMaxRadius = 0.0f;

	m_fMinRadius = fMinRadius;
	m_fMaxRadius = fMaxRadius;

	if (m_fMinRadius > m_fMaxRadius)
	{
		m_fMinRadius = m_fMaxRadius;
	}
}