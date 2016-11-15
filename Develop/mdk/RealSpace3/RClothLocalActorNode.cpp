#include "StdAfx.h"
#include "RClothLocalActorNode.h"
#include "RClothUtility.h"
#include "RRenderHelper.h"
#include "RActorCollision.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RDebugPassRenderer.h"

namespace rs3 
{

	RClothLocalActorNode::RClothLocalActorNode(void) 
		: RClothActorNode() 
	{

	}


	RClothLocalActorNode::~RClothLocalActorNode(void)
	{
	}

	void RClothLocalActorNode::SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj)
	{
		RMatrix clothWorldTransformMatrix = RMatrix::IDENTITY;
		getClothWorldTransformMatrix(&clothWorldTransformMatrix);
		RShaderCompositeManager::SetTransformConstant(clothWorldTransformMatrix, matView, matViewProj);
	}

	bool RClothLocalActorNode::OnCreateDeviceDependentPart()
	{
		if( false == RClothActorNode::OnCreateDeviceDependentPart() )	return true;


		return true;
	}

	void RClothLocalActorNode::simulate( float timeStep )
	{
		_ASSERT( timeStep >= 0 );
		if( timeStep == 0 )	return;
		
		
		if( true == isAttachedToBone() )
		{
			virtuallyTranslate();
			skinningAttachingPoints();
		}

		integrateByExplicitEulerMethod(timeStep);
		simulateResolvingLogic(timeStep);
		integrateByVerletMethod(timeStep);
	}

	void RClothLocalActorNode::skinningAttachingPoints( void )
	{
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

				vResult = vResult * m_pActor->GetWorldTransform();
			}
			else
			{
				_ASSERT(false);
				vResult = pParticles[i].oriP * GetWorldTransform();
			}
			RMatrix invClothWorldMatrix(RMatrix::IDENTITY);
			getClothInvWorldTransfromMatrix(&invClothWorldMatrix);
			pParticles[i].p	= vResult * invClothWorldMatrix;
		}//for
	}//moveBoneAttachingPoints

	void RClothLocalActorNode::virtuallyTranslate( void )
	{
		RVector virtualTranslation;
		getVirtualTranslation(&virtualTranslation);
		translateMovableClothParticles(&virtualTranslation);
	}

	void RClothLocalActorNode::getVirtualTranslation( RVector *pVirtualTranslation )
	{
		_ASSERT(NULL != pVirtualTranslation);
		*pVirtualTranslation = RVector::ZERO;
		RVector	lastTranslation;
		getLastTranslation(&lastTranslation);

			 if( lastTranslation.z >  getParameter()->getVirtualTranslationThresholdX() )	pVirtualTranslation->x =  getParameter()->getVirtualTranslationSizeX();
		else if( lastTranslation.z < -getParameter()->getVirtualTranslationThresholdX() )	pVirtualTranslation->x = -getParameter()->getVirtualTranslationSizeX();

			 if( lastTranslation.z >  getParameter()->getVirtualTranslationThresholdY() )	pVirtualTranslation->y =  getParameter()->getVirtualTranslationSizeY();
		else if( lastTranslation.z < -getParameter()->getVirtualTranslationThresholdY() )	pVirtualTranslation->y = -getParameter()->getVirtualTranslationSizeY();

			 if( lastTranslation.z >  getParameter()->getVirtualTranslationThresholdZ() )	pVirtualTranslation->z =  getParameter()->getVirtualTranslationSizeZ();
		else if( lastTranslation.z < -getParameter()->getVirtualTranslationThresholdZ() )	pVirtualTranslation->z = -getParameter()->getVirtualTranslationSizeZ();
	}

	void RClothLocalActorNode::translateMovableClothParticles( RVector *pVirtualTranslation )
	{
		if( *pVirtualTranslation == RVector::ZERO ) return;
		RClothParticle*		pParticles		= getClothInfo()->getParticles();
		int	theNumberOfParticles			= getClothInfo()->getTheNumberOfParticles();

		for(int i=0;i<theNumberOfParticles;i++)
		{
			if( pParticles[i].inverseMass == 0 )	continue;
			pParticles[i].p += (*pVirtualTranslation);
			pParticles[i].newP = pParticles[i].p;
		}	
	}


	void RClothLocalActorNode::resolveCollision( void )
	{
		RMatrix		matClothInvWolrd = RMatrix::IDENTITY;
		RBox		clothAABB;
		ROBB		obbForLocalShape;
		RCapsule	CapsuleInClothLocal;
		RSphere		sphereInClothLocal;

		getClothInvWorldTransfromMatrix(&matClothInvWolrd);
		caculateClothAABB(&clothAABB);	m_clothAABBForDebugRender = clothAABB;

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
					CapsuleInClothLocal = CollisionObejct.m_Capsule.Multiply(matCollisionWorldToClothLocal);
					CapsuleInClothLocal.makeOBB(&obbForLocalShape);
					if( false == obbForLocalShape.Intersect(clothAABB) ) continue;
					resolveACollisionWithAllParticles(CapsuleInClothLocal);
				}break;

			case MCollision::SPHERE:
				{
// 					_ASSERT(false); //현재 구와의 충돌은 지원 안함
// 					sphereInClothLocal = CollisionObejct.m_Sphere.Multiply(matWorldForCollisionObject);
// 					sphereInClothLocal.makeOBB(&obbForLocalShape);
// 					if( false == obbForLocalShape.Intersect(clothAABB) ) continue;
// 					resolveACollisionWithAllParticles(sphereInClothLocal);
				}break;
			}
		}

	}


	void RClothLocalActorNode::RenderPrimitive(int index)
	{
		RPFC_THISFUNC;

		if( false == isValid() )	return;
		if(index!=0) return;
		RActorNode::RenderPrimitive(index);

		if( getParameter()->getDrawCollision() == true ) 
		{
			RenderCollision(index);
		}
	}

	void RClothLocalActorNode::RenderCollision(int index)
	{
		RPFC_THISFUNC;

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
					RCapsule CapsuleInClothWorld = CollisionObejct.m_Capsule.Multiply(matWorldForCollisionObject);
					pDebugPassRenderer->AddDebugCapsule( CapsuleInClothWorld, 0.001f, 0x66ff00ff );

					RCapsule CapsuleInClothLocal = CollisionObejct.m_Capsule.Multiply(matCollisionWorldToClothLocal);
					ROBB obb;
					CapsuleInClothLocal.makeOBB(&obb);

					if( false == obb.Intersect(m_clothAABBForDebugRender) )
					{
						pDebugPassRenderer->AddDebugOBox( obb, 0.001f, 0xff000000 );
					}
					else
					{
						pDebugPassRenderer->AddDebugOBox( obb, 0.001f, 0xffff0000 );
					}
				}break;
			}
		}
	}

}