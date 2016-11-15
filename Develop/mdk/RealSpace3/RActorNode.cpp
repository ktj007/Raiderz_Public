#include "StdAfx.h"
#include "RActorNode.h"
#include "RMeshNode.h"
#include "RActor.h"
#include "RActorCollision.h"
#include "RRenderHelper.h"
#include "RUtil.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"

#include "RAnimationControllerNode.h"
#include "RAnimationNode.h"
#include "RProfileInfo.h"

#include "RDirectionalLightSceneNode.h"
#include "RActorPassRenderer.h"
#include "REnvironmentSet.h"
#include "RActorAlignController.h"
#include "RShaderEffectController.h"

#include "REffectSceneNode.h" //RActorNode에서 이팩트씬노드와 링크되어있다.
#include "RShaderCompositeManager.h"
#include "RMaterialResource.h"

#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"
#include "RBackgroundWorker.h"
#include "RShaderFX.h"

#include "RShadowStrategy.h"
#include "RAnimationResource.h"

#include "RMeshCollision.h"
#include "RActorNodeConstraintController.h"
#include "RActorNodeRenderStrategy.h"

namespace rs3 {

MImplementRTTI(RActorNode, RSceneNode);
MImplementRTTI(RActorNodeMLeaf, RActorNode);

RActorNode::RActorNode(ACTOR_NODE_TYPE eActorNodeType) : 
	m_nBoneRefCount(0), m_strName(m_strNodeName)
	, m_pAlignController(NULL)/*, m_pAnimationController(NULL)*/
	, m_pNextRenderChain(NULL)
	, m_dwPartsColor(0xffffffff)
	, m_dwNoiseRefColor1(RVector4(0.988f,0.474f,0.222f,0))
	, m_dwNoiseRefColor2(0)
	, m_fNoiseRefSection1(0.1f)				//(0)
	, m_fNoiseRefSection2(0.15f)				//(0)
	, m_nUpdateVertexBufferFrame(0)
	, m_pUpdateVertexAniNode(NULL)
	, m_bCreated(false)
	, m_baseComponents(RSC_POSITION | RSC_NORMAL)
	, m_pBlengingMaterial(NULL)
	, m_bBlendingMaterial(false)
	, m_nBlendingMaterialID(-1)
	, m_fBlendingFactor(0)
	, m_fBlendingTime(0)
	, m_fBlendingAccTime(0)
	, m_fIlliminationScale(1.0f)
	, m_bCanInstancing(true)
	, m_bAddColor( false )
	, m_vAddColor( 0.f, 0.f, 0.f )
{
	m_fVisibility	= 1.f;
	m_fNoiseRef		= 1.f;
	m_pActor		= NULL;
	m_pMeshNode		= NULL;

	m_matResult.MakeIdentity();

	m_eActorNodeType = eActorNodeType;
	m_bCullingNode	= false;
}

RActorNode::~RActorNode()
{
	// 	if(m_pAnimationController)
	// 		m_pAnimationController->Disconnect();
	// 
	if(m_pAlignController)
	{
		RemoveController(m_pAlignController);
	}
	SAFE_DELETE(m_pAlignController);

	RemoveFromParent();
	_ASSERT( m_relatedControllers.empty() == true );

	// TODO: RShaderComposite 레퍼런스 관리

// 	m_vecCurrentShaders.clear();
// 	m_vecCurrentShaderIDs.clear();
// 	m_vecLoadingShaders.clear();
// 	m_vecLoadingShaderIDs.clear();

	_ASSERT( m_nBoneRefCount == 0 );
}

//#define TRACEBONE
#ifdef TRACEBONE
const char* TRACEBONENAME = "Bip01 R Toe0";
#endif

int	RActorNode::AddBoneRef()
{
#ifdef TRACEBONE
 	if(m_strNodeName==TRACEBONENAME)
 		mlog("AddBoneRef : %s %d\n", TRACEBONENAME, m_nBoneRefCount +1);
#endif
	if(GetParentActorNode())
		GetParentActorNode()->AddBoneRef();
	return ++m_nBoneRefCount;
}

int RActorNode::ReleaseBoneRef()
{
#ifdef TRACEBONE
 	if(m_strNodeName==TRACEBONENAME)
 		mlog("ReleaseBoneRef : %s %d\n", TRACEBONENAME, m_nBoneRefCount -1);
#endif
	if(GetParentActorNode())
		GetParentActorNode()->ReleaseBoneRef();
	return --m_nBoneRefCount;
}

bool RActorNode::CreateBoneReferenceTable()
{
	//////////////////////////////////////////////////////////////////////////
	// 뼈 참조 테이블을 만든다.
	m_refBones.clear();
	m_matBones.clear();

#ifdef USING_RSC_PHYSIQUE_QT
	m_quatBoneRots.clear();
	m_vBoneTrans.clear();
	m_quatBoneRotsPrev.clear();
	m_vBoneTransPrev.clear();
#endif
#ifdef USING_RSC_PHYSIQUE_QT
	m_quatBoneBaseRots.clear();
	m_vBoneBaseTrans.clear();
	m_vBoneBaseScales.clear();
#endif

#ifdef USING_RSC_PHYSIQUE_DQ
	m_dqBoneDQ.clear();
#endif
	m_matBoneBases.clear();

	RQuaternion quatRot;
	RVector3 vScale;
	RVector4 vTran;

	//매트릭스로부터 스케일 값을 뽑아낸다. 
	m_pMeshNode->m_matBase.Decompose( vScale, vTran, quatRot);
	// 스케일 값은 높이를 기준으로..
	m_fSBasecale = vScale.z;

	for(int iBone=0;iBone<m_pMeshNode->m_nBoneTableCnt;iBone++) 
	{
		// pBoneNode는 기본 메시 상태에서의 본 정보
		// pBoneActorNode는 현재 프레임에서의 정보
		RMeshNode *pBoneNode = m_pMeshNode->m_pBaseMesh->m_NodeTable[m_pMeshNode->m_pBoneTableIndex[iBone]];
		RActorNode* pBoneActorNode = m_pActor->GetActorNode(pBoneNode->m_Name.c_str());
		if (!pBoneActorNode) continue;
		pBoneActorNode->AddBoneRef();
		m_refBones.push_back( pBoneActorNode);
		m_matBones.push_back( (*pBoneActorNode->m_matWorld) * m_pActor->GetWorldInverseTransform());

		m_matBoneBases.push_back( m_pMeshNode->m_matBase * pBoneNode->m_matInvBase);
#ifdef USING_RSC_PHYSIQUE_QT
		// 매트릭스로부터 쿼터니언과 이동 값 추출 : base 
		m_matBoneBases[iBone].Decompose( vScale, vTran, quatRot);
		m_quatBoneBaseRots.push_back( quatRot);
		m_vBoneBaseTrans.push_back( vTran);
		m_vBoneBaseScales.push_back( vScale);
#endif
#ifdef USING_RSC_PHYSIQUE_QT
		// 매트릭스로부터 쿼터니언과 이동 값 추출 
		m_matBones[iBone].Decompose( vScale, vTran, quatRot);
		m_quatBoneRots.push_back( quatRot);
		m_vBoneTrans.push_back( vTran);
		m_quatBoneRotsPrev.push_back( quatRot);
		m_vBoneTransPrev.push_back( vTran);
#endif
#ifdef USING_RSC_PHYSIQUE_DQ
		// 추출 한 값으로부터 듀얼 쿼터니언 생성 
		RDQuaternion dq( quatRot, vTran);
		m_dqBoneDQ.push_back( dq);
#endif

		// m_nBoneTableCnt가 있으면 피지크용 이라 생각 해도 될 것 같다. 
		// 매트릭스를 사용 안하고 쿼터니언과 이동값만 사용 하기 위해 피지크에 사용 되는 것은 uniform scale만 지원하도록 한다. 090710, OZ
		//pBoneNode->NotUseScale();
		//pBoneActorNode->NotUseScale();
		//this->SetUseUniformScale();
		//m_pMeshNode->NotUseScale();
		//m_pBaseMesh->NotUseScale();
	}

	// 렌더링되는 노드이면 스스로의 레퍼런스를 하나 늘려놓는다
	if( m_pMeshNode->isRenderAble() )
		AddBoneRef();

	return true;
}

void RActorNode::DestroyBoneReferenceTable()
{
	// CreateBoneReferenceTable 에서 했던것을 정확히 반대로 되돌려야 한다
	for(size_t j=0;j<m_refBones.size();j++) 
	{
		RActorNode* pBoneNode = m_refBones[j];
		pBoneNode->ReleaseBoneRef();
	}

	m_refBones.clear();
	m_matBones.clear();
#ifdef USING_RSC_PHYSIQUE_QT
	m_quatBoneRots.clear();
	m_vBoneTrans.clear();
	m_quatBoneRotsPrev.clear();
	m_vBoneTransPrev.clear();
	m_quatBoneBaseRots.clear();
	m_vBoneBaseTrans.clear();
	m_vBoneBaseScales.clear();
#endif
#ifdef USING_RSC_PHYSIQUE_DQ
	m_dqBoneDQ.clear();
#endif
	m_matBoneBases.clear();

	if( m_pMeshNode->isRenderAble() )
		ReleaseBoneRef();
}


bool RActorNode::Create()
{
	_ASSERT( !m_bCreated );
	if(!m_pMeshNode) return false;

	if(m_pMeshNode->m_nAlign!=RMA_NONE)
	{
		if(m_pMeshNode->m_nAlign==RMA_NORMAL)
			m_pAlignController = new RActorAlignController(m_pActor);
		else
		if(m_pMeshNode->m_nAlign==RMA_Z_FIXED)
			m_pAlignController = new RActorZFixedAlignController(m_pActor);

		AddController(m_pAlignController);
	}

//	CreateBoneReferenceTable();

	m_subMaterialMask.clear();
	m_vecMaterialTable.clear();
	for(int i=0; i<m_pMeshNode->m_nMtrlCnt; ++i)
	{
		m_subMaterialMask.push_back(true);
	}

	if(!OnCreateDeviceIndependentPart()) return false;

	CreateBoneReferenceTable();

	m_bCreated = true;

	return true;
}

ComponentDesc_GStage RActorNode::GetGStageComponentDesc(int nSubMaterialID, bool bInstancing, bool bNoiseRef)
{
	ComponentDesc_GStage desc;

	desc.pMtrl = GetSubMaterial(nSubMaterialID);
	desc.bPhysique = IsUsePhysiue();
	desc.bTexCoord = IsUseTexCoord();
	desc.bExtraTexCoord = IsUseExtraTexCoord();
	desc.bVertexColor = IsUsePointColor();
	desc.bNoiseRef = bNoiseRef;
	desc.bInstancing = bInstancing;
	desc.bMultiplyColor = m_pActor->IsUseMultiplyColor();
	desc.bAO = m_pActor->GetAO();
	desc.bBlendingMaterial = (m_bBlendingMaterial && nSubMaterialID == m_nBlendingMaterialID);

	return desc;
}

ComponentDesc_MStage RActorNode::GetMStageComponentDesc(bool bAddColorPass, int nSubMaterialID, bool bInstancing, bool bNoiseRef)
{
	ComponentDesc_MStage desc;

	desc.pMtrl = GetSubMaterial(nSubMaterialID);
	desc.bAddColorPass = bAddColorPass;
	desc.bPhysique = IsUsePhysiue();
	desc.bVertexColor = IsUsePointColor();
	desc.bTexCoord = IsUseTexCoord();
	desc.bExtraTexCoord = IsUseExtraTexCoord();
	desc.bUseShade = !IsNoShade();
	desc.bBlendingMaterial = (m_bBlendingMaterial && nSubMaterialID == m_nBlendingMaterialID);
	desc.fVisibility = GetVisibilityResult();
	desc.bNoiseRef = bNoiseRef;
	desc.bInstancing = bInstancing;
	desc.bMultiplyColor = m_pActor->IsUseMultiplyColor();

	return desc;
}

ComponentDesc_MStage RActorNode::GetMaskingPassComponentDesc(int nSubMaterialID)
{
	ComponentDesc_MStage desc;

	desc.pMtrl = GetSubMaterial(nSubMaterialID);
	desc.bAddColorPass = false;
	desc.bPhysique = IsUsePhysiue();
	desc.bVertexColor = IsUsePointColor();
	desc.bTexCoord = IsUseTexCoord();
	desc.bExtraTexCoord = false;
	desc.bUseShade = !IsNoShade();
	desc.bBlendingMaterial = false;
	desc.fVisibility = GetVisibilityResult();
	desc.bNoiseRef = false;
	desc.bInstancing = false;
	desc.bMultiplyColor = m_pActor->IsUseMultiplyColor();

	return desc;
}

RShaderFX* RActorNode::CheckAndCreateTechniqueTableOnMaterialForGStage( RRENDERINGMETHOD renderMethod, int nSubMaterialID, bool bInstancing, bool bNoiseRef, bool bBackgroundCreation)
{
	_ASSERT( nSubMaterialID < m_pMeshNode->m_nMtrlCnt );

	RMaterial* pMtrl = GetSubMaterial(nSubMaterialID);
	if( pMtrl == NULL )
		return NULL;


	RPFC_THISFUNC;
	
	RShaderCompositeManager* pShaderCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();

	RSCID rscComponents = pShaderCompositeManager->MakeComponentBitflagForGStage( renderMethod, GetGStageComponentDesc(nSubMaterialID, bInstancing, bNoiseRef) );

	// 지오메트리 패스에서는 림 라이트 관련 정보만 씌워줌.
	if( m_pActor->GetShaderEffectController())
		rscComponents = m_pActor->GetShaderEffectController()->CommitShaderGStage( &rscComponents, IsUsePointColor(), m_pActor->m_bUseShade, m_pActor->m_fDistanceToCamera );


	if( renderMethod == RENDERING_DEFERRED && IsApplyMaterialDecal(IsUseTexCoord(), pMtrl) )
		REngine::GetSceneManager().GetShaderCompositeManager()->ApplyMaterialDecalComponents(rscComponents);


	m_vecGStageRSCIDs[nSubMaterialID] = rscComponents;

	return pShaderCompositeManager->GetShader( rscComponents, bBackgroundCreation );
}

/// 딱히 Mstage에서는 인퍼드 디퍼드 구분할만한게 현재로서는 없다. 
/// 반투명 오브젝트는 인퍼드든 디퍼드든 M stage에서 그리기는 매한가지고
/// 불투면 오브젝트는 디퍼드인 상태에서는 여기에 진입을 하지 않을 것이다. 
RShaderFX* RActorNode::CheckAndCreateTechniqueTableOnMaterialForMStage( RRENDERINGMETHOD renderMethod, int nSubMaterialID, bool bInstancing, bool bNoiseRef, bool bBackgroundCreation)
{
	_ASSERT( nSubMaterialID < m_pMeshNode->m_nMtrlCnt );

	bool bAddColorPass = false;

	// Deferred 랜더 시스템일 시는 M-stage의 것은 Add pass로 사용 되는 것으로 인식하게 한다. 
	// Deferred 랜더 시스템이라도 알파로 그릴 시는 이 함수를 호출 시 RENDERING_INFERRED로 넘겨줘서 랜더할 것이다. 아님 낭패 OTL
	// 하지만 DEFERRED로 넘어와도, 라이팅 없는 조건이면 일반적으로 그리는 것으로 취급한다. 아오 뭐 구조가 이따구야.
	if( renderMethod == RENDERING_DEFERRED && !IsNoShade())
	{
		bAddColorPass = true;
	}

	RShaderCompositeManager* pShaderCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();

	RSCID rscComponents = pShaderCompositeManager->MakeComponentBitflagForMStage( GetMStageComponentDesc(bAddColorPass, nSubMaterialID, bInstancing, bNoiseRef) );

	if( m_pActor->GetShaderEffectController())	// 특수 효과 셰이더 적용
	{
		rscComponents = m_pActor->GetShaderEffectController()->CommitShaderMStage( &rscComponents, IsUsePointColor(), !IsNoShade());
	}


	if( !bAddColorPass && IsApplyMaterialDecal(IsUseTexCoord(), GetSubMaterial(nSubMaterialID)) )
		REngine::GetSceneManager().GetShaderCompositeManager()->ApplyMaterialDecalComponents(rscComponents);
	

	m_vecMStageRSCIDs[nSubMaterialID] = rscComponents;

	return pShaderCompositeManager->GetShader( rscComponents, bBackgroundCreation );
}

void RActorNode::SetAddEffectComponents( RSCID& _rscComponents )
{
	if( m_bAddColor )
	{
		_rscComponents |= RSC_ADD_SPECIFIC_COLOR | RSC_ADD_COLOR;
		RShaderCompositeManager::m_cAddColor.SetVector3( m_vAddColor );
	}
}


void RActorNode::EnableAddColorEffect(const RVector3& color)
{
	m_bAddColor = true;
	m_vAddColor = color;
}


void RActorNode::DisableAddColorEffect()
{
	m_bAddColor = false;
}


bool RActorNode::OnCreateDeviceIndependentPart()
{
	int nCount = m_pMeshNode->m_nMtrlCnt;

	m_vecGStageRSCIDs.resize(nCount);
	m_vecMStageRSCIDs.resize(nCount);

	return true;
}

bool RActorNode::OnCreateDeviceDependentPart()
{
	// Dummy나 Bone은 출력하지 않는다.
	if( m_pMeshNode->isBone() || m_pMeshNode->isDummyMesh() )
		return true;

	char szSubName[10];

//	_ASSERT( (strstr(m_pActor->m_pMesh->GetName().c_str(),"hf_adult")==0) || (strstr(m_pMeshNode->GetName(),"hair")==0) );

	for( int i = 0; i < m_pMeshNode->m_nMtrlCnt; ++i )
	{
		int nMaterialID = m_pMeshNode->m_pMtrlTableInfo[i].mtrlid;
		sprintf(szSubName, ".%d", nMaterialID);
		string szMeshName = m_pMeshNode->m_pBaseMesh->m_strName + szSubName;
		RMaterialResource* pMaterialResource = REngine::GetSceneManager().CreateResource< RMaterialResource >( szMeshName.c_str() );

// 		if( pMaterialResource->GetState() == RRS_EMPTY )
// 		{
// 			pMaterialResource->Fill();
// 			if (!pMaterialResource->Load()) 
// 				mlog_tool( "  %s 파일 %s 노드의 리소스 오류\n", m_pActor->m_pMesh->GetName().c_str(), GetName().c_str());
// 
// 		}

		if (pMaterialResource->GetRefCount() > 1)
			m_vecMaterialTable.push_back(pMaterialResource);
		else	// material 을 찾을수 없다면
			m_vecMaterialTable.push_back( RMesh::m_pDefaultMaterial);

		REngine::GetSceneManager().ReleaseResource(pMaterialResource);
	}

	return true;
}

bool RActorNode::OnCreateMaterials(bool bBackground)
{
	for( size_t i = 0; i < m_vecMaterialTable.size(); ++i )
	{
		RMaterialResource* pMaterialResource = m_vecMaterialTable[i];
		if(pMaterialResource==NULL) continue;
		pMaterialResource->BuildAll( bBackground, m_pActor );
	}

	return true;
}

void RActorNode::RemoveRelatedController( RActorNodeConstraintController* pController )
{
	m_relatedControllers.remove( pController );
}

void RActorNode::AddRelatedController( RActorNodeConstraintController* pController )
{
	m_relatedControllers.push_back(pController);
}

void RActorNode::Destroy()
{
//	mlog("%s %s destroyed\n", m_pActor->GetNodeName().c_str(), m_strName.c_str() );
	DestroyBoneReferenceTable();

	OnDestroy();

	for (std::list<RActorNodeConstraintController*,RStlCustomAlloc<RActorNodeConstraintController*>>::iterator itr = m_relatedControllers.begin(); itr != m_relatedControllers.end(); ++itr )
	{
		(*itr)->Invalidate();
	}
	m_relatedControllers.clear();
}

void RActorNode::OnDestroy()
{
}

void RActorNode::SetSubMaterialMask(int nSubMaterial,bool bShow)
{
	//	_ASSERT(nSubMaterial<m_pMeshNode->m_nMtrlCnt);
	//	if(nSubMaterial>=m_pMeshNode->m_nMtrlCnt) return;
	for(int i=0; i<m_pMeshNode->m_nMtrlTableInfoCnt; ++i)
	{
		if(m_pMeshNode->m_pMtrlTableInfo[i].nSubMaterialIDForDrawMasking==nSubMaterial)
			m_subMaterialMask[i] = bShow;
	}
}

void RActorNode::SetAllSubMaterialMask( bool bShow )
{
	for(int i=0; i<m_pMeshNode->m_nMtrlTableInfoCnt; ++i)
	{
		m_subMaterialMask[i] = bShow;
	}
}

void RActorNode::UpdateTransformAndVertex()
{
}

bool RActorNode::PickLocal(const RVector &origin,const RVector &to,RPICKINFO *pPickInfo)
{
	bool bPickFound = false;
	float fPickDistanceSq = FLT_MAX;
	RVector dir = (to-origin).Normalize();
	for(int i=0;i<m_pMeshNode->m_nFaceIndexTableCnt/3;i++)
	{
		RVector pos0 = m_pMeshNode->m_pPointTable[m_pMeshNode->m_pVertexIndexTable[m_pMeshNode->m_pFaceIndexTable[i*3+0]].p];
		RVector pos1 = m_pMeshNode->m_pPointTable[m_pMeshNode->m_pVertexIndexTable[m_pMeshNode->m_pFaceIndexTable[i*3+1]].p];
		RVector pos2 = m_pMeshNode->m_pPointTable[m_pMeshNode->m_pVertexIndexTable[m_pMeshNode->m_pFaceIndexTable[i*3+2]].p];
		RVector pickPos;

		if(MMath::IsIntersect(origin,dir,pos0,pos1,pos2,&pickPos))
		{
			float lengthSq = (pickPos-origin).LengthSq();
			if(fPickDistanceSq>lengthSq) {
				bPickFound = true;
				pPickInfo->outPosition = pickPos;
				fPickDistanceSq = lengthSq;
				// TODO : normal ?
			}
		}
	}
	return bPickFound;
}

bool RActorNode::OnPick(RPICKINFO& pickInfo)
{
	if( m_eActorNodeType == ACTOR_NODE_COMMON )
	{
		// normal 일때는 box test
		RMatrix invWorld;
		m_matWorld->GetInverse(&invWorld);

		RVector localOrigin = pickInfo.inPickOrigin * invWorld;
		RVector localTo = pickInfo.inPickTo * invWorld;

		if(!m_pMeshNode->m_boundingBox.ContainsLineSegment(localOrigin,localTo))
			return false;	

		if(pickInfo.inMethod != RPM_DETAIL)	// box 안에 있으므로 true
		{
			float fDist;
			RVector localDir = Normalize(localTo-localOrigin);
			if(m_pMeshNode->m_boundingBox.Intersect(&fDist,localOrigin,localDir))
			{
				pickInfo.outPosition = (localOrigin+fDist*localDir) * (*m_matWorld);
				return true;
			}
			return false;
		}

		// detail 일때는 polygon picking
		if(PickLocal(localOrigin,localTo,&pickInfo))
		{
			pickInfo.outPosition = pickInfo.outPosition * (*m_matWorld);
			return true;
		}
		return false;
	}

	return false;
}

void RActorNode::ResetTransform()
{
	//	static frame 으로 복구 , parent 가 미리 불린다고 가정.
	SetTransform(m_pMeshNode->m_matLocal);
	SetVisibility(m_pMeshNode->m_fBaseVisibility);
	SetNoiseRef(m_pMeshNode->m_fBaseNoiseRef);

	UpdateAllParentDependentValues();
	UpdateFinalResultTransforms();
}

bool RActorNode::Validate()
{
	for(int i=0;i<m_pMeshNode->m_nMtrlCnt;i++)
	{
		if(GetSubMaterial(i)==false)
			return false;
	}

	return true;
}

bool RActorNode::GetVisible() const
{
	return ( RSceneNode::GetVisible() && m_pMeshNode->isVisibleMesh() && m_pActor->GetVisible() );
}

bool RActorNode::GetUsable() const 
{
	return	RSceneNode::GetUsable() && m_pActor->GetUsable();
}

RMatrix RActorNode::GetBaseLocalTransform()		// 원래 메쉬의 base 트랜스폼을 얻는다
{
	return m_pMeshNode->m_matLocal;
}

void RActorNode::UpdateLocalAnimation()
{
	RAnimationControllerNode* pControllerNode = m_pActor->GetAnimationController().GetNodeController( GetNodeName() );
	if(pControllerNode)
	{
		pControllerNode->Update( m_pActor->GetAnimationController().GetFrame() );
		pControllerNode->UpdateActorNode();
	}
}

void RActorNode::ForceUpdateAnimation()
{
	if(m_pParent==NULL)
	{
		// 이미 destroy 되었다 ?
		return;
	}

// 	// 부모가 있으면 부모도 강제 update 해야 한다.
	if(m_pParent != m_pActor)
	{
		RActorNode *pParentNode = MDynamicCast(RActorNode, m_pParent);
		if ( pParentNode )
			pParentNode->ForceUpdateAnimation();
	}

	UpdateLocalAnimation();
}

void RActorNode::RenderWire(DWORD dwColor)
{
	REngine::GetDevice().ShaderOff();
	REngine::GetDevice().SetLighting(false);

	RMatrix matWorld = GetWorldTransform();

	RVector* pPointTable;
	
	// vertex animation 이거나 physique 일때 처리
	bool bSoftBuffer = m_pActor->IsUsingVertexAnimation() || m_pMeshNode->isPhysique();
	if(bSoftBuffer)
		pPointTable = m_pMeshNode->m_pPointTable;
	else
		pPointTable = m_pMeshNode->m_pPointTableOrg;

	// 폴리곤의 wireframe
	for(int i=0;i<m_pMeshNode->m_nFaceIndexTableCnt/3;i++)
	{
		for(int j=0;j<3;j++)
		{
			RVector pos0 = pPointTable[m_pMeshNode->m_pVertexIndexTable[m_pMeshNode->m_pFaceIndexTable[i*3+j]].p];
			RVector pos1 = pPointTable[m_pMeshNode->m_pVertexIndexTable[m_pMeshNode->m_pFaceIndexTable[i*3+(j+1)%3]].p];
			RRenderHelper::m_batchLineDrawer.Add(pos0 , pos1);
		}
	}
	RRenderHelper::m_batchLineDrawer.Render(matWorld,dwColor);
}

void RActorNode::RenderNormal(DWORD dwColor)
{
	RPFC_THISFUNC;

	REngine::GetDevice().ShaderOff();
	REngine::GetDevice().SetLighting(false);

	RMatrix matWorld = GetWorldTransform();

	RVector* pPointTable;

	// vertex animation 이거나 physique 일때 처리
	bool bSoftBuffer = m_pActor->IsUsingVertexAnimation() || m_pMeshNode->isPhysique();
	if(bSoftBuffer)
		pPointTable = m_pMeshNode->m_pPointTable;
	else
		pPointTable = m_pMeshNode->m_pPointTableOrg;

	// show normal
	float fSize = 10.f;
	for(int i=0;i<m_pMeshNode->m_nVertexIndexTableCnt;i++)
	{
		int nPosIndex = m_pMeshNode->m_pVertexIndexTable[i].p;
		RVector pos = pPointTable[nPosIndex];
		RVector normal = m_pMeshNode->m_pPointNormalTable ? 
			m_pMeshNode->m_pPointNormalTable[i] :	// normal table 
		m_pMeshNode->m_pNormalTable[m_pMeshNode->m_pVertexIndexTable[i].n];
		RRenderHelper::m_batchLineDrawer.Add(pos , pos+fSize*normal);
	}
	RRenderHelper::m_batchLineDrawer.Render(matWorld,dwColor);
}

void RActorNode::RenderPrimitive(int index)
{
	RPFC_THISFUNC;
	bool bVertexAni = m_pActor->IsUsingVertexAnimation();
	if(bVertexAni)
		m_pMeshNode->m_pVb->RenderIndexSoft( 
		&m_pMeshNode->m_pIb->m_pIndex[m_pMeshNode->m_pMtrlTableInfo[index].offset],
		m_pMeshNode->m_pMtrlTableInfo[index].count,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	else
		m_pMeshNode->m_pVb->RenderIndexBuffer( 
		m_pMeshNode->m_pMtrlTableInfo[index].offset,
		m_pMeshNode->m_pMtrlTableInfo[index].count,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	REngine::GetResourceProfiler().AddPolygon( PPT_ACTOR, m_pMeshNode->m_pMtrlTableInfo[index].count );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTOR, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, index);
}

void RActorNode::SetVertexStream( bool b2ndBufferMng)
{
	RPFC_THISFUNC;
	// 버텍스 설정
	if ( !m_pMeshNode->m_pVb || !m_pMeshNode->m_pIb )
		return;

	// 버텍스 애니메이션 설정
	if(m_pUpdateVertexAniNode)
	{
		m_pUpdateVertexAniNode->GetVecValue(m_nUpdateVertexBufferFrame, m_pMeshNode->m_pPointTable);
		m_pMeshNode->UpdateNodeBuffer();
	}

	// 메시노드의 버텍스 설정
	m_pMeshNode->SetVertexStream();

	if( b2ndBufferMng)
	{
		// 세컨드 버퍼 관련 설정을 아무것도 하지 않으면 DX debug ver. 에러
		REngine::GetDevice().SetupForRenderInstancing( R_NONE, 1);
	}
}

bool RActorNode::CheckInstancingCondition()
{
	if(CanInstancing()==false)
		return false;

	const RVector4 vWhite(1,1,1,1);

	if( IsAlpha() || IsApplyNoise() || m_pActor->GetNoiseRefResult() < 1 || m_pActor->GetNoiseRef() < 1 || m_bBlendingMaterial
	 || IsNoShade()
	 || m_pActor->IsUsingVertexAnimation()
	 || m_pActor->IsUseMultiplyColor()	/// Texture Factor Color가 셋팅이 되면 인스턴싱 불가. 셋팅 유무를 이거 말고 이쁜 방법으로 체크가 안되나...
	 )
		return false;
	
	if( m_pActor->GetShaderEffectController() && m_pActor->GetShaderEffectController()->CheckInstancingCondition() == false )
		return false;

	if( m_pActor->IsPEEdge() )
		return false;

	return true;
}


void RActorNode::SetBlendMaterialRenderState()
{
	RPFC_THISFUNC;
	// 재질 교체 중에 블랜딩
	if( m_bBlendingMaterial == false)
		return;

	
	m_fBlendingAccTime += REngine::GetTimer().GetElapsedTime();
	m_fBlendingFactor = m_fBlendingAccTime / m_fBlendingTime;
	RShaderCompositeManager::m_cBlendFactor.SetFloat( m_fBlendingFactor );

	if( m_fBlendingFactor >= 1.0f)
	{
		m_bBlendingMaterial = false;
		m_pBlengingMaterial = NULL;
	}
	else
	{
		_ASSERT(m_pBlengingMaterial);
		REngine::GetDevice().SetTexture( AS_BLENDING_MATERIALMAP, m_pBlengingMaterial->GetTexLayerByType(RTT_DIFFUSE)->GetTexture() );
		REngine::GetDevice().SetTextureFilter( AS_BLENDING_MATERIALMAP, REngine::GetConfig().GetTextureFilter() );
		REngine::GetDevice().SetTextureMaxAnisotropy( AS_BLENDING_MATERIALMAP, REngine::GetConfig().GetTextureMaxAnisotropy() );
	}
}

void RActorNode::SetNodeRenderState(const RMatrix& matView, const RMatrix& matViewProj)
{
	RPFC_THISFUNC;


	SetShaderTransformConstant(matView, matViewProj);


	RShaderCompositeManager::m_cVisiblity.SetFloat(GetVisibilityResult());

	RShaderCompositeManager::m_cPartsColor.SetVector4( RVector4(m_dwPartsColor) );
	RShaderCompositeManager::m_cTextureFactor.SetVector4( m_pActor->GetMultiplyColor() );

	if(IsApplyNoise())
	{
		RVector4 vNoiseFadeParm( GetNoiseRefResult(), m_fNoiseRefSection1, m_fNoiseRefSection2, 0);
		RShaderCompositeManager::SetNoiseFadeConstant(vNoiseFadeParm, RVector4(m_dwNoiseRefColor1), RVector4(m_dwNoiseRefColor2));
	}


	/// 이곳에 오면 인스턴싱이 아니라는 뜻이 되므로 노드를 ID로 사용
	if( REngine::GetSceneManager().GetShaderCompositeManager()->GetDSF())
		RShaderCompositeManager::m_cDSFID.SetFloat( (float)(((int)this)%10));
}




void RActorNode::SetNodeRenderStateForEdge( RCameraSceneNode* pCurrentCamera)
{
	PFC_THISFUNC;

	const RMatrix& matWorld = GetWorldTransform();
	
	if( IsPEEdge())
	{
		RShaderCompositeManager::m_cWorldPrev.SetMatrix( matWorld );
	}
}


void RActorNode::SetNodeRenderStateForBlur( RCameraSceneNode* pCurrentCamera)
{
	PFC_THISFUNC;
	// 픽셀 모션 블러를 위한 상수 설정
	if( m_pActor->m_nPixelMotionBlur > PMB_NONE)
	{
		RShaderCompositeManager::m_cMotionBlurFactorFront.SetFloat( (float)(m_pActor->m_nPixelMotionBlur-1));
	}

	const RMatrix& matWorld = GetWorldTransform();
	RMatrix matWorldPrev = GetWorldTransformPrev();
	// 이전 프레임 매트릭스를 그대로 적용 하지 않고, 30 FPS 맞춤 기준 설정
	if( IsVertexMotionBlur() || IsPixelMotionBlur())
	{
		if( GetMotionBlurBasisFrame() != 0)
		{
			float fSlerp = REngine::GetDevice().GetFrameRate() / GetMotionBlurBasisFrame();
			// 한줄로 써 넣는 것보다 가독성 측면에서 이게 낫네..
			//*this = to-from; // delta on all 3 axis
			//*this*=slerp;    // times interpolant distance.
			//*this+=from;     // plus source
			RMatrix matFixedWorldPrev = matWorldPrev - matWorld;
			matFixedWorldPrev = matFixedWorldPrev * fSlerp;
			matFixedWorldPrev = matFixedWorldPrev + matWorld;
			matWorldPrev = matFixedWorldPrev;
		}
		RShaderCompositeManager::m_cWorldPrev.SetMatrix( matWorldPrev);
	}
}

void RActorNode::RenderSingle( RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pRenderStrategy)
{
	RPFC_THISFUNC;

	if(CheckDoRender())
	{
		pRenderStrategy->Begin();
		{
			SetVertexStream();

			for (int index = 0; index < m_pMeshNode->m_nMtrlCnt ; ++index)
			{
				if( GetSubMaterialMask(index) == false )
					continue;

				if(pRenderStrategy->IsRender(pCurrentCamera, this, index)==false)
					continue;

				pRenderStrategy->SetMaterialRenderState(this, GetSubMaterial(index));
				pRenderStrategy->SetNodeRenderState( pCurrentCamera, this, GetSubMaterial(index) );


				RShaderFX* pShader = pRenderStrategy->GetShader(this, index, false);
				if( pShader )
				{
					RTechnique technique = pShader->GetTechniqueByIndex(0);

					technique.Begin(NULL, TECH_PRESERVE_NOT);
					technique.BeginPass();

					RenderPrimitive(index);

					technique.EndPass();
					technique.End();
				}			
			}
		}
		pRenderStrategy->End();
	}

	pRenderStrategy->ApplyDistortion(pCurrentCamera, this, -1);
}

void RActorNode::RenderSingleOneMaterial( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RActorNodeRenderStrategy* pRenderStrategy)
{
	RPFC_THISFUNC;
	if(CheckDoRender()==false)
		return;

	if( GetSubMaterialMask(nSubMaterialID) == false )
		return;


	if(pRenderStrategy->IsRender(pCurrentCamera, this, nSubMaterialID)==false)
		return;


	RMaterial* pMaterial =NULL;
	if(nSubMaterialID>=0)
	{
		pMaterial = GetSubMaterial(nSubMaterialID);
		pRenderStrategy->SetMaterialRenderState( this, pMaterial );
	}
	pRenderStrategy->SetNodeRenderState( pCurrentCamera, this, pMaterial );


	pRenderStrategy->Begin();
	{
		SetVertexStream();


		RShaderFX* pShader = pRenderStrategy->GetShader(this, nSubMaterialID, false);

		if( pShader )
		{
			RTechnique technique = pShader->GetTechniqueByIndex(0);

			technique.Begin(NULL, TECH_PRESERVE_NOT);
			technique.BeginPass();

			if(nSubMaterialID<0)
				RenderAllNodePrimitive();
			else
				RenderPrimitive(nSubMaterialID);

			technique.EndPass();
			technique.End();
		}
	}
	pRenderStrategy->End();


	pRenderStrategy->ApplyDistortion(pCurrentCamera, this, nSubMaterialID);
}

RQuaternion RActorNode::GetRotation()
{
	RQuaternion quatRotation = (*m_matLocal);
	return quatRotation;
}

void RActorNode::OnRender(RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategyMStageInferred renderStrategy;

	if( nSubMaterialID >= 0 )
	{
		RenderSingleOneMaterial( pCurrentCamera, nSubMaterialID, &renderStrategy);
	}
	else
	{
		RenderSingle( pCurrentCamera, &renderStrategy);
	}
}

void RActorNode::RenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, RActorNodeRenderStrategy* pRenderStrategy )
{
	RPFC_THISFUNC;

	if( CheckRenderPerMaterialToShadowMap() )
		RenderSingle(pCurrentCamera, pRenderStrategy);
	else
		RenderSingleOneMaterial(pCurrentCamera, -1, pRenderStrategy);
}

bool RActorNode::IsAlpha()
{
	if( GetVisibilityResult() < 1.f || IsUsePointColor())
		return true;

	return false;
}

bool RActorNode::CheckRenderPerMaterialToShadowMap()
{
	bool bHaveAlpha =m_pMeshNode->hasAlphaMaterial();
	bool bUseAlphaTest =m_pMeshNode->CheckMaterialFlag(RM_FLAG_USEALPHATEST);

	return bHaveAlpha || bUseAlphaTest;
}

// 이름은 중복될 위험의 소지가 있다
void RActorNode::ChangeMtrlSet(string strSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	int nSrcIndex = -1;
	int nDstIndex = -1;

	// SrcMtrl을 찾는다.
	for( size_t i = 0; i < m_vecMaterialTable.size(); ++i )
	{
		if( GetSubMaterial(i)->GetName() == strSrcMtrl )
		{
			nSrcIndex = i;
			break;
		}
	}

	// DestMtrl를 찾는다.
	char szSubName[MAX_PATH];

	for( size_t i = 0; i < m_pMeshNode->m_pParentMesh->m_vecMtrlList.size(); ++i )
	{
		sprintf(szSubName, ".%d", i);
		string szMeshName = m_pMeshNode->m_pBaseMesh->GetName() + szSubName;

		RMaterialResource* pMaterialResource = REngine::GetSceneManager().GetResource< RMaterialResource >( szMeshName.c_str() );

		if( pMaterialResource && pMaterialResource->m_pMaterial->GetName() == strDstMtrl )
		{
			nDstIndex = i;
			break;
		}

		REngine::GetSceneManager().ReleaseResource( pMaterialResource );
	}

	ChangeMtrlSet(nSrcIndex, nDstIndex, fBlendingTime);
}

void RActorNode::ChangeMtrlSet(int nSrcMtrl, int nDstMtrl, float fBlendingTime)
{
	if( nSrcMtrl < 0 || nDstMtrl < 0 )
	{
		_ASSERT("RActorNode::ChangeMaterial 없는 재질 인덱스 입니다!!");
		return;
	}

	// 해당 Mtrl을 리소스로 생성
	char szSubName[MAX_PATH];
	sprintf(szSubName, ".%d", nDstMtrl);
	string szMeshName = m_pMeshNode->m_pBaseMesh->GetName() + szSubName;

	RMaterialResource* pMaterialResource = REngine::GetSceneManager().CreateResource< RMaterialResource >( szMeshName.c_str() );

	m_pBlengingMaterial = GetSubMaterial(nSrcMtrl);	// 원본은 Mesh 에서 관리한다. 따로 삭제가 필요 없다.
	m_vecMaterialTable[nSrcMtrl] = pMaterialResource;
	
	REngine::GetSceneManager().ReleaseResource( pMaterialResource );

	if( fBlendingTime > 0.0f )
	{
		m_bBlendingMaterial	= true;
		m_fBlendingTime		= fBlendingTime * 1000.0f;	// Tick 단위로 변환
		m_fBlendingAccTime		= 0;
		m_fBlendingFactor	= 0;
		m_nBlendingMaterialID		= nSrcMtrl;
	}
	OnCreateMaterials();
}

// 인덱스로 검색을 할때 남여 차이에 의해 같은 메쉬인데도 다른 재질 순서가 생기는 문제가 있어서
// 이름으로 검색하게 만들면서 생긴 새로운 규칙
void RActorNode::ChangeMtrlSet(int nSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	int nDstIndex = -1;

	if( nSrcMtrl < 0 )
	{
		_ASSERT("RActorNode::ChangeMaterial 없는 재질 인덱스 입니다!!");
		return;
	}

	// DestMtrl를 찾는다.
	char szSubName[MAX_PATH];
	for( size_t i = 0; i < m_pMeshNode->m_pParentMesh->m_vecMtrlList.size(); ++i )
	{
		sprintf(szSubName, ".%d", i);
		string szMeshName = m_pMeshNode->m_pBaseMesh->GetName() + szSubName;

		RMaterialResource* pMaterialResource = REngine::GetSceneManager().CreateResource< RMaterialResource >( szMeshName.c_str() );

		if( pMaterialResource && pMaterialResource->m_pMaterial->GetName() == strDstMtrl )
		{
			nDstIndex = i;
			break;
		}
		REngine::GetSceneManager().ReleaseResource( pMaterialResource );
	}

	ChangeMtrlSet(nSrcMtrl, nDstIndex, fBlendingTime);
}

bool RActorNode::HasTranslucentMaterial()	// 반투명 재질을 가지고있나
{
	return m_pMeshNode->hasTranslucentMaterial();
}

bool RActorNode::HasAlphaMaterial()			// 알파 (additive 및 blending) material을 포함하는가
{
	return m_pMeshNode->hasAlphaMaterial();
}


bool RActorNode::IsUsePhysiue()
{
	return m_pMeshNode->m_nPhysiqueCnt && !m_pMeshNode->isClothMesh();
}

bool RActorNode::IsUseTexCoord()
{
	return m_pMeshNode->m_nTexCoordTableCnt && m_pMeshNode->m_pTexCoordTable;
}


bool RActorNode::IsUseExtraTexCoord()
{
	return m_pMeshNode->m_nTexCoordExtraTableCnt && m_pMeshNode->m_pTexCoordExtraTable;
}

bool RActorNode::IsUsePointColor()
{ 
	return !m_pMeshNode->isClothMesh() && m_pMeshNode->m_nPointColorCnt;
}


bool RActorNode::IsLoadingCompleted()
{
	if(!m_bCreated) return false;

	if( m_pMeshNode->m_nMtrlCnt != m_vecMaterialTable.size() )	return false;

	for ( int i = 0; i<m_pMeshNode->m_nMtrlCnt; ++i )
	{
		RMaterialResource* pMaterialResource = m_vecMaterialTable.at(i);
		if(!pMaterialResource->IsLoadingCompleted())
			return false;
	}

	return true;
}

bool RActorNode::IsNoShade()
{
	return (!m_pActor->m_bUseShade) || (m_pMeshNode->GetFlag(RM_FLAG_NOSHADE));
}

bool RActorNode::IsUseAddColor(RMaterial* pMaterial)
{
	RShaderEffectController* pEffectController = m_pActor->GetShaderEffectController();
	bool bUseAddEffect = pEffectController && (pEffectController->IsEnableAddColorEffect() || pEffectController->IsEnableGlowEffect());

	return bUseAddEffect || pMaterial->GetTexLayerByType(RTT_SELFILLUMINATION) || IsApplyNoise();
}

//RShaderFX* RActorNode::GetShader(unsigned int nMtrlIndex, bool bMstage)
//{
//	_ASSERT( nMtrlIndex < m_vecMaterialTable.size() );
//	_ASSERT( nMtrlIndex < m_vecGStageRSCIDs.size() );
//	_ASSERT( nMtrlIndex < m_vecMStageRSCIDs.size() );
//
//	RShaderCompositeManager* pShaderCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
//
//	RShaderFX* pShaderGStage = pShaderCompositeManager->GetShader( m_vecGStageRSCIDs[nMtrlIndex] );
//	RShaderFX* pShaderMStage = pShaderCompositeManager->GetShader( m_vecMStageRSCIDs[nMtrlIndex] );
//	
//	if( NULL == pShaderGStage || NULL == pShaderMStage )
//		return NULL;
//
//	RShaderFX* pShader = bMstage ? pShaderMStage : pShaderGStage;
//	return pShader;
//}

void RActorNode::OnAdd()
{
	_ASSERT(m_pActor);

	// 충돌체
	MCollisionVector vCollisions;
	GetMeshCollision(vCollisions, m_pActor->m_pCollision);
	for (MCollisionItr itr = vCollisions.begin(); itr != vCollisions.end(); ++itr)
	{
		AddBoneRef();
		m_pActor->m_pCollision->Add(*itr, this);
	}

}

void RActorNode::OnRemove()
{
	_ASSERT(m_pActor);

	RActorCollision* pCollision = m_pActor->m_pCollision;
	MCollisionVector vCollsion;
	GetMeshCollision(vCollsion, pCollision);

	for (MCollisionItr itr = vCollsion.begin(); itr != vCollsion.end(); ++itr )
	{
		ReleaseBoneRef();
		pCollision->Remove(*itr, this);
	}
}

void RActorNode::GetMeshCollision( std::vector<MCollision*>& _rOut, RActorCollision* _pCollsion )
{
	RPFC_THISFUNC;
	if ( NULL == _pCollsion || NULL == m_pMeshNode)
		return;

	RMesh* pMesh = m_pMeshNode->m_pParentMesh;
	if (NULL == pMesh)
		return;

	RMeshCollision* pCollision = pMesh->m_pMeshCollision;
	if(NULL == pCollision)
		return;

	pCollision->GetCollsionFromParentName(_rOut, GetNodeName());
}

void RActorNode::GetLocalTransformAt(DWORD nFrame, RMatrix& outMatrix)
{
	RAnimationControllerNode* pNode = m_pActor->GetAnimationController().GetNodeController( GetNodeName() );
	if(pNode)
		pNode->GetLocalTransformAt( nFrame, outMatrix );
	else
		outMatrix = (*m_matLocal);
}

void RActorNode::GetWorldTransformAt(DWORD nFrame, RMatrix& outMatrix)
{
	GetResultTransformAt(nFrame, outMatrix);
	outMatrix *= m_pActor->GetWorldTransform();
}

void RActorNode::GetResultTransformAt(DWORD nFrame, RMatrix& outMatrix)
{
	RActorNode* pParent = GetParentActorNode();

	RMatrix matLocal;
	GetLocalTransformAt( nFrame, matLocal);

	if(pParent)
	{
		RMatrix matParent;
		pParent->GetResultTransformAt( nFrame, matParent );

		matLocal.MultiplyTo( matParent, outMatrix);
	}
	else
	{
		outMatrix = matLocal;
	}
}

bool RActorNode::IsRenderable( void )
{
	if(m_pMeshNode == NULL)
		return false;

	if( // 메시속성에 의해
			m_pMeshNode->isRenderAble()
		&&	m_pMeshNode->m_pVb 
		&&	m_pMeshNode->m_pIb
		&&	m_pMeshNode->m_pMtrlTableInfo
		// 액터노드 속성에 의해
		&& (bool)(GetVisibilityResult() > 0.0f)
		&& GetVisible()
		&& (bool)(GetNoiseRefResult() > 0.0f)
		&& (bool)(GetNoiseRef() > 0.0f
		// LOD에 의해
		&& (!m_pActor || m_pActor->GetLODLevel() == m_pMeshNode->GetLODLevel()))
		)
	{
		return true;
	}

	return false;
}

bool RActorNode::CheckDoRender()
{
	if( !GetUsable() || !IsRenderable() )
		return false;

	if(m_pActor->IsDistortionRenderable() == false)
		return false;

	return true;
}

bool RActorNode::PreCreateShader( bool bBackgroundCreation )
{
	if( !m_pMeshNode )
		return false;

	if( m_pMeshNode->m_pMtrlTableInfo == NULL )
		return false;

	if( (int)m_vecMaterialTable.size() < m_pMeshNode->m_nMtrlCnt )
		return false;


	for (int index = 0; index < m_pMeshNode->m_nMtrlCnt ; index ++)
	{
		if( GetSubMaterialMask(index) == false )
			continue;

		CheckAndCreateTechniqueTableOnMaterialForGStage( REngine::GetSceneManager().GetRealtimeRenderingMethod(), index, true, false, bBackgroundCreation );
		CheckAndCreateTechniqueTableOnMaterialForMStage( REngine::GetSceneManager().GetRealtimeRenderingMethod(), index, true, false, bBackgroundCreation );

		CheckAndCreateTechniqueTableOnMaterialForGStage( REngine::GetSceneManager().GetRealtimeRenderingMethod(), index, false, false, bBackgroundCreation );
		CheckAndCreateTechniqueTableOnMaterialForMStage( REngine::GetSceneManager().GetRealtimeRenderingMethod(), index, false, false, bBackgroundCreation );
	}

	return true;
}


void RActorNode::UpdateNoiseRef()
{
	float beforeVR,afterVR;
	bool  beforeV,afterV;

	beforeVR = GetNoiseRefResult();
	beforeV = beforeVR == 1.0f ? true : false;

	RSceneNode::UpdateNoiseRef();

	afterVR = GetNoiseRefResult();
	afterV = afterVR == 1.0f ? true : false;

#ifdef BVH_CAMERA
 	if( beforeV != afterV )
 	{
		if( m_pMeshNode->isRenderAble() )
		{
 			if( m_pActor->IsAttached() && m_pActor->GetRootSceneNode() )	//컬링리스트에서 지우기
 			{
 				m_pActor->GetRootSceneNode()->RefreshRenderingList(m_pActor);
 			}
		}
 	}
#endif
}


void RActorNode::UpdateVisibility()
{
	float beforeVR,afterVR;
	bool  beforeV,afterV;

	beforeVR = GetVisibilityResult();
	beforeV = beforeVR == 1.0f ? true : false;

	RSceneNode::UpdateVisibility();

	afterVR = GetVisibilityResult();
	afterV = afterVR == 1.0f ? true : false;

#ifdef BVH_CAMERA
	if( beforeV != afterV )
	{
		if( m_pMeshNode->isRenderAble() )
		{
			if( m_pActor->IsAttached() && m_pActor->GetRootSceneNode() )	//컬링리스트에서 지우기
			{
				m_pActor->GetRootSceneNode()->RefreshRenderingList(m_pActor);
			}
		}
	}
#endif
}

bool RActorNode::IsVertexMotionBlur( void )
{
	return m_pActor->IsVertexMotionBlur();			
}

float RActorNode::GetMotionBlurBasisFrame( void )
{
	return m_pActor->GetMotionBlurBasisFrame();		
}

bool RActorNode::IsAccumMotionBlur( void )
{
	return m_pActor->IsAccumMotionBlur();			
}

bool RActorNode::IsPixelMotionBlur( void )
{
	return m_pActor->IsPixelMotionBlur();			
}

bool RActorNode::IsPEEdge( void )
{
	return m_pActor->IsPEEdge();
}


void RActorNode::RenderAlpha( RCameraSceneNode *pCamera,int iSubMaterial, RRENDERINGMETHOD eMothod)
{
	RPFC_THISFUNC;

	RActorNodeRenderStrategyAlpha RenderStrategy;

	if(iSubMaterial<0)
		RenderSingle( pCamera, &RenderStrategy );
	else
		RenderSingleOneMaterial( pCamera, iSubMaterial, &RenderStrategy );	
}

void RActorNode::UpdateFinalResultTransforms()
{
#ifdef USING_RSC_PHYSIQUE_QT
	if(IsUseForPhysiqueBone())
	{
		m_qRotResult = GetWorldRotation() * m_pActor->GetWorldInverseRotation();
		// V = ( PR * LT * S) + PT
		// 언유니폼 스케일링이 되어야 하는데 대표적으로 높이를 쓰면 될 것 같으므로 z컴포넌트를 쓴다.
		m_vTranResult = ( m_pActor->GetWorldInverseRotation() * ( GetWorldTranslation() * m_pActor->GetWorldInverseScale().z)) + m_pActor->GetWorldInverseTranslation();
		//m_vScaleResult = ( m_pActor->GetWorldInverseRotation() * GetWorldScale() * m_pActor->GetWorldInverseScale().z) * m_pActor->GetWorldInverseScale();
		
		//m_vScaleResult.z = GetWorldScale().z * m_pActor->GetWorldInverseScale().z;
		m_vScaleResult.z = GetWorldScale().z * m_pActor->GetWorldInverseScale().z;
	}
	else
#endif
	{
		// result 트랜스폼은 tm이 바뀌지 않은 노드는 생략가능할수 있겠다.
		m_matResult = GetWorldTransform() * m_pActor->GetWorldInverseTransform();
	}
}

void RActorNode::RenderAllNodePrimitive()
{
	RPFC_THISFUNC;

	bool bVertexAni = m_pActor->IsUsingVertexAnimation();
	if(bVertexAni)
		m_pMeshNode->m_pVb->RenderIndexSoft( 
		&m_pMeshNode->m_pIb->m_pIndex[0],
		m_pMeshNode->m_nFaceCnt,
		m_pMeshNode->m_pIb->m_PrimitiveType);
	else
		m_pMeshNode->m_pVb->RenderIndexBuffer( 
		0,
		m_pMeshNode->m_nFaceCnt,
		m_pMeshNode->m_pIb->m_PrimitiveType);

	REngine::GetResourceProfiler().AddPolygon( PPT_ACTOR, m_pMeshNode->m_nFaceCnt );
	REngine::GetResourceProfiler().AddDrawCall( PPT_ACTOR, 1);
	REngine::GetResourceProfiler().AddMeshProfileInfo(m_pMeshNode, -1);
}

void RActorNode::SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj)
{
	RShaderCompositeManager::SetTransformConstant(GetWorldTransform(), matView, matViewProj);
}

// void RActorNode::UpdateTechniqueTableForGStage(int nSubMaterialID, bool bInstancing)
// {
// 	RSCID rscComponents =m_vecGStageRSCIDs[nSubMaterialID];
// 	
// 	if( bInstancing)
// 		rscComponents |= RSC_INSTANCING;	
// 	else
// 		rscComponents &= ~RSC_INSTANCING;
// 
// 	if(rscComponents != m_vecGStageRSCIDs[nSubMaterialID])
// 	{
// 		RShaderComposite* pActorShader = m_pActor->GetShaderManager()->CreateShaderAsync( rscComponents );
// 		m_vecGStageShaders[nSubMaterialID] = pActorShader;
// 		m_vecGStageRSCIDs[nSubMaterialID] = rscComponents;
// 	}
// }

// void RActorNode::UpdateTechniqueTableForMStage(int nSubMaterialID, bool bInstancing)
// {
// 	RSCID rscComponents =m_vecMStageRSCIDs[nSubMaterialID];
// 
// 
// 	//Instancing
// 	if( bInstancing)
// 		rscComponents |= RSC_INSTANCING;	
// 	else
// 		rscComponents &= ~RSC_INSTANCING;
// 
// 
// 	//Diffuse Blending
// 	if( m_bBlendingMaterial && ( (RSC_DIFFUSEMAP & rscComponents) > 0 ) )
// 		rscComponents |= RSC_DIFFUSEMAP_BLEND;
// 	else
// 		rscComponents &= ~RSC_DIFFUSEMAP_BLEND;
// 
// 	if(rscComponents != m_vecMStageRSCIDs[nSubMaterialID])
// 	{
// 		RShaderComposite* pActorShader = m_pActor->GetShaderManager()->CreateShaderAsync( rscComponents );
// 		m_vecMStageShaders[nSubMaterialID] = pActorShader;
// 		m_vecMStageRSCIDs[nSubMaterialID] = rscComponents;
// 	}
// }

const RSCID& RActorNode::GetShaderIDGStage(int nSubMaterialID) const
{
	return m_vecGStageRSCIDs[nSubMaterialID];
}

const RSCID& RActorNode::GetShaderIDMStage(int nSubMaterialID) const
{
	return m_vecMStageRSCIDs[nSubMaterialID];
}

bool RActorNode::IsApplyMaterialDecal(bool bTexCoord, RMaterial* pMaterial)
{
	if(bTexCoord==false)
		return false;

	return m_pActor->GetMaterialDecal()->HaveDecalLayer(pMaterial->GetName()) && pMaterial->GetCompositeLayer(RTT_DIFFUSE)==NULL;
}

}
