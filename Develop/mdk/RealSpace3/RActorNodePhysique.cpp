#include "StdAfx.h"
#include "RActorNodePhysique.h"
#include "RMeshNode.h"
#include "RMesh.h"
#include "RActor.h"
#include "RProfileInfo.h"
#include "RShaderCompositeManager.h"


namespace rs3 {

MImplementRTTI(RActorNodePhysique, RActorNode);
MImplementRTTI(RActorNodePhysiqueGPU, RActorNodePhysique);

RActorNodePhysique::RActorNodePhysique(ACTOR_NODE_TYPE eActorNodeType = ACTOR_NODE_PHYSIQUE) : RActorNode(eActorNodeType)
{
	m_baseComponents = RSC_POSITION | RSC_NORMAL | RSC_PHYSIQUE;

	// 피직은 아직 인스턴싱 지원을 하지 않는다. 텍스쳐 기입 인스턴싱을 작업하면 그때 열어주자.
	m_bCanInstancing = false;

	//m_bResetVertexAgainstMatrix = false;
}

RActorNodePhysique::~RActorNodePhysique(void)
{
}

void RActorNodePhysique::UpdateTransformAndVertex()
{
	RPFC_THISFUNC;
	// 한번 수행 한 적이 있는지 없는지 체크 하므로 매번 수행하도 상관 없을 듯..
	//ResetVertexAgainstMatrix();

	// 뼈대 매트릭스 등록.
	const int MAX_BONE_COUNT = 54;
	_ASSERT(m_refBones.size()<=MAX_BONE_COUNT);	// 동시에 참조되는 뼈의 개수 제한

	// 한 프레임 안에서 로직상에 의해 갱신을 여러번 할 수도 있다. 갱신 시간을 비교하여 같으면 안하도록 하자.
	if( m_nLastUpdatedFrame == REngine::GetDevice().GetFrameCount()) 
		return;
	m_nLastUpdatedFrame = REngine::GetDevice().GetFrameCount();

	if( IsVertexMotionBlur() || IsPixelMotionBlur())
	{
#ifdef USING_RSC_PHYSIQUE_QT
		// 이전 프레임을 저장 
		m_vBoneTransPrev = m_vBoneTrans;
		m_quatBoneRotsPrev = m_quatBoneRots;
#endif
	}

	for(size_t i=0;i<m_refBones.size();i++) 
	{
		RActorNode* pBoneNode = m_refBones[i];

		// 쿼터니언과 이동값이 셋팅 되어 있는 놈은 그걸 쓴다. 
		if( pBoneNode->IsUseForPhysiqueBone())
		{
#ifdef USING_RSC_PHYSIQUE_QT
			m_quatBoneRots[i] = m_quatBoneBaseRots[i] * pBoneNode->GetResultRotation();	// <- It's OK!!!
			RVector4 vResultTran = RVector4( pBoneNode->GetResultTranslation().x, pBoneNode->GetResultTranslation().y, pBoneNode->GetResultTranslation().z, 1);
			// V = ( PR * VT * S) + PT
			m_vBoneTrans[i] = ( pBoneNode->GetResultRotation() * (m_vBoneBaseTrans[i] * pBoneNode->GetResultScale().z)) + vResultTran;

#endif
		}
 		else
		{
			const RMatrix& matResult = pBoneNode->GetResultTransform();

			m_matBoneBases[i].MultiplyTo(matResult,m_matBones[i]);

			/* 나의 base * 뼈 inv base * 애니메이션 결과
			m_matBones[i] = m_pMeshNode->m_matBase  
			* pBoneNode->m_pMeshNode->m_matInvBase
			* matResult;

			이중 m_pMeshNode->m_matBase * pBoneNode->m_pMeshNode->m_matInvBase 을
			m_matBoneBases[i] 에다가 미리 계산해 놓았다

			*/
			
#ifdef USING_RSC_PHYSIQUE_QT
			// 쿼터니언을 뽑아 내는 것으로... 
			// 이게 잘 되면 윗줄은 꺼져
			RQuaternion qBoneRot;
			RVector3 vBoneScale;
			RVector4 vBoneTran;

			// 쿼터니언 이동 값 계산 시스템이 완벽히 갖추어 지지 않았다.
			// 결과 본에서 데이터를 추출하자..
			//m_matBones[i].Decompose( vBoneScale,  m_vBoneTrans[i], m_quatBoneRots[i]);
			// 위와 같이 최종 매트릭스에서 분해를 하니 팔 빠꾸 빠져나오던 현상 또 생겼다.

			// 최종 매트릭스에서 뽑지 말고 직접 계산을 해보자
			matResult.Decompose( vBoneScale,  vBoneTran, qBoneRot);
			m_quatBoneRots[i] = m_quatBoneBaseRots[i] * qBoneRot;	// <- It's OK!!!
			RVector4 vTran = RVector4( vBoneTran.x, vBoneTran.y, vBoneTran.z, 1);
			// V = ( PR * VT * S) + PT
			// 스케일링은 m_fSBasecale를 쓰는 것이 아니라, matResult의 스케일링 기준이 되어야 한다.
			// 최종 = 베이스 * 리절트 이므로 베이스의 이동 값에 리절트의 스케일링을 적용 시켜야 정상. 
			// 스케일링은 z값을 가지고 사용.
			m_vBoneTrans[i] = ( qBoneRot * (m_vBoneBaseTrans[i] * vBoneScale.z)) + vTran;
			// 스케일링 값은 xyz중 하나만 넘겨야 되는데 만만한 z를 주자
			// w값에다가 스케일링 정보 새기자
			m_vBoneTrans[i].w = vBoneScale.x;

			// 이전 본 정보를 그대로 사용 하는 것이 아니라 프레임 레이트에 따라 고정하게 
			//float fSlerp = REngine::GetDevice().GetFrameRate() / 30.0f;
			//float fSlerpDecimal = fSlerp;
			//if( fSlerp > 1)
			//	fSlerpDecimal = fSlerp - (float)((int)fSlerp);
			// to : prev  from : curr
			//*this = to-from; // delta on all 3 axis
			//*this*=slerp;    // times interpolant distance.
			//*this+=from;     // plus source
			// 우선 애니메이션은 보간을 하지 않아본다... 
			//m_vBoneTrans[i].InterpolateTo( m_vBoneTransPrev[i], fSlerp, m_vBoneTransPrev[i]);
			//m_quatBoneRotsPrev[i] = m_quatBoneRotsPrev[i].Slerp( m_quatBoneRots[i], m_quatBoneRotsPrev[i], fSlerpDecimal);
			// 위 두줄은 차후 고려..
#else		
 #ifdef USING_RSC_PHYSIQUE_DQ
			// 쿼터니언과 이동 값을 추출 
			//m_matBones[i].Decompose( m_vBoneScales[i],  m_vBoneTrans[i], m_quatBoneRots[i]);
			// 듀얼 쿼터니언 값 설정 
			//m_dqBoneDQ[i] = RDQuaternion( m_quatBoneRots[i], m_vBoneTrans[i]);
 #endif
#endif
		}
	}

}


int RActorNodePhysique::UpdatePoint_SoftPhysiqueAnimation()
{
	RPFC_THISFUNC;

	// 한프레임에 두번 업데이트를 막는다
	if(m_nLastUpdatedFrame==REngine::GetDevice().GetFrameCount()) 
		return 1;

	m_nLastUpdatedFrame = REngine::GetDevice().GetFrameCount();


	int p_num,i,j,c_id;
	RVector _vec_all,_vec;
	float weight;
	RMatrix t_mat,local,basemat,inv;

	for(i=0;i<m_pMeshNode->m_nPhysiqueCnt;i++) { // point_num

		_vec_all = RVector(0,0,0);

		p_num = m_pMeshNode->m_pPhysique[i].m_num;

		//		_ASSERT(p_num<5);
		//		if(p_num > MAX_PHYSIQUE_KEY) p_num = MAX_PHYSIQUE_KEY;

		for(j=0;j<p_num;j++) {// 1 - 3

			c_id	= m_pMeshNode->m_pPhysique[i].GetBoneID(j);
			weight	= m_pMeshNode->m_pPhysique[i].GetWeight(j);

			_vec = m_pMeshNode->m_pPointTableOrg[i] * m_matBones[c_id];

			_vec_all += _vec * weight;
		}

		//		_ASSERT(_vec_all.Length()>0.01);

		m_pMeshNode->m_pPointTable[i] = _vec_all;
	}


	/////// normal update 테스트
	if(!m_pMeshNode->m_pPointNormalTable) return 1;

	for(int i=0;i< m_pMeshNode->m_nVertexIndexTableCnt;i++ ) {

		int nPosIndex = m_pMeshNode->m_pVertexIndexTable[i].p;

		RVector n   = m_pMeshNode->m_pNormalTable[m_pMeshNode->m_pVertexIndexTable[i].n];

		_vec_all = RVector(0,0,0);
		p_num = m_pMeshNode->m_pPhysique[nPosIndex].m_num;
//		if(p_num > MAX_PHYSIQUE_KEY) p_num = MAX_PHYSIQUE_KEY;

		RVector trNormal[20];
		for(j=0;j<p_num;j++) {// 1 - 3

			c_id	= m_pMeshNode->m_pPhysique[nPosIndex].GetBoneID(j);
			weight	= m_pMeshNode->m_pPhysique[nPosIndex].GetWeight(j);

			m_matBones[c_id].TransformNormal(n,trNormal[j]);
			_vec_all += trNormal[j] * weight;
		}

		m_pMeshNode->m_pPointNormalTable[i] = _vec_all;
	}

	return 1;
}

void RActorNodePhysique::RenderNormal(DWORD dwColor)
{
	UpdatePoint_SoftPhysiqueAnimation();
	RActorNode::RenderNormal(dwColor);
}

void RActorNodePhysique::RenderWire(DWORD dwColor)
{
	UpdatePoint_SoftPhysiqueAnimation();
	RActorNode::RenderWire(dwColor);
}

bool RActorNodePhysique::OnPick(RPICKINFO& pickInfo)
{
	// detail 일때만 polygon picking 이 가능하다.
	if(pickInfo.inMethod!=RPM_DETAIL) return false;

	RMatrix matWorld = m_pActor->GetWorldTransform();
	RMatrix invWorld;
	matWorld.GetInverse(&invWorld);

	RVector localOrigin = pickInfo.inPickOrigin * invWorld;
	RVector localTo = pickInfo.inPickTo * invWorld;

	UpdatePoint_SoftPhysiqueAnimation();

	if(PickLocal(localOrigin,localTo,&pickInfo))
	{
		pickInfo.outPosition = pickInfo.outPosition * matWorld;
		return true;
	}
	return false;
}

void RActorNodePhysique::ResetTransform()
{
//	RActorNode::ResetTransform();
	m_matLocal->MakeIdentity();
//	m_matResult.MakeIdentity();
	m_fVisibility = m_pMeshNode->m_fBaseVisibility;
	m_fNoiseRef = m_pMeshNode->m_fBaseNoiseRef;
}

void RActorNodePhysique::RenderPrimitive(int index)
{
	RPFC_THISFUNC;

	UpdatePoint_SoftPhysiqueAnimation();
	m_pMeshNode->UpdateNodeBuffer( ); // m_pPointTable 에 만들어진걸 m_pVert 로 옮긴다...

	m_pMeshNode->m_pVb->RenderIndexSoft( 
		&m_pMeshNode->m_pIb->m_pIndex[m_pMeshNode->m_pMtrlTableInfo[index].offset],
		m_pMeshNode->m_pMtrlTableInfo[index].count,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	REngine::GetResourceProfiler().AddPolygon(PPT_ACTORPHYSIQUE, m_pMeshNode->m_pMtrlTableInfo[index].count );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTORPHYSIQUE, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, index);
}

void RActorNodePhysique::RenderAllNodePrimitive()
{
	RPFC_THISFUNC;
	UpdatePoint_SoftPhysiqueAnimation();
	m_pMeshNode->UpdateNodeBuffer( ); // m_pPointTable 에 만들어진걸 m_pVert 로 옮긴다...

	m_pMeshNode->m_pVb->RenderIndexSoft( 
		&m_pMeshNode->m_pIb->m_pIndex[0],
		m_pMeshNode->m_nFaceCnt,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	REngine::GetResourceProfiler().AddPolygon(PPT_ACTORPHYSIQUE, m_pMeshNode->m_nFaceCnt );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTORPHYSIQUE, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, -1);
}

bool RActorNodePhysique::Validate()
{
	for(unsigned int i=0;i<m_refBones.size();i++)
	{
		if(m_refBones[i]==NULL) return false;
	}
	return RActorNode::Validate();
}


//////////////////////////////////////////////////////////////////////////
// RActorNodePhysiqueGPU

RActorNodePhysiqueGPU::RActorNodePhysiqueGPU(void)
{
}

RActorNodePhysiqueGPU::~RActorNodePhysiqueGPU(void)
{
}

void RActorNodePhysiqueGPU::SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj)
{
	RPFC_THISFUNC;

	RShaderCompositeManager::SetTransformConstant(m_pActor->GetWorldTransform(), matView, matViewProj);

#ifdef USING_RSC_PHYSIQUE_DQ	
	if(!m_dqBoneDQ.empty())
	{
		RShaderCompositeManager::m_cBlendDQ.SetDQuaternionArray( &m_dqBoneDQ[0],m_pMeshNode->m_nBoneTableCnt);
	}
#else
 #ifdef USING_RSC_PHYSIQUE_QT
	if(!m_quatBoneRots.empty())
	{
		// 쿼터니언과 사이즈는 같으므로 Vector4 array로 넘겨줘보자.. - 097022, OZ
		// 쿼터니언을 SetValue로 넘기면 가끔 뻗는데 Vector4로 넘기면 안뻗네... 거참..
		//RShaderCompositeManager::m_cBlendQuats.SetQuaternionArray( &m_quatBoneRots[0],m_pMeshNode->m_nBoneTableCnt);
		RShaderCompositeManager::m_cBlendQuats.SetVector4Array( (RVector4*)&m_quatBoneRots[0],m_pMeshNode->m_nBoneTableCnt);
	}

	if(!m_vBoneTrans.empty())
	{
		RShaderCompositeManager::m_cBlendTrans.SetVector4Array( &m_vBoneTrans[0],m_pMeshNode->m_nBoneTableCnt);
	}

	if( IsVertexMotionBlur() || IsPixelMotionBlur())
	{
		if(!m_quatBoneRotsPrev.empty())
		{
			// 쿼터니언과 사이즈는 같으므로 Vector4 array로 넘겨줘보자.. - 097022, OZ
			// 쿼터니언을 SetValue로 넘기면 가끔 뻗는데 Vector4로 넘기면 안뻗네... 거참..
			//RShaderCompositeManager::m_cBlendQuatsPrev.SetQuaternionArray( &m_quatBoneRotsPrev[0],m_pMeshNode->m_nBoneTableCnt);
			RShaderCompositeManager::m_cBlendQuatsPrev.SetVector4Array( (RVector4*)&m_quatBoneRotsPrev[0],m_pMeshNode->m_nBoneTableCnt);
		}

		if(!m_vBoneTransPrev.empty())
		{
			RShaderCompositeManager::m_cBlendTransPrev.SetVector4Array( &m_vBoneTransPrev[0],m_pMeshNode->m_nBoneTableCnt);
		}
	}

	if( m_fSBasecale > 0)
		RShaderCompositeManager::m_cBlendScale.SetFloat( m_fSBasecale);

 #else
	if(!m_matBones.empty())
	{
		RShaderCompositeManager::m_cBlendMatrices.SetMatrixArray(&m_matBones[0],m_pMeshNode->m_nBoneTableCnt);
	}
 #endif
#endif
}

// COMMENT OZ - 이곳에서 케릭터가 랜더 된다. 
void RActorNodePhysiqueGPU::RenderPrimitive(int index)
{
	RPFC_THISFUNC;
	m_pMeshNode->m_pVb->RenderIndexBuffer( 
		m_pMeshNode->m_pMtrlTableInfo[index].offset,
		m_pMeshNode->m_pMtrlTableInfo[index].count,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	REngine::GetResourceProfiler().AddPolygon(PPT_ACTORPHYSIQUE, m_pMeshNode->m_pMtrlTableInfo[index].count );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTORPHYSIQUE, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, index);
}

void RActorNodePhysiqueGPU::RenderAllNodePrimitive()
{
	RPFC_THISFUNC;
	m_pMeshNode->m_pVb->RenderIndexBuffer( 
		0,
		m_pMeshNode->m_nFaceCnt,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	REngine::GetResourceProfiler().AddPolygon(PPT_ACTORPHYSIQUE, m_pMeshNode->m_nFaceCnt );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTORPHYSIQUE, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, -1);
}

}