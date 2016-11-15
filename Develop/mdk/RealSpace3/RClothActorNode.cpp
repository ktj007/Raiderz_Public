#include "StdAfx.h"
#include "RClothActorNode.h"
#include "RClothUtility.h"
#include "RClothLocalActorNode.h"
#include "RClothWorldActorNode.h"
#include "RActorCollision.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RDebugPassRenderer.h"

namespace rs3
{

	MImplementRTTI(RClothActorNode, RActorNode);

	RClothActorNode::RClothActorNode(void)
		: RActorNode(ACTOR_NODE_CLOTH),RClothSimulator(),m_nLastUpdatedFrame(0)
	{
		m_beforeDuaration = m_currentDuration = 0.0f;
		m_pBoneForWorldTransform = NULL;
		m_bValidModel = false;
		// 클로스는 인스턴스 별 메쉬의 형태가 다르므로 애초에 인스턴싱이 불가능하다.
		m_bCanInstancing = false;
	}
		
	RClothActorNode::~RClothActorNode(void)
	{
	}

	bool RClothActorNode::OnCreateDeviceDependentPart()
	{
		RActorNode::OnCreateDeviceDependentPart();

		if( 0 == isValidModeling() )
		{
			m_bValidModel = true;
		}
		else
		{
			m_bValidModel = false;
		}	

		if( false == isValid() )	return false;

		RClothInfo *pClothInfo = allocateClothInfo();

		if( false == pClothInfo->load(m_fileName.c_str()) )
		{
			RClothUtility::makeClothInfoFromMeshNode(pClothInfo,m_pMeshNode);
		}

		setClothInfo(pClothInfo);
		makeSortedCollision();

		if( true == isAttachedToBone() )
		{
			m_pBoneForWorldTransform = getBoneForWorldTransformFromBoneName(getBoneNameForWorldTransformFromProperty()); 
			_ASSERT( NULL != m_pBoneForWorldTransform );
		}

		getClothInvWorldTransfromMatrix(&m_beforeClothWorldMatrix);
		getClothInvWorldTransfromMatrix(&m_currentClothWorldMatrix);

		return true;
	}


	void RClothActorNode::UpdateTransformAndVertex()
	{
		if( false == isValid() )	return;

		if( m_nLastUpdatedFrame == REngine::GetDevice().GetFrameCount()) 
			return;
		m_nLastUpdatedFrame = REngine::GetDevice().GetFrameCount();

		m_beforeClothWorldMatrix = m_currentClothWorldMatrix;
		getClothInvWorldTransfromMatrix(&m_currentClothWorldMatrix);
		RPFC_B("RClothActorNode::simulateByFixedTime()");
		simulateByFixedTime();
		RPFC_E;

		RPFC_B("RClothActorNode::calculateNormals()");
		calculateNormals();
		RPFC_E;
	}

	void RClothActorNode::SetVertexStream( bool b2ndBufferMng)
	{
		RPFC_THISFUNC;
		RClothParticle* pParticle = getClothInfo()->getParticles();

		bool bNeedLock = (m_pMeshNode->m_pVb->m_bUseHWVertex && m_pMeshNode->m_pVb->m_hVB != -1);
		if (bNeedLock)
			m_pMeshNode->m_pVb->Lock();

		char* arrVertex	= m_pMeshNode->m_pVb->m_v;

		int cursorPos = m_pMeshNode->m_pVb->m_nPosOffset;
		int cursorNormal = m_pMeshNode->m_pVb->m_nNormalOffset;

		_ASSERT( getClothInfo()->getTheNumberOfParticles() == m_pMeshNode->m_pVb->m_nVertexCnt);

		RVector temp = RVector(1,0,0);
		for(int i = 0; i < m_pMeshNode->m_pVb->m_nVertexCnt; ++i)
		{
			memcpy( &arrVertex[cursorPos], &pParticle[i].p, sizeof(RVector) );
			memcpy( &arrVertex[cursorNormal], &pParticle[i].n, sizeof(RVector) );

			cursorPos += m_pMeshNode->m_pVb->m_nVertexSize;
			cursorNormal += m_pMeshNode->m_pVb->m_nVertexSize;
		}

		if (bNeedLock)
			m_pMeshNode->m_pVb->Unlock();

		RActorNode::SetVertexStream( b2ndBufferMng);
	}

	void RClothActorNode::simulateByFixedTime( void )
	{
		static const float TIME_TO_SIMULATION = 0.032f;	//30프레임일시 프레임당 걸리는 시간(초)

		DWORD dwDuration = REngine::GetDevice().GetLastElapsedTime();
		float duration = dwDuration / 1000.0f;
		float timeToSimulate = getClothInfo()->getParameter()->getTimeToSimulate();	_ASSERT(timeToSimulate > 0.0f );
		float accumulatedDuration = 0.0f;

		if( duration <= TIME_TO_SIMULATION )
		{
			simulate( duration );
		}
		else //duration > timeToSimulate
		{
			int nMaxIter = 8;
			int nIter = 0;
			while(true)
			{
				if( accumulatedDuration > duration)	break;
				if( nIter++ >= nMaxIter )
					break;
				simulate( TIME_TO_SIMULATION );
				accumulatedDuration+=TIME_TO_SIMULATION;
			}
		}
	}

	void RClothActorNode::RenderPrimitive(int index)
	{
		_ASSERT(false);
	}







	void RClothActorNode::calculateNormals( void )
	{

		RClothParticle* pParticle = getClothInfo()->getParticles();
		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			pParticle[i].n = RVector::ZERO;
		}

		int theNumberOfTriangles = m_pMeshNode->m_nFaceIndexTableCnt/3;
		for( int i = 0; i < theNumberOfTriangles; i++ )
		{
			
			WORD index0 = m_pMeshNode->m_pFaceIndexTable[i*3+0];
			WORD index1 = m_pMeshNode->m_pFaceIndexTable[i*3+1];
			WORD index2 = m_pMeshNode->m_pFaceIndexTable[i*3+2];
			
			const RVector &p0 = pParticle[index0].p;
			const RVector &p1 = pParticle[index1].p;
			const RVector &p2 = pParticle[index2].p;

			RVector p01 = p1 - p0;
			RVector p02 = p2 - p0;
			RVector n   = p01.CrossProduct(p02);
			n.Normalize();

			pParticle[index0].n += n;			
			pParticle[index1].n += n;
			pParticle[index2].n += n;
		}	

		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			pParticle[i].n.Normalize();
		}
	}

	void RClothActorNode::applyWind( RVector *pWindVector )
	{
		RClothParticle* pParticle = getClothInfo()->getParticles();
		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			pParticle[i].n = RVector::ZERO;
		}	

		int theNumberOfTriangles = m_pMeshNode->m_nFaceIndexTableCnt/3;
		for( int i = 0; i < theNumberOfTriangles; i++ )
		{
			WORD index0 = m_pMeshNode->m_pFaceIndexTable[i*3+0];
			WORD index1 = m_pMeshNode->m_pFaceIndexTable[i*3+1];
			WORD index2 = m_pMeshNode->m_pFaceIndexTable[i*3+2];

			const RVector &p0 = pParticle[index0].p;
			const RVector &p1 = pParticle[index1].p;
			const RVector &p2 = pParticle[index2].p;

			RVector p01 = p1 - p0;
			RVector p02 = p2 - p0;
			RVector n   = p01.CrossProduct(p02);
			n.Normalize();

			pParticle[index0].v += CrossProduct(n,n.DotProduct(*pWindVector));
			pParticle[index1].v += CrossProduct(n,n.DotProduct(*pWindVector));
			pParticle[index2].v += CrossProduct(n,n.DotProduct(*pWindVector));

		}	




	}

	void RClothActorNode::getClothWorldTransformMatrix( RMatrix *pWorldTransformMatrix )
	{
		RPFC_THISFUNC;

		if( true == isAttachedToBone() )
		{
			RActorNode *pBone = getBoneForWorldTransform();
			//옷의로컬 * 옷에연결된 Bone을 원점으로 옮기는 변환 * 옷에연결된 Bone을 애니메이션 되게 하는 변환
			(*pWorldTransformMatrix) = m_pMeshNode->m_matBase * pBone->m_pMeshNode->m_matInvBase * pBone->GetResultTransform() * m_pActor->GetWorldTransform();
		}
		else
		{
			(*pWorldTransformMatrix) = m_pParent->GetWorldTransform() * m_pActor->GetWorldTransform();
		}
	}

	void RClothActorNode::getClothInvWorldTransfromMatrix( RMatrix *pInvWorldTransformMatrix )
	{
		RMatrix worldTransformMatrix = RMatrix::IDENTITY;
		getClothWorldTransformMatrix(&worldTransformMatrix);
		worldTransformMatrix.GetInverse(pInvWorldTransformMatrix);
	}

	void RClothActorNode::makeSortedCollision( void )
	{
		typedef multimap<float,int>	SortedMap;
		typedef SortedMap::reverse_iterator riterator;
		SortedMap sm;

		for(unsigned int i = 0; i < m_pActor->m_pCollision->m_vCollisions.size(); ++i)
		{
			RActorCollisionObject& CollisionObejct = m_pActor->m_pCollision->m_vCollisions[i];
			RMatrix matWorldForCollisionObject = CollisionObejct.m_matTransform * CollisionObejct.m_pParent->GetWorldTransform();


			switch(CollisionObejct.m_Type)
			{
				case MCollision::CAPSULE:
				{
					RVector3 position(0,0,CollisionObejct.m_Capsule.bottom.z + CollisionObejct.m_Capsule.height);
					matWorldForCollisionObject.TransformVect(position);
					sm.insert( make_pair(position.z,i));
				}break;

				case MCollision::SPHERE:
				{
					RVector3 position(0,0,CollisionObejct.m_Sphere.center.z + CollisionObejct.m_Sphere.radius);
					matWorldForCollisionObject.TransformVect(position);
					sm.insert( make_pair(CollisionObejct.m_Sphere.center.z + CollisionObejct.m_Sphere.radius,i));
				}break;
			}
		}

		_ASSERT( m_pActor->m_pCollision->m_vCollisions.size() == sm.size() );

		for(riterator riter = sm.rbegin(); riter != sm.rend() ; riter ++)
		{
			m_vSortedCollisionIndices.push_back(riter->second);
		}
	}

	void RClothActorNode::AABB2OBB( RBox& aabb,ROBB& obb )
	{
		obb.vPosition = (aabb.vmax + aabb.vmin) * 0.5f;
		obb.vAxis[0]	= MVector3(1,0,0);
		obb.vAxis[1]	= MVector3(0,1,0);
		obb.vAxis[2]	= MVector3(0,0,1);
		obb.vRange[0] = (aabb.maxx - aabb.minx) * 0.5f;
		obb.vRange[1] = (aabb.maxy - aabb.miny) * 0.5f;
		obb.vRange[2] = (aabb.maxz - aabb.minz) * 0.5f;
	}

	RClothActorNode* RClothActorNode::Create( std::string meshName,std::string fileName )
	{
		RClothActorNode* pMeshNode = NULL;

		bool bWorldSimulation = true;
		RClothInfo::isWorldSimulation(fileName.c_str(),&bWorldSimulation);
		if( true == bWorldSimulation)
		{
			pMeshNode = new RClothWorldActorNode;
		}
		else
		{
			pMeshNode = new RClothLocalActorNode;
		}

		pMeshNode->setFileName(fileName);

		return pMeshNode;
	}

	void RClothActorNode::RenderNormal()
	{
		RDebugPassRenderer* pDebugPassRenderer = REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>();
		_ASSERT( pDebugPassRenderer );

		float fSize = 10.f;
		RClothParticle* pParticle = getClothInfo()->getParticles();
		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			pDebugPassRenderer->AddDebugLine( pParticle[i].p, pParticle[i].p + fSize * pParticle[i].n, 0.001f, 0xffffff00 );
		}
	}

	string RClothActorNode::getBoneNameForWorldTransformFromProperty( void )
	{
		int nUserPropertySize = m_pMeshNode->GetUserPropertyCount();

		// 프로퍼티로부터 정보 모으기
		string boneName;
		size_t pos;

		for (int i = 0; i < nUserPropertySize; ++i)
		{

			const string& propertyString = m_pMeshNode->GetUserPropertyString(i);

			pos = propertyString.find("set_cloth_anchor_name = ");
			if (pos == string::npos) continue;

			pos = strlen("set_cloth_anchor_name = ");
			boneName.append(propertyString,pos,propertyString.size()-pos);
		}

		_ASSERT( "" != boneName );
		return boneName;
	}

	RActorNode* RClothActorNode::getBoneForWorldTransform( void )
	{
		
		_ASSERT( NULL != m_pBoneForWorldTransform );
		return m_pBoneForWorldTransform;
	}

	RActorNode* RClothActorNode::getBoneForWorldTransformFromBoneName( const string& boneName )
	{
		RActorNode* pBoneForWorldTM = NULL;

		for( unsigned int i=0; i<m_refBones.size(); i++)
		{
			pBoneForWorldTM = m_refBones[i];
			if( pBoneForWorldTM->m_strName == boneName )	return pBoneForWorldTM;
		}

		//_ASSERT(false);
		return (RActorNode*)m_pParent;
	}

	// < return값 >
	// 0 : 유효한 모델링 데이타
	// 1 : m_pMeshNode가 NULL
	// 2 : 참조하는 본 갯수가 0개
	// 3 : 정점의 칼라값이 없다
	// 4 : 정점의 칼라테이블이 NULL
	// 5 : 참조할 수 있는 ActorNode가 없다.
	// 6 : 포인트갯수가 0개
	int RClothActorNode::isValidModeling( void )
	{
		if( NULL	== m_pMeshNode )							return 1;
		//if( 0		== m_pMeshNode->m_nBoneTableCnt )			return 2;
		if( 0		== m_pMeshNode->m_nPointColorCnt )			return 3;
		if( NULL	== m_pMeshNode->m_pPointColorTable )		return 4;
		//if( NULL    == getBoneForWorldTransformFromBoneName(getBoneNameForWorldTransformFromProperty() ) )	return 5;
		if( 0		== m_pMeshNode->m_nPointCnt	)				return 6;
		if( NULL	== m_pMeshNode->m_pPointTable )				return 7;
		if( NULL	== m_pMeshNode->m_pPointTableOrg )			return 8;


		return 0;
	}

	void RClothActorNode::getLastTranslation( RVector *pLastTranslation )
	{
		_ASSERT(NULL != pLastTranslation);
		RMatrix	invBeforeClothWorldMatrix;	m_beforeClothWorldMatrix.GetInverse(&invBeforeClothWorldMatrix);
		RMatrix	lastClothWorldMatrix = invBeforeClothWorldMatrix *  m_currentClothWorldMatrix;
		*pLastTranslation = lastClothWorldMatrix.GetTranslation();
	}

	bool RClothActorNode::isAttachedToBone( void )
	{
		if( 0 == m_pMeshNode->m_nBoneTableCnt )	return false;
		return true;
	}



}