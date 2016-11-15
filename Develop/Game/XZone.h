#pragma once

#include "XGlobal.h"
#include "XFieldInfo.h"
#include "XWorldEnv.h"

#include "RTerrain.h"
#include "RCollisionTree.h"

typedef RCollisionTriangle CollisionAttribute;

using namespace rs3;
namespace rs3 {
	class RActorNode;
	class RPointLightSceneNode;
}
namespace physx{ class MPxObjStatic; }

class XCollisionTree;

class XZone
{
protected:
	int						m_nLastCollisionTree_ZoneID;
	int						m_nLastPhysX_ZoneID;

	XCollisionTree*			m_pCollisionTree;				// 충돌맵
	physx::MPxObjStatic*	m_pPhysXStatic;					// PhysX

public:
	bool		m_bPickZPartialArea;		// 지정 영역 사용 여부
	MRect		m_rtPickZPartialArea;	// 지정 영역

protected:
	void ClearPhysX();
	void ClearCollisionTree();

	void LoadPickingZSize(wstring& strFilename);
public:
	XZone();
	virtual ~XZone();

	void Load(XFieldInfo* pFieldInfo, RTerrain* pTerrain);
	void InitPxStaticBody(XFieldInfo* pFieldInfo, RTerrain* pTerrain, vec3 vMyPos);
	void Clear();

	float						GetGroundZ(vec3& vPos, float fPickAddOriginZ = 10.0f);	/// 해당 위치의 바닥 높이를 알아온다.
	bool						PickCollision(vec3 vOrigin, vec3 vTo, vec3* pvOutPos=NULL, vec3* pvOutNormal=NULL, DWORD dwAttribute = ~CollisionAttribute::WATER);
	RMatrix						GetTerrainMatrix(const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bUseAlign);

	wstring						GetMaterialName(vec3 vEntityPosition);

	XCollisionTree*				GetCollisionTree()	{ return m_pCollisionTree; }
};