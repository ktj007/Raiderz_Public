#include "StdAfx.h"
#include "RClothWorldActorNode.h"
#include "RClothUtility.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "MOBox.h"
#include "RActorCollision.h"
#include "RClothCollision.h"
#include "RDebugPassRenderer.h"

namespace rs3 
{

	RClothWorldActorNode::RClothWorldActorNode(void) 
		: RClothActorNode()
	{
		m_bIntersectWithPartitionedOBBForDebugRender = false;
	}	


	RClothWorldActorNode::~RClothWorldActorNode(void)
	{
	}

	void RClothWorldActorNode::SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj)
	{
		RShaderCompositeManager::SetTransformConstant(RMatrix::IDENTITY, matView, matViewProj);
	}

	bool RClothWorldActorNode::OnCreateDeviceDependentPart()
	{
		if( false == RClothActorNode::OnCreateDeviceDependentPart() ) return true;
		
		RMatrix matClothWorld(RMatrix::IDENTITY);
		getClothWorldTransformMatrix(&matClothWorld);
		transformParticles(matClothWorld);
		return true;
	}

	void RClothWorldActorNode::simulate( float timeStep )
	{
		_ASSERT( timeStep >= 0 );
		if( timeStep == 0 )	return;


		warpCloth();
		if( true == isAttachedToBone() )
		{	
			skinningAttachingPoints();
		}
		else
		{
			RMatrix matClothWorld(RMatrix::IDENTITY);
			getClothWorldTransformMatrix(&matClothWorld);
			transformAttachingParticles(matClothWorld);
		}

		integrateByExplicitEulerMethod(timeStep);
		simulateResolvingLogic(timeStep);
		integrateByVerletMethod(timeStep);
	}



	void RClothWorldActorNode::skinningAttachingPoints( void )
	{
		RPFC_THISFUNC;

		RMatrix _matBone[60];// Mesh에서 참조하는 Bone의 최대 개수
		for(int i = 0; i < m_pMeshNode->m_nBoneTableCnt; ++i)
		{
			_matBone[i] = m_pMeshNode->m_matBase
				* m_refBones[i]->m_pMeshNode->m_matInvBase				
				* m_refBones[i]->GetResultTransform();
		}
		
		RClothParticle*		pParticles		= getClothInfo()->getParticles();
		int	theNumberOfParticles			= getClothInfo()->getTheNumberOfParticles();
		RVector vResult;
		int		nRefBone;
		int		nCid;
		float	fWeight;
		RVector vCurPos;

		RMatrix matClothWorld(RMatrix::IDENTITY);
		matClothWorld = m_pActor->GetWorldTransform();

		for( int i = 0; i < theNumberOfParticles; ++i )
		{
			if( pParticles[i].inverseMass != 0 )	continue;

			vResult = RVector( 0, 0, 0 );
			if( m_pMeshNode->isHWPhysique() )
			{
				const WORD& index = m_pMeshNode->m_pVertexIndexTable[i].p;
				_ASSERT( index < m_pMeshNode->m_nPhysiqueCnt );
				nRefBone = m_pMeshNode->m_pPhysique[index].m_num;	

				for( int j = 0; j < nRefBone; ++j )
				{
					nCid	= m_pMeshNode->m_pPhysique[index].GetBoneID(j);
					fWeight	= m_pMeshNode->m_pPhysique[index].GetWeight(j);

					vCurPos	= pParticles[i].oriP * _matBone[nCid];
					vResult	+= vCurPos * fWeight;
				}

				vResult = vResult * matClothWorld;
			}
			else
			{
				_ASSERT(false);
				vResult = pParticles[i].oriP * GetWorldTransform();
			}
			pParticles[i].p	= vResult;
			pParticles[i].newP = pParticles[i].p;
		}//for
	}//moveBoneAttachingPoints



	void RClothWorldActorNode::resolveCollision( void )
	{
		RBox		clothAABB;
		RCapsule	capsuleInWorldCoordinates;
		RSphere		sphereInWorldCoordinates;
		ROBB		obbForWorldShape;
		RMatrix		matClothInvWolrd = RMatrix::IDENTITY;

		caculateClothAABB(&clothAABB);	m_clothAABBForDebugRender = clothAABB;
		getClothInvWorldTransfromMatrix(&matClothInvWolrd);

		for(unsigned int i = 0; i < m_pActor->m_pCollision->m_vCollisions.size(); ++i)
		{
			RActorCollisionObject& CollisionObejct = m_pActor->m_pCollision->m_vCollisions[m_vSortedCollisionIndices[i]];
			if(CollisionObejct.m_MeshType != MCollision::MESHTYPE_CLOTH)		continue; // 옷감용 충돌체만

			RMatrix matWorldForCollisionObject = CollisionObejct.m_matTransform * CollisionObejct.m_pParent->GetWorldTransform();
			RMatrix	matCollisionWorldToClothLocal = matWorldForCollisionObject * matClothInvWolrd;
			
			switch(CollisionObejct.m_Type)
			{
				case MCollision::CAPSULE:
				{
					capsuleInWorldCoordinates = CollisionObejct.m_Capsule.Multiply(matWorldForCollisionObject);
					capsuleInWorldCoordinates.makeOBB(&obbForWorldShape);

					if( false == obbForWorldShape.Intersect(clothAABB) )			continue;
//					if( false == intersectWithPartitionedOBB(obbForWorldShape) )	continue;

					m_bIntersectWithPartitionedOBBForDebugRender = true;
					resolveACollisionWithAllParticles(capsuleInWorldCoordinates);

				}break;

				case MCollision::SPHERE:
				{
// 					_ASSERT( false ); //현재 구는 지원 안함
// 					sphereInWorldCoordinates = CollisionObejct.m_Sphere.Multiply(matWorldForCollisionObject);
// 					sphereInWorldCoordinates.makeOBB(&obbForWorldShape);
// 					if( false == obbForWorldShape.Intersect(clothAABB) ) continue;
// 					resolveACollisionWithAllParticles(sphereInWorldCoordinates);
				}break;
			}
		}

	}



	void RClothWorldActorNode::RenderPrimitive(int index)
	{
		if( false == isValid() )	return;
		if(index!=0) return;
		RActorNode::RenderPrimitive(index);


		if( getParameter()->getDrawCollision() == true ) 
			RenderCollision(index);
	}

	void RClothWorldActorNode::RenderCollision(int index)
	{
		RDebugPassRenderer* pDebugPassRenderer = REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>();
		_ASSERT( pDebugPassRenderer );

		RenderNormal();

		ROBB obbcloth;
		AABB2OBB(m_clothAABBForDebugRender,obbcloth);
		pDebugPassRenderer->AddDebugOBox( obbcloth, 0.001f, 0x6600ff00 );

		RMatrix	matClothInvWolrd = RMatrix::IDENTITY;
		getClothInvWorldTransfromMatrix(&matClothInvWolrd);
		for (size_t i=0; i<m_pActor->m_pCollision->m_vCollisions.size(); i++)
		{
			RActorCollisionObject& CollisionObejct = m_pActor->m_pCollision->m_vCollisions[i];
			if(CollisionObejct.m_MeshType != MCollision::MESHTYPE_CLOTH) continue;
			RMatrix matWorldForCollisionObject = CollisionObejct.m_matTransform * CollisionObejct.m_pParent->GetWorldTransform();
			RMatrix	matCollisionWorldToClothLocal = matWorldForCollisionObject * matClothInvWolrd;

			switch(CollisionObejct.m_Type)
			{
			case MCollision::CAPSULE :
				{
					RCapsule CapsuleTransformedInWorldCoordinates = CollisionObejct.m_Capsule.Multiply(matWorldForCollisionObject);
					pDebugPassRenderer->AddDebugCapsule( CapsuleTransformedInWorldCoordinates, 0.001f, 0x66ff00ff );
					ROBB obb;
					CapsuleTransformedInWorldCoordinates.makeOBB(&obb);
					if( false == obb.Intersect(m_clothAABBForDebugRender) )
					{
						pDebugPassRenderer->AddDebugOBox( obb, 0xff000000 );
					}
					else
					{
						if(  true == m_bIntersectWithPartitionedOBBForDebugRender)
						{
							pDebugPassRenderer->AddDebugOBox( obb, 0xffff0000 );
						}
						else
						{
							pDebugPassRenderer->AddDebugOBox( obb, 0xff000000 );
						}
					}
				}break;
			}//switch
		}//for

		for(unsigned int j=0;j<getClothInfo()->m_partitionedTriangles.m_theNumberOfTriangleGroup;j++)
		{
			int n = getClothInfo()->m_partitionedTriangles.m_pTriangleGroups[j].m_theNumberOfTriangles;
			RBox aabb;
			aabb.Initialize();
			for(int i=0;i<n;i++)
			{
				RC_TRIANGLE* pTriangle = &getClothInfo()->m_partitionedTriangles.m_pTriangleGroups[j].m_pTriangles[i];
				RClothParticle* pParticles = getClothInfo()->getParticles();

				RC_VERTEX_INDEX &index0 = pTriangle->index0;
				RC_VERTEX_INDEX &index1 = pTriangle->index1;
				RC_VERTEX_INDEX &index2 = pTriangle->index2;

				aabb.Add(pParticles[index0].p);
				aabb.Add(pParticles[index1].p);
				aabb.Add(pParticles[index2].p);
			}
			ROBB obb;
			AABB2OBB(aabb,obb);
			pDebugPassRenderer->AddDebugOBox( obb, 0.001f, 0x66ffff00 );
		}
	}

	bool RClothWorldActorNode::intersectWithPartitionedOBB( ROBB &obbForWorldShape )
	{
		RAABB clothPartitionedAABB;
		for(unsigned int i=0;i<getClothInfo()->m_partitionedTriangles.m_theNumberOfTriangleGroup;i++)
		{
			getClothInfo()->calculateAABBOfTheGroup(&clothPartitionedAABB,i);
			if( true == obbForWorldShape.Intersect(clothPartitionedAABB) )
			{
				return true;
			}
		}
		return false;
	}

	void RClothWorldActorNode::warpCloth( void )
	{
		float warpThreshold = getClothInfo()->getParameter()->getWarpThreshold();

		RVector lastTranslation;
		getLastTranslation(&lastTranslation);
		if( lastTranslation.Length() > warpThreshold )
		{
			RMatrix matClothWorld(RMatrix::IDENTITY);
			getClothWorldTransformMatrix(&matClothWorld);
			transformParticles(matClothWorld);
		}

	}

}

