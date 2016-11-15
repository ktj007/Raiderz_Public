#include "stdafx.h"

#include "RPhysXClothActorNode.h"
#include "RActor.h"
#include "RVType.h"
#include "RMeshNode.h"
#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"
#include "RShaderCompositeManager.h"
#include "RShaderComposite.h"
#include "REnvironmentSet.h"
#include "RCameraSceneNode.h"

#include "RRenderHelper.h"
#include "RPhysXClothController.h"

namespace rs3 {

RPhysXClothActorNode::RPhysXClothActorNode(NxPhysicsSDK* pSDK, NxScene* pScene, NxCookingInterface* pCooking)
: RActorNode(ACTOR_NODE_CLOTH)
{
	m_pSDK			= pSDK;
	m_pScene		= pScene;
	m_pCooking		= pCooking;

	m_pController	= NULL;

	m_eClothState	= E_NONE;

	ClearNxClothMeshData();
	m_ClothCPUMeshData.Init(this);
}

RPhysXClothActorNode::~RPhysXClothActorNode()
{
}

void RPhysXClothActorNode::ConnectClothMeshData( NxMeshData& rMeshData )
{
	m_NxClothMeshData = rMeshData;
}

void RPhysXClothActorNode::ChangeClothState( E_CLOTH_STATE _eClothState )
{
	m_eClothState = _eClothState;

	switch(m_eClothState)
	{
	case E_NONE:
		ClearNxClothMeshData();
		break;

	case E_CPU:
		ClearNxClothMeshData();
		UpdateTransformAndVertex();
		break;

	case E_PHYSX:
		_ASSERT(GetNxClothVertexPosBegin() != NULL);
	    break;
	}
}

void RPhysXClothActorNode::OnDestroy()
{
	_ASSERT(m_pController != NULL);

	// cloth 지우기
	m_pController->RemoveClothNode(GetNodeName());

	// 컨트롤러 관리
	int nRefCount = m_pController->Drop();
	_ASSERT(nRefCount >= 0);
	if ( 0 == nRefCount)
	{
		m_pActor->RemoveController(m_pController);
		delete m_pController;
	}
	m_pController = NULL;

	// RActorNode
	RActorNode::OnDestroy();
}

void RPhysXClothActorNode::OnAdd()
{
	RActorNode::OnAdd();

	// 업데이트 컨트롤러 관리
	_ASSERT(m_pController == NULL);

	// 이미 액터에 등록된 컨트롤러가 있는지 확인
	m_pController = static_cast<RPhysXClothController*>( m_pActor->GetFirstControllerByName( RPhysXClothController::CONTROLLER_TYPE(), RPhysXClothController::CONTROLLER_NAME() ) );

	// 등록된 컨트롤러가 없을경우
	if (NULL == m_pController)
	{
		// 생성을 씬매니저에 요청
		RSceneNodeController* pController = REngine::GetSceneManager().CreateSceneNodeController( RPhysXClothController::CONTROLLER_NAME() );

		// 씬매니저에서 생성할수 있는 컨트롤러가 없을경우
		if (pController == NULL)
		{
			pController = new RPhysXClothController(m_pSDK, m_pScene, m_pCooking);
		}

		// 액터 등록
		m_pController =  static_cast<RPhysXClothController*>(pController);
		m_pActor->AddController(m_pController);
	}

	m_pController->AddClothNode(GetNodeName());
	m_pController->Grap();
}

void RPhysXClothActorNode::UpdateTransformAndVertex()
{
	if (E_CPU == m_eClothState)
		m_ClothCPUMeshData.UpdateVertex();
}

void RPhysXClothActorNode::SetVertexStream()
{
	if (E_NONE == m_eClothState)
		return;

	if (E_PHYSX == m_eClothState)
	{
		RVector* arrReceivePosition	= GetNxClothVertexPosBegin();
		RVector* arrReceiveNormal	= GetNxClothVertexNormalBegin();

		bool bNeedLock = (m_pMeshNode->m_pVb->m_bUseHWVertex && m_pMeshNode->m_pVb->m_hVB != -1);
		if (bNeedLock)
			m_pMeshNode->m_pVb->Lock();

		char* arrVertex	= m_pMeshNode->m_pVb->m_v;

		int cursorPos = m_pMeshNode->m_pVb->m_nPosOffset;
		int cursorNormal = m_pMeshNode->m_pVb->m_nNormalOffset;

		for(int i = 0; i < m_pMeshNode->m_pVb->m_nVertexCnt; ++i)
		{
			memcpy( &arrVertex[cursorPos], &arrReceivePosition[i], sizeof(RVector) );
			memcpy( &arrVertex[cursorNormal], &arrReceiveNormal[i], sizeof(RVector) );

			cursorPos += m_pMeshNode->m_pVb->m_nVertexSize;
			cursorNormal += m_pMeshNode->m_pVb->m_nVertexSize;
		}

		if (bNeedLock)
			m_pMeshNode->m_pVb->Unlock();
	}

	RActorNode::SetVertexStream();
}

#ifdef INFERRED_WORK
void RPhysXClothActorNode::SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial, bool bFogEnable, bool bBlurAlphaForce, bool bSetAboutLight)
{
	if (E_NONE == m_eClothState)
		return;

	RActorNode::SetNodeRenderState( pCurrentCamera, pMaterial, bFogEnable, bBlurAlphaForce, bSetAboutLight);
	SetNodeRenderStateForCloth( pCurrentCamera, pMaterial );
}
#else
void RPhysXClothActorNode::SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial, bool bFogEnable, bool bBlurAlphaForce)
{
	if (E_NONE == m_eClothState)
		return;

	RActorNode::SetNodeRenderState( pCurrentCamera, pMaterial, bFogEnable, bBlurAlphaForce);
	SetNodeRenderStateForCloth( pCurrentCamera, pMaterial );
}
#endif


void RPhysXClothActorNode::SetNodeRenderStateForDepth( RCameraSceneNode* pCurrentCamera,  RMaterial* pMaterial )
{
	RActorNode::SetNodeRenderStateForDepth( pCurrentCamera, pMaterial );
	SetNodeRenderStateForCloth( pCurrentCamera, pMaterial );
}

void RPhysXClothActorNode::SetNodeRenderStateForCloth(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial )
{
	// 이미 월드 기준으로 시뮬레이션이 된 상태이므로 그대로 출력하면 된다.
	RMatrix matIdentity = RMatrix::IDENTITY;//m_pActor->GetWorldTransform();
	RShaderCompositeManager::m_cWorld.SetMatrix(matIdentity);
	RShaderCompositeManager::m_cWorldViewProj.SetMatrix( /*matWorld * */pCurrentCamera->GetViewProjectionMatrix() );
	RShaderCompositeManager::m_cWorldView.SetMatrix( /*matWorld **/ pCurrentCamera->GetViewMatrix() );
	RShaderCompositeManager::m_cTextureFactor.SetVector4( /*m_pActor->m_vMultiplyColor */ RVector4(1,1,1,1) );
	REngine::GetDevice().SetAlphaTestEnable(false);

	if (!m_matBones.empty())
		RShaderCompositeManager::m_cBlendMatrices.SetMatrixArray(&matIdentity/*m_matBones[0]*/,m_pMeshNode->m_nBoneTableCnt);

	//if (!m_matBonesPrev.empty())
	//	RShaderCompositeManager::m_cBlendMatricesPrev.SetMatrixArray(&matIdentity/*m_matBones[0]*/,m_pMeshNode->m_nBoneTableCnt);	// 090629, OZ
}

void RPhysXClothActorNode::RenderPrimitive(int index)
{
	if (E_NONE == m_eClothState)
		return;

	RActorNode::RenderPrimitive(index);
	REngine::GetProfileInfo().AddPolygon(PPT_ACTORPHYSIQUE, m_pMeshNode->m_pMtrlTableInfo[index].count );
}

void RPhysXClothActorNode::RCPUClothMeshData::UpdateVertex()
{
	if (NULL == m_pOwnerClothActorNode)
		return;

	RMeshNode* pMeshNode = m_pOwnerClothActorNode->m_pMeshNode;
	RMesh* pParentMesh = pMeshNode->m_pParentMesh;
	int nVertCount = pMeshNode->m_nPointCnt;
	int nCntFaceIndex = pMeshNode->m_nFaceIndexTableCnt; // == m_nCntFace * 2 * 3;	// 하나의 폴리곤(면)은 2개의 삼각형으로 이루어지고 삼각형의 정점은 3개이다.

	if (NULL == m_pVertPos)
		m_pVertPos = new RVector[nVertCount];

	if (NULL == m_pVertNorm)
		m_pVertNorm = new RVector[nVertCount];

	//////////////////////////////////////////////////////////////////////////
	//
	// from RCharCloth
	// position update and normal update
	//
	//////////////////////////////////////////////////////////////////////////

	int		nRefBone;
	int		nCid;
	float	fWeight;
	RMatrix mat;
	RVector vResult;
	RVector vCurPos;

	RMatrix _matBone[60];// Mesh에서 참조하는 Bone의 최대 개수
	// Bone TM은 REFERENCE 모델의 TM 이다. 변환 과정이 필요함.
	for(int i = 0; i < pMeshNode->m_nBoneTableCnt; ++i)
	{
		RActorNode* pBoneNode = m_pOwnerClothActorNode->m_refBones[i];

		const RMatrix& matResult = pBoneNode->GetResultTransform();

		// Bone World TM
		//				RMatrix matResult = m_refBones[i]->GetResultTransform();
		// 		RMatrix matResult = m_pActorNode->m_refBones[i]->GetWorldTransform()
		// 			* m_pActorNode->m_pActor->GetWorldInverseTransform();// * m_pActorNode->m_pActor->GetWorldInverseTransform();

		// Base는 REFRENCE 모델의 TM을 뜻한다.
		// MeshNode Base * Bone Inverse Base // m_pMeshNode->m_pBoneTable[iBone] * 
		_matBone[i] = pMeshNode->m_matBase * pBoneNode->m_pMeshNode->m_matInvBase * matResult;
	}

	for( int i = 0; i < nVertCount; ++i )
	{
		vResult.Set( 0, 0, 0 );
		
//		if( pMeshNode->isHWPhysique() )
		// isPhysique 로 컴파일이 안되서 고쳐놓습니다. - dub
		if( pMeshNode->isPhysique() )
		{
			nRefBone = pMeshNode->m_pPhysique[i].m_num;

			for( int j = 0; j < nRefBone; ++j )
			{
				nCid	= pMeshNode->m_pPhysique[i].GetBoneID(j);
				fWeight	= pMeshNode->m_pPhysique[i].GetWeight(j);

				vCurPos	= pMeshNode->m_pPointTableOrg[i] * _matBone[nCid];
				vResult	+= vCurPos * fWeight;
			}

			vResult = vResult * m_pOwnerClothActorNode->m_pActor->GetWorldTransform();
		}
		else
		{
			RMatrix matWorld = m_pOwnerClothActorNode->GetWorldTransform();
			vResult = pMeshNode->m_pPointTableOrg[i] * matWorld;
		}
		m_pVertPos[i].Set(vResult);
	}

	// normal update
	ZeroMemory( m_pVertNorm, sizeof(RVector)*nVertCount );

	int	nIndexVertex;
	RVertexIndex* pVertexIndexTable;
	RVector vPoint[3];
	for( int i = 0; i < nCntFaceIndex / 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			pVertexIndexTable = &pMeshNode->m_pVertexIndexTable[ pMeshNode->m_pFaceIndexTable[i * 3 + j] ];
			nIndexVertex = pVertexIndexTable->p;

			_ASSERT( nIndexVertex < nVertCount );
			_ASSERT( nIndexVertex >= 0 );

			vPoint[j]		= m_pVertPos[nIndexVertex];
		}

		RVector vNormal( (vPoint[2] - vPoint[0]).CrossProduct(vPoint[2] - vPoint[1]) );

		for( int j = 0; j < 3; ++j )
		{
			pVertexIndexTable = &pMeshNode->m_pVertexIndexTable[ pMeshNode->m_pFaceIndexTable[i * 3 + j] ];
			nIndexVertex = pVertexIndexTable->n;

			m_pVertNorm[nIndexVertex] += vNormal;
		}
	}

	for( int i = 0; i < nVertCount; ++i )
		m_pVertNorm[i].Normalize();

	// vertex buffer update 
	BYTE* pVertex = (BYTE*)REngine::GetDevice().LockVertexBuffer(pMeshNode->m_pVb->m_hVB);
	_ASSERT(pVertex);
	const int nVertexSize = pMeshNode->m_pVb->m_nVertexSize;
	for( int i = 0; i < pMeshNode->m_nVertexIndexTableCnt; ++i )
	{
		// pos 갱신
		RVector* pPosition = (RVector*) ( pVertex + pMeshNode->m_pVb->m_nPosOffset );
		*pPosition = m_pVertPos[pMeshNode->m_pVertexIndexTable[ i ].p];

		// normal 
		RVector* pNormal = (RVector*) ( pVertex + pMeshNode->m_pVb->m_nNormalOffset );
		*pNormal = m_pVertNorm[ pMeshNode->m_pVertexIndexTable[ i ].n ];

		pVertex += nVertexSize;
	}

	REngine::GetDevice().UnlockVertexBuffer(pMeshNode->m_pVb->m_hVB);
}

}
