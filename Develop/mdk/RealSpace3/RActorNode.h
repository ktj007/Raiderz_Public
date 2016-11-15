#pragma once

#include "RTypes.h"
#include "RSceneNode.h"
#include "RShaderComponentID.h"
#include "RLoadingWorkUnit.h"
#include "RMaterialResource.h"
#include "RResourceLoader.h"
#include "MMemPool.h"
//#include "RTechnique.h"
#include "RStlCustomAlloc.h"


namespace rs3 {

class RMeshNode;
class RActor;
class RActorPassRenderer;
class RVertexMotionBlurPassRenderer;
class RPixelMotionBlurPassRenderer;
class RAccumMotionBlurPassRenderer;
class RActorCollision;
class REnvironmentSet;
class RAnimation;
class RAnimationNode;
class RActorController;
class RActorNodeController;
// class RAnimationController;
// class RAnimationControllerNode;
class RActorAlignController;

class RLightMapPacker;
class REffectSceneNode;
class RMaterialResource;
class RShaderFX;
class MCollision;
class RActorNodeConstraintController;
class RAdditivePassRenderer;
class RActorNodeRenderStrategy;
struct ComponentDesc_GStage;
struct ComponentDesc_MStage;

enum RRENDERINGMETHOD;

RS_API extern const char *RSID_ACTORNODE;

///////////////////////////////////////////////////
// 
typedef list<RActorNodeController*> RACTORNODECONTROLLERLIST;

enum ACTOR_NODE_TYPE
{
	ACTOR_NODE_COMMON = 0,
	ACTOR_NODE_PHYSIQUE,
	ACTOR_NODE_CLOTH,

	ACTOR_NODE_TYPE_NUM
};


enum PIXEL_MOTION_BLUR_MODE
{
	PMB_NONE = 0,
	PMB_HALF,
	PMB_FULL,
	PMB_MODE,
};

class RS_API RActorNode : public RSceneNode, public RResource::LoadingController
{
	MDeclareRTTI;

	friend RActor;
	friend RActorPassRenderer;
	friend RVertexMotionBlurPassRenderer;
	friend RPixelMotionBlurPassRenderer;
	friend RAccumMotionBlurPassRenderer;
	friend RActorController;
	friend RAdditivePassRenderer;
// 	friend RAnimationController;
// 	friend RAnimationControllerNode;
public:
	DECLARE_NODE_ID_STRING(RSID_ACTORNODE);

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	bool							Create();				///< 공통, Device Independent
	void							Destroy();
	virtual void					OnDestroy();

	virtual void					OnAdd();				// 액터에 더해지거나 빠질때 불리우는 펑션
	virtual void					OnRemove();

	//////////////////////////////////////////////////////////////////////////
	// 로딩
	//
	virtual bool					OnCreateDeviceIndependentPart();	///< 노드마다 다른 생성
	virtual bool					OnCreateDeviceDependentPart();		///< 디바이스 종속적인 리소스 만들기
	bool							OnCreateMaterials(bool bBackground = false);	///< Material 생성
	virtual bool					IsLoadingCompleted();
//	void							UpdateLoadingPriority();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 액터노드 속성
	//
	ACTOR_NODE_TYPE					GetActorNodeType()	{ return m_eActorNodeType; }

	// BoneReferenceCount 관리
	bool							CreateBoneReferenceTable();
	void							DestroyBoneReferenceTable();
	int								GetBoneRefCount()	{ return m_nBoneRefCount; }
//	float							GetBoneImportance()	{ return m_fBoneImportance; }

	// 렌더링 액터노드일 경우
	bool							HasRefBone()		{ return (m_refBones.empty() == false); }

	virtual bool					Validate();						// material, 뼈 등을 검사하여 이상이 있는지 리턴
	virtual bool					GetVisible()	const	override;
	virtual bool					GetUsable()		const	override;	

	//////////////////////////////////////////////////////////////////////////
	// Parts Color
	void							SetPartsColor(DWORD dwPartsColor);
	DWORD							GetPartsColor();

	/////////////////////////////////////////////////////////////////////////
	// Noise Fade out
	void							SetNoiseRefColor1( float fSize, DWORD dwSectionColor);
	void							SetNoiseRefColor2( float fSize, DWORD dwSectionColor);
	bool							IsApplyNoise() { return m_fNoiseRefResult<1.0f; }

	// 애니메이션 컨트롤 관련
	const RMatrix&					GetResultTransform();				// 현재 애니메이션 결과 행렬 얻는다
	const RQuaternion&				GetResultRotation();
	const RVector3&					GetResultTranslation();
	const RVector3&					GetResultScale();
	RQuaternion						GetRotation();						// 현재 애니메이션 회전값을 얻는다

//	RAnimationControllerNode*		GetAnimationNodeController() { return m_pAnimationController; }

	const string&					m_strName;				// 디버그창에서 보기 쉽도록 만들어놓은것 필요없음
	RMeshNode*						m_pMeshNode;					// 자신과 연결된 노드. RActor::NewActorNode(). 체인 구성의 key 성분 중 하나로도 사용 됨.
//	RActorNode*						m_pParent;						// 부모
//	RActorNode*						m_pCurrentParent;				// 현재애니메이션의 부모, 특정 애니메이션에서는 달라질수도 있다
//	vector<RActorNode*>				m_refBones;						// 참조하는 뼈 테이블
	vector<RActorNode*,RStlCustomAlloc<RActorNode*>>				m_refBones;	// 참조하는 뼈 테이블
	vector<RMatrix,RStlCustomAlloc<RMatrix>>					m_matBones;						// 뼈 매트릭스
	vector<RMatrix,RStlCustomAlloc<RMatrix>>					m_matBoneBases;					// 나의 base * 뼈 inv base
#ifdef USING_RSC_PHYSIQUE_QT
	
	vector<RQuaternion,RStlCustomAlloc<RQuaternion>>				m_quatBoneBaseRots;					// 나의 base * 뼈 inv base -> 뼈 매트릭스에 대한 쿼터니언 
	vector<RVector4,RStlCustomAlloc<RVector4>>				m_vBoneBaseTrans;					// 나의 base * 뼈 inv base -> 뼈 매트릭스에 대한 이동값 	Vector4로 변경 - 090722, OZ
	vector<RVector3,RStlCustomAlloc<RVector3>>				m_vBoneBaseScales;					// 나의 base * 뼈 inv base -> 뼈 매트릭스에 대한 스케일값 
#endif

#ifdef USING_RSC_PHYSIQUE_QT
	vector<RQuaternion,RStlCustomAlloc<RQuaternion>>		m_quatBoneRots;					// 뼈 매트릭스에 대한 쿼터니언 
	vector<RVector4,RStlCustomAlloc<RVector4>>				m_vBoneTrans;					// 뼈 매트릭스에 대한 이동값 	Vector4로 변경 - 090722, OZ
	vector<RQuaternion,RStlCustomAlloc<RQuaternion>>		m_quatBoneRotsPrev;				// 뼈 매트릭스에 대한 쿼터니언 
	vector<RVector4,RStlCustomAlloc<RVector4>>				m_vBoneTransPrev;				// 뼈 매트릭스에 대한 이동값 	Vector4로 변경 - 090722, OZ
#endif
#ifdef USING_RSC_PHYSIQUE_DQ
	vector<RDQuaternion,RStlCustomAlloc<RQuaternion>>			m_dqBoneDQ;						// 뼈 매트릭스에 듀얼 쿼터니언 
#endif
	// tip : Decompose() 사용



	RActor*							m_pActor;						// frame 과..정보등을 참조하기 위해. 자신의 오너를 참조

	vector<RMaterialResource*,RStlCustomAlloc<RMaterialResource*>>		m_vecMaterialTable;				// 객제별로 다르게 사용하기 위해 MeshNode의 재질 정보를 복사해서 가진다. 

	RActorNode*						GetParentActorNode();

	int								AddBoneRef();
	int								ReleaseBoneRef();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 업데이트 관련
	//
	virtual void					ResetTransform();
	RMatrix							GetBaseLocalTransform();		// 원래 메쉬의 base 트랜스폼을 얻는다
//	void							UpdateWorldTransform();

	virtual bool					OnPick(RPICKINFO& pickInfo);

	void							UpdateLocalAnimation();			// 애니메이션을 업데이트한다
	void							ForceUpdateAnimation();			// 애니메이션을 강제로 (부모를 포함하여) 업데이트한다

	void							ReserveUpdateVertexBuffer(RAnimationNode* pAniNode, int nFrame); // 버텍스 애니메이션이나 soft 스킨등으로 vb 업데이트를 예약

	void							GetLocalTransformAt(DWORD nFrame, RMatrix& outMatrix);	///< 특정 프레임의 local tm 을 얻는다
	void							GetWorldTransformAt(DWORD nFrame, RMatrix& outMatrix);	///< 특정 프레임의 world tm 을 얻는다
	void							GetResultTransformAt(DWORD nFrame, RMatrix& outMatrix);	///< 특정 프레임의 world tm 을 얻는다


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 관련
	//

	/// 렌더링하기 전 vertex 단위의 업데이트가 필요한경우. 한 actornode 렌더링시 한번 불린다
	virtual void					UpdateTransformAndVertex();
	/// vertex buffer, index buffer, vertex format 설정
	virtual void					SetVertexStream(  bool b2ndBufferMng = true);
	/// node 별로 각각 설정 해줘야 할것
	virtual void					SetNodeRenderStateForEdge( RCameraSceneNode* pCurrentCamera);
	virtual void					SetNodeRenderState(const RMatrix& matView, const RMatrix& matViewProj);
	virtual void					SetNodeRenderStateForBlur(  RCameraSceneNode* pCurrentCamera);
	virtual void					SetBlendMaterialRenderState();

	virtual void					RenderPrimitive(int index);		// 마테리얼에 열견된 버텍스의 DP
	
	virtual void					RenderAllNodePrimitive();

	virtual	void					UpdateVisibility() override;

	virtual	void					UpdateNoiseRef() override;

	bool							CheckInstancingCondition();		// 인스턴싱 사용 가능한 조건인지를 체크

	bool							IsVertexMotionBlur(void);		
	float							GetMotionBlurBasisFrame(void);	
	bool							IsAccumMotionBlur(void);			
	bool							IsPixelMotionBlur(void);			
	bool							IsPEEdge(void);

	void							RenderSingle(  RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pRenderStrategy );
	void							RenderSingleOneMaterial( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RActorNodeRenderStrategy* pRenderStrategy);

	bool							IsRenderable(void);
	bool							CheckDoRender();

	bool							PreCreateShader( bool bBackgroundCreation );


	RShaderFX*						CheckAndCreateTechniqueTableOnMaterialForGStage( RRENDERINGMETHOD renderMethod, int nSubMaterialID, bool bInstancing/* = false*/, bool bNoiseRef, bool bBackgroundCreation/* = true*/);
	RShaderFX*						CheckAndCreateTechniqueTableOnMaterialForMStage( RRENDERINGMETHOD renderMethod, int nSubMaterialID, bool bInstancing/* = false*/, bool bNoiseRef, bool bBackgroundCreation/* = true*/);

	RSCID							GetBaseComponent() { return m_baseComponents; }

	ComponentDesc_GStage			GetGStageComponentDesc(int nSubMaterialID, bool bInstancing, bool bNoiseRef);
	ComponentDesc_MStage			GetMStageComponentDesc(bool bAddColorPass, int nSubMaterialID, bool bInstancing, bool bNoiseRef);
	ComponentDesc_MStage			GetMaskingPassComponentDesc(int nSubMaterialID);


public:
	virtual void					RenderNormal(DWORD dwColor = 0xffff0000);	// 디버깅 인포 : 노멀
	virtual void					RenderWire(DWORD dwColor = 0x80ffffff);		// 디버깅 인포 : 와이어
	virtual void					OnRender(RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual void					RenderAlpha( RCameraSceneNode *pCamera, int iSubMaterial,RRENDERINGMETHOD eMothod) override;

	void							RenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, RActorNodeRenderStrategy* pRenderStrategy );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 마테리얼 관련
	//
	bool							GetSubMaterialMask(int nSubMaterial);
	void							SetSubMaterialMask(int nSubMaterial,bool bShow);
	void							SetAllSubMaterialMask(bool bShow);
	RMaterial*						GetSubMaterial(int nIndex) { return m_vecMaterialTable.at(nIndex)->m_pMaterial; }

	// 재질 변경
	bool							m_bBlendingMaterial;			///< 블렌딩이 활성화 되어있는지 여부
	int								m_nBlendingMaterialID;
	RMaterial*						m_pBlengingMaterial;		///< 블랜딩 할 이전 재질
	float							m_fBlendingFactor;			///< DstFactor( Factor ) * AS_BLENDING_MATERIALMAP + SrcFactor( 1 - Factor ) * DIFFUSE
	float							m_fBlendingTime;			///< 총 블렌딩 시간
	float							m_fBlendingAccTime;			///< 누적 시간 
	// !외부에서 직접 사용하는 함수가 아니다 // RActor::ChangeMatetial 로 ActorNode를 통해서 사용하자
	void							ChangeMtrlSet(string strSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);
	void							ChangeMtrlSet(int nSrcMtrl, int nDstMtrl, float fBlendingTime = 0.3f);
	void							ChangeMtrlSet(int nSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);

	bool							HasTranslucentMaterial();	// 반투명 재질을 가지고있나
	bool							HasAlphaMaterial();			// 알파 (additive 및 blending) material을 포함하는가

	bool							IsUsePhysiue();
	bool							IsUseTexCoord();
	bool							IsUseExtraTexCoord();
	bool							IsUsePointColor();

	bool							IsBlendingMaterial()	{ return m_bBlendingMaterial; }

	bool							IsNoShade();
	bool							IsUseAddColor(RMaterial* pMaterial);

	bool							IsAlpha();

	const RSCID&					GetShaderIDGStage(int nSubMaterialID) const;
	const RSCID&					GetShaderIDMStage(int nSubMaterialID) const;

	float							GetIlluminationScale();



	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 관련된 ActorNodeConstraintController
	//
 	void							RemoveRelatedController( RActorNodeConstraintController* pController );
 	void							AddRelatedController( RActorNodeConstraintController* pController );

	//////////////////////////////////////////////////////////////////////////
	// 업데이트
	void							UpdateFinalResultTransforms();

	bool							GetAddColorState()	{ return m_bAddColor; }

	virtual void					SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj);

	bool							CanInstancing() { return m_bCanInstancing; }

protected:
	explicit						RActorNode(ACTOR_NODE_TYPE eActorNodeType);
	virtual ~RActorNode();

	bool							PickLocal(const RVector &origin,const RVector &to,RPICKINFO *pPickInfo);

	RMatrix							m_matResult;			// 현재 애니메이션의 부모로부터 곱해진 행렬
#ifdef USING_RSC_PHYSIQUE_QT
	RVector3						m_vTranResult;
	RQuaternion						m_qRotResult;
	RVector3						m_vScaleResult;
#endif

	vector<RSCID,RStlCustomAlloc<RSCID>> m_vecGStageRSCIDs;
	vector<RSCID,RStlCustomAlloc<RSCID>> m_vecMStageRSCIDs;

	
	RSCID							m_baseComponents;		///< 형체를 그리기 위한 기본 쉐이더 콤포넌트

	DWORD							m_dwPartsColor;			///< Parts Color

	DWORD							m_dwNoiseRefColor1;
	float							m_fNoiseRefSection1;
	DWORD							m_dwNoiseRefColor2;
	float							m_fNoiseRefSection2;

	bool							m_bCanInstancing;

//	RShaderFX*						GetShader(unsigned int nMtrlIndex, bool bMstage);
	void							GetMeshCollision(std::vector<MCollision*>& _rOut, RActorCollision* _pCollsion);

	void							EnableAddColorEffect(const RVector3& color);
	void							DisableAddColorEffect();


	//void							UpdateTechniqueTableForGStage(int nSubMaterialID, bool bInstancing);
	//void							UpdateTechniqueTableForMStage(int nSubMaterialID, bool bInstancing);

//	virtual void					OnAddedToActor();							///< 액터노드에 더해질때

private:

	bool							m_bAddColor;
	RVector3						m_vAddColor;


	bool							m_bCreated;
	ACTOR_NODE_TYPE					m_eActorNodeType;
	vector<bool,RStlCustomAlloc<bool>>					m_subMaterialMask;

	int								m_nBoneRefCount;		// bone 레퍼런스 카운트
//	float							m_fBoneImportance;		// 뼈의 중요도

//	RAnimationControllerNode*		m_pAnimationController;
	RSceneNodeController*			m_pAlignController;
	int								m_nUpdateVertexBufferFrame;	// vertex ani 업데이트 해야 할 프레임
	RAnimationNode*					m_pUpdateVertexAniNode;		// vertex 업데이트를 받아 올 AnimationNode

	RActorNode*						m_pNextRenderChain;		// 다음 렌더체인
	std::list<RActorNodeConstraintController*,RStlCustomAlloc<RActorNodeConstraintController*>>	m_relatedControllers;	// 연관되어있는 콘트롤러들

	float							m_fIlliminationScale;	// Self illumination Scale
	void							SetIlluminationScale(const float fScale);	

	bool							CheckRenderPerMaterialToShadowMap();
	void							SetAddEffectComponents( RSCID& _rscComponents );

	bool							IsApplyMaterialDecal(bool bTexCoord, RMaterial* pMaterial);
};

//----------------------------------------------------------------------------------------------------
inline const RMatrix& RActorNode::GetResultTransform()		{ return m_matResult; }
#ifdef USING_RSC_PHYSIQUE_QT
inline const RVector3& RActorNode::GetResultTranslation()	{ return m_vTranResult; }
inline const RQuaternion& RActorNode::GetResultRotation()	{ return m_qRotResult; }
inline const RVector3& RActorNode::GetResultScale()			{ return m_vScaleResult; }
#endif
//----------------------------------------------------------------------------------------------------



inline bool RActorNode::GetSubMaterialMask(int nSubMaterial)
{
	if( m_subMaterialMask.empty() )
//		return true;	// 기본적으로 true를 리턴하면 백그라운드 로딩 문제로 그리다 뻑남
		return false;

	if(nSubMaterial<0)
		return true;

	return m_subMaterialMask[nSubMaterial];
}
//----------------------------------------------------------------------------------------------------
inline void RActorNode::SetPartsColor(DWORD dwPartsColor)	{	m_dwPartsColor = dwPartsColor;	}
inline DWORD RActorNode::GetPartsColor()					{	return m_dwPartsColor;	}
//----------------------------------------------------------------------------------------------------
inline void RActorNode::SetNoiseRefColor1( float fSize, DWORD dwSectionColor)	{ m_dwNoiseRefColor1 = dwSectionColor; m_fNoiseRefSection1 = fSize; }
inline void RActorNode::SetNoiseRefColor2( float fSize, DWORD dwSectionColor)	{ m_dwNoiseRefColor2 = dwSectionColor; m_fNoiseRefSection2 = fSize; }
//----------------------------------------------------------------------------------------------------
inline void RActorNode::ReserveUpdateVertexBuffer(RAnimationNode* pAniNode, int nFrame)	// 버텍스 애니메이션이나 soft 스킨등으로 vb 업데이트를 예약
{
	m_nUpdateVertexBufferFrame = nFrame;
	m_pUpdateVertexAniNode = pAniNode;
}
//----------------------------------------------------------------------------------------------------
inline RActorNode* RActorNode::GetParentActorNode() 
{
	_ASSERT(m_pParent!=NULL);
	if((RSceneNode*)m_pActor==m_pParent)
		return NULL;
	return (RActorNode*)m_pParent;
}

inline void RActorNode::SetIlluminationScale(const float fScale)	{ m_fIlliminationScale = fScale; }
//----------------------------------------------------------------------------------------------------
inline float RActorNode::GetIlluminationScale()						{ return m_fIlliminationScale; }


class RS_API RActorNodeMLeaf : public RActorNode, public MMemPool<RActorNodeMLeaf, 50000>
{
	MDeclareRTTI;
public:
	RActorNodeMLeaf(ACTOR_NODE_TYPE eActorNodeType) : RActorNode(eActorNodeType) {}
	virtual ~RActorNodeMLeaf() {}
};

}
