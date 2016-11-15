#pragma once

#define _NEWTON_USE_LIB

#include "Newton.h"
#include "RTypes.h"

typedef float dFloat;

namespace rs3 {

class RStaticMeshSceneNode;
class RTerrain;
class RSceneNode;

class RNewton
{
public:
	RNewton(void);
	~RNewton(void);

//	static RNewton* GetInstance();	// singleton 이어야 하는가

	bool Create();
	void Destory();

	bool CreateCollision(const RSceneNode* pSceneNode);

	void Update();

	void SetWorldSize(const RVector& vecMin, const RVector& vecMax);

	const NewtonWorld* GetNewtonWorld();

	bool AddStaticMesh(RStaticMeshSceneNode* pAABBTree);
	bool AddTerrain(RTerrain* pTerrain);

	static bool GetTerrainNormal(RVector& outNormal,RTerrain* pTerrain,const RVector& position);

	static float TerrainRayHitCallback(NewtonUserMeshCollisionRayHitDesc* rayDesc);

protected:
	NewtonWorld* nWorld;
};

inline const NewtonWorld* RNewton::GetNewtonWorld() { return nWorld; }

}