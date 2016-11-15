#include "stdafx.h"
#include "RNewton.h"
#include "MDebug.h"
#include "RTypes.h"
#include "RSceneNode.h"
#include "RStaticMeshSceneNode.h"
#include "RTerrain.h"

#pragma comment (lib, "newton.lib")

namespace rs3 {

// add force and torque to rigid body
void  PhysicsApplyForceAndTorque (const NewtonBody* body)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	RVector force (0.0f, 0.0f, -mass * 9.8f * 10);
	NewtonBodySetForce (body, &force.x);
}

// set the transformation of a rigid body
void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix)
{
	RSceneNode* primitive;

	// get the graphic object form the rigid body
	primitive = (RSceneNode*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	RMatrix& mat = *((RMatrix*)matrix);
	primitive->SetTransform(mat);
}


RNewton::RNewton(void) : nWorld(NULL)
{
}

RNewton::~RNewton(void)
{
}

/*
RNewton* RNewton::GetInstance()
{
	static RNewton m_newton;
	return &m_newton;
}
*/

bool RNewton::Create()
{
	// create the newton world
	nWorld = NewtonCreate (NULL, NULL);
	if(!nWorld) return false;

	int nVersion = NewtonWorldGetVersion (nWorld);
	mlog("newton ( version %d ) created.\n",nVersion);

	RVector maxValue(10000,10000,10000);
	RVector minValue = -maxValue;

	NewtonSetWorldSize(nWorld,minValue,maxValue);

	return true;
}

void RNewton::Destory()
{
	// destroy the Newton world
	NewtonDestroy (nWorld);
}

void RNewton::Update()
{
//	NewtonUpdate (nWorld, (float)GetSceneManager()->GetTimer().GetElapsedTime() / 1000.f);
	NewtonUpdate (nWorld, 1.0f/30.0f);
}

void RNewton::SetWorldSize(const RVector& vecMin, const RVector& vecMax)
{
	NewtonSetWorldSize(nWorld,vecMin,vecMax);
}

bool RNewton::AddStaticMesh(RStaticMeshSceneNode* pAABBTree)
{
	// Create the collision tree geometry
	NewtonCollision* collision = NewtonCreateTreeCollision(nWorld, NULL);

	NewtonTreeCollisionBeginBuild(collision);
	for(int i=0;i<pAABBTree->GetCellCount();i++)
	{
		const RCell* pSector = pAABBTree->GetCell(i);
		
		for(int j=0;j<pSector->m_nPolygon;j++)
		{
//			int nBaseIndex = pSector->m_pPolygonInfo[j].nBaseIndex;
			RVector v[3];
			//v[0] = pSector->m_pVertices[nBaseIndex].pos;
			//v[1] = pSector->m_pVertices[nBaseIndex+1].pos;
			//v[2] = pSector->m_pVertices[nBaseIndex+2].pos;

			// 이게 맞게 고친건지 모름. 일단 컴파일이 안되서 이렇게 바꿈 - birdkr
			v[0] = pSector->m_pPolygonInfo[j].pVertices[0].pos;
			v[1] = pSector->m_pPolygonInfo[j].pVertices[1].pos;
			v[2] = pSector->m_pPolygonInfo[j].pVertices[2].pos;

			NewtonTreeCollisionAddFace(collision, 3, (float*)v, sizeof (RVector), 0);
		}
	}

	NewtonTreeCollisionEndBuild(collision,1);

	NewtonCreateBody(nWorld, collision);	// TODO: 관리가 필요하다

	NewtonReleaseCollision(nWorld,collision);

	return true;
}

bool RNewton::GetTerrainNormal(RVector& outNormal,RTerrain* pTerrain,const RVector& position)
{
	float fNear = min(pTerrain->GetXScale(),pTerrain->GetYScale()) * .5f;
	
	RVector normal(0,0,0);
	int nPickCount = 0;
	RVector pickArround;
	if(pTerrain->GetPickVertical(pickArround, NULL, position.x ,position.y - fNear))
	{
		RVector diff = Normalize(pickArround - position);
		RVector right = CrossProduct(diff,RVector(0,0,1));
		normal += CrossProduct(right,diff); nPickCount++;
	}
	if(pTerrain->GetPickVertical(pickArround, NULL, position.x ,position.y + fNear))
	{
		RVector diff = Normalize(pickArround - position);
		RVector right = CrossProduct(diff,RVector(0,0,1));
		normal += CrossProduct(right,diff); nPickCount++;
	}
	if(pTerrain->GetPickVertical(pickArround, NULL, position.x - fNear,position.y ))
	{
		RVector diff = Normalize(pickArround - position);
		RVector right = CrossProduct(diff,RVector(0,0,1));
		normal += CrossProduct(right,diff); nPickCount++;
	}
	if(pTerrain->GetPickVertical(pickArround, NULL, position.x + fNear,position.y ))
	{
		RVector diff = Normalize(pickArround - position);
		RVector right = CrossProduct(diff,RVector(0,0,1));
		normal += CrossProduct(right,diff); nPickCount++;
	}

	if(nPickCount==0)
	{
		outNormal = RVector(0,0,1);
		return false;
	}
	
	outNormal = 1.f/nPickCount * normal;
	return true;
}

float RNewton::TerrainRayHitCallback(NewtonUserMeshCollisionRayHitDesc* rayDesc)
{
	RTerrain* pTerrain = (RTerrain*)rayDesc->m_userData;

	RVector rayOrigin = RVector(rayDesc->m_p0[0],rayDesc->m_p0[1],rayDesc->m_p0[2]);
	RVector rayDestination = RVector(rayDesc->m_p1[0],rayDesc->m_p1[1],rayDesc->m_p1[2]);

	RVector pickPos;
	if(pTerrain->GetPickLine(pickPos, NULL, rayOrigin,rayDestination))
	{
		RVector normal;
		GetTerrainNormal(normal,pTerrain,pickPos);
		rayDesc->m_normalOut[0] = normal.x;
		rayDesc->m_normalOut[1] = normal.y;
		rayDesc->m_normalOut[2] = normal.z;
		rayDesc->m_userIdOut = 0;
		return (pickPos-rayOrigin).Length() / (rayDestination-rayOrigin).Length();
	}
	return 1.2f;
}

bool RNewton::AddTerrain(RTerrain* pTerrain)
{
//	pTerrain->m_aabb

	// polygon 검사때문에 구현이 쉽지않다.
	/*
	RVector vecMin = (0,0,0);
	RVector vecMax = (pTerrain->GetWidthSize(),pTerrain->GetHeightSize(),10000);

	NewtonCollision* collision = NewtonCreateUserMeshCollision(nWorld, vecMin, vecMax, pTerrain, NULL, TerrainRayHitCallback, NULL);
	NewtonBody* m_level = NewtonCreateBody(nWorld, collision);
	NewtonReleaseCollision(nWorld,collision);
	*/

	NewtonCollision* collision = NewtonCreateTreeCollision(nWorld, NULL);

	NewtonTreeCollisionBeginBuild(collision);
	
	
	int nx = 255;
	int ny = 255;

	_ASSERT(pTerrain->GetHeightCount()==pTerrain->GetWidthCount());

	float fStepX = pTerrain->GetWidthSize() / (float)nx;
	float fStepY = pTerrain->GetHeightSize() / (float)nx;

	for(int i=0;i<nx;i++) {
		for(int j=ny-1;j>=0;j--) {
			RVector v[3];

			pTerrain->GetPickVertical(v[0], NULL, i*fStepX,j*fStepY);
			pTerrain->GetPickVertical(v[2], NULL, (i+1)*fStepX,j*fStepY);
			pTerrain->GetPickVertical(v[1], NULL, i*fStepX,(j-1)*fStepY);
			NewtonTreeCollisionAddFace(collision, 3, (float*)v, sizeof (RVector), 0);

			pTerrain->GetPickVertical(v[0], NULL, (i+1)*fStepX,j*fStepY);
			pTerrain->GetPickVertical(v[2], NULL, (i+1)*fStepX,(j-1)*fStepY);
			pTerrain->GetPickVertical(v[1], NULL, i*fStepX,(j-1)*fStepY);
			NewtonTreeCollisionAddFace(collision, 3, (float*)v, sizeof (RVector), 0);
		}
	}


	NewtonTreeCollisionEndBuild(collision,0);

	NewtonCreateBody(nWorld, collision);	// TODO: 관리가 필요하다

	NewtonReleaseCollision(nWorld,collision);

	return true;
}

bool RNewton::CreateCollision(const RSceneNode* pSceneNode)
{
	for(RSceneNodeList::const_iterator i = pSceneNode->GetChildren().begin();
		i!=pSceneNode->GetChildren().end();++i)
	{
		if(MIsExactlyClass(RStaticMeshSceneNode,*i))
		{
			AddStaticMesh((RStaticMeshSceneNode*)*i);
		}else
		if(MIsExactlyClass(RTerrain,*i))
		{
            AddTerrain((RTerrain*)*i);
		}
		// TODO : actor & tree

		CreateCollision(*i);	// child traverse
	}

	return true;
}

}