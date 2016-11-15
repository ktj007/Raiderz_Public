#pragma once

#include "../MPxObjStaticSubActorBuilder.h"

#include "../MPxObjStatic.h"
#include "../MPxObjClothed.h"

#include "RPhysXClothController.h"

namespace rs3
{
	class RCollisionTreeNode;
	class RTerrain;
	class RSceneNode;
	class RActorNode;
}

//////////////////////////////////////////////////////////////////////////
//
// Factory for RPhysX
//
//////////////////////////////////////////////////////////////////////////

namespace physx
{
	// RPhysX SceneNode Factory
	rs3::RSceneNode*			CreateRPhysXSceneNode(const char* pSceneNodeName);
	rs3::RSceneNodeController*	CreateRPhysXController(const char* pControllerName);
}


//////////////////////////////////////////////////////////////////////////
//
// Static Object SubActor Builder
//
//////////////////////////////////////////////////////////////////////////

class RPxObjStaticSubActorBuilder : public physx::MPxObjStaticSubActorBuilder
{
public:
	RPxObjStaticSubActorBuilder() : m_pCollisionTree(NULL), m_pTerrain(NULL), m_fRecommendedCollisionCubeSize(-1.f) {}
	virtual ~RPxObjStaticSubActorBuilder(){}

	void					SetCollisionTreeRootNode(const rs3::RCollisionTreeNode *pNode, float _fCubeSize);
	void					SetTerrain(const rs3::RTerrain* pTerrain, int _nDivisionCount);
	float					GetCubeSizeFromTerrainInfo(){ return m_fRecommendedCollisionCubeSize; }

protected:
	virtual void			OnGetCubeSectorMesh(std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, const MBox& _refPieceAABB);
	virtual void			OnGetSquareSectorMesh(std::vector<NxVec3>& _refVertices, int x1, int y1, int x2, int y2);

private:
	rs3::RCollisionTreeNode*	m_pCollisionTree;
	rs3::RTerrain*				m_pTerrain;
	float						m_fRecommendedCollisionCubeSize;

	void GetCollsionMeshInfo( const MBox& _refAABB, const rs3::RCollisionTreeNode *pNode, std::vector<NxVec3>& _refVertices );
};
