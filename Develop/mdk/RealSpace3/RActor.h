#ifndef _RActor_h
#define _RActor_h

#include "RVType.h"
#include "RSceneNode.h"
#include "RSkeleton.h"
#include "RActorNode.h"
#include "RAnimationController.h"
#include "RResource.h"
#include "RLODContext.h"
#include "MemPool.h"
#include "RMaterialDecal.h"
#include <hash_map>

namespace rs3 {

RS_API extern const char *RSID_ACTOR;
RS_API extern const char *RSID_EFFECT; 

class RActorNode;
class RPhysXClothActorNode;
class RSkeleton;
class RActorController;
// class RBlendAnimationController;
class RActorPassRenderer;
class RVertexMotionBlurPassRenderer;
class REdgePassRenderer;
class RPixelMotionBlurPassRenderer;
class RAccumMotionBlurPassRenderer;
class RSceneManager;
class RShaderEffectController;
class RMesh;
class RMeshNode;
class RCullingMgr;

class REngine;

class RListener; // 애니메이션 이벤트
class RDistortionParameter;

typedef list<RActorController*> RACTORCONTROLLERLIST;
typedef map<string,RActorNode*> RACTORNODEMAP;

// TODO: 제거 가능
class REffectSceneNode;
class RSceneSky;
class RActorNodeBuilder;

enum RANIMATIONTRANSITIONTYPE;


// COMMENT OZ - 갓츄!! 캐릭터 등 모든 애니메이션이 가능한 메시
////////////////////////////////////////////////////////////////////////////////////////////////////
// 캐릭터 등 모든 애니메이션이 가능한 메시
//

class RS_API RActor : public RSceneNode, public RResource::LoadingController
{
	friend RActorNode;
	friend RActorController;	// for RefreshAABB
	friend RActorPassRenderer;	// for render factor setting
	friend RVertexMotionBlurPassRenderer;
	friend REdgePassRenderer;
	friend RPixelMotionBlurPassRenderer;
	friend RAccumMotionBlurPassRenderer;
	friend RSceneManager;		// for shader creation
	friend REngine;
	friend RCameraSceneNode;
	friend RCullingMgr;

	// 지울 액터노드에 딸린 액터노드(자기자신 포함)들을 분류하는 비지터
	class CActorNodeDeletionVisitor : public RSceneNodeVisitor
	{
	public:
		CActorNodeDeletionVisitor(RActor* _pCaller) : m_pCaller(_pCaller) {}
		virtual ~CActorNodeDeletionVisitor(){}

		// post order 로 traversal 할꺼임
		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode ){ return true; }
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode )
		{
			RActorNode* pActorNode = static_cast<RActorNode*>(pSceneNode);
			if (pActorNode->m_pActor == m_pCaller)
				m_vecActorNodesToDelete.push_back(pActorNode);
		}

		std::vector<RActorNode*>& GetDeleteNodes() { return m_vecActorNodesToDelete; }

	private:
		std::vector<RActorNode*> m_vecActorNodesToDelete;
		RActor* m_pCaller;
	};

public:
	class RS_API RActorNodeBuilder
	{
	public:
		struct RADDMESHNODE {
			RMeshNode*	pMeshNode;
			RActorNode* pParentNode;
			const char* szActorNodeName;
			bool		bBackground;
		};

		RActorNodeBuilder(RActor* pActor);
		~RActorNodeBuilder();

		void DeleteActorNode(RActorNode* pActorNode);
		void DeleteActorNode(const char* szNodeName);
		void AddMeshNode(RMeshNode* pMeshNode, RActorNode* pParent, const char* szActorNodeName = NULL, bool bBackground = false);

		void Build();

	protected:
		RActor* m_pActor;

		std::vector<RADDMESHNODE*>	m_vecAddMeshList;
		std::vector<RActorNode*>	m_vecDeleteList;
	};
	
	// how to use
	/*
	RActor actor;
	RActor::RActorNodeBuilder builder(&actor);
	builder.DeleteActorNode("actorNodeName1");
	builder.DeleteActorNode("actorNodeName2");
	builder.DeleteActorNode(pActorNode);
	builder.AddMeshNode(pMeshNode1, pParent, ...);
	builder.AddMeshNode(pMeshNode2, pParent, ...);
	builder.AddMeshNode(pMeshNode3, pParent, ...);
	builder.Build();
	*/


public:
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_ACTOR);

	virtual bool IsLoadingCompleted();

protected:
	virtual void				DeleteManagingChildren();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// AutoUpdate 설정을 위한 Context Structure
	//
	struct SAutoUpdateContext
	{
		SAutoUpdateContext()
		{
			m_bMustNotAutoUpdate = false;
			m_bMustAutoUpdate = false;
			m_bPlayingAnimation = false;
			m_nAutoUpdateControllers = 0;
		}

		bool m_bMustNotAutoUpdate;
		bool m_bMustAutoUpdate;
		bool m_bPlayingAnimation;
		int  m_nAutoUpdateControllers;

		bool CheckAutoUpdateContext()
		{
			_ASSERT(m_nAutoUpdateControllers >= 0);

			// AutoUpdate를 무조건 안하는경우
			if (m_bMustNotAutoUpdate)
				return false;

			// 조건에 의해 AutoUpdate를 안하는 경우
			if (m_bMustAutoUpdate == false &&
				m_bPlayingAnimation == false &&
				m_nAutoUpdateControllers == 0 )
			{
				return false;
			}

			// AutoUpdate
			return true;
		}

	} m_AutoUpdateContext;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 환경관련 설정 팩터
	//
	RVector4					m_vMultiplyColor;				///< Color Factor


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 액터노드
	//
	stdext::hash_map<string,RActorNode*>		m_actorNodeMap;					///< RActorNode 이름순 정렬
	vector<RActorNode*>			m_actorNodes;					///< RActorNode 계층관계순 정렬
	vector<RActorNode*>			m_sortedRenderableActorNode;	///< physique - phsyique_alpha - non_physique - non_phsyique_alpha
	vector<RActorNode*>			m_animationReferencedNodes;		///< 애니메이션에 참조되는 노드 셋을 담고있다


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 기본 애니메이션 / 업데이트 관련
	//
	RAnimationController	m_animationController;			///< 기본 애니메이션 콘트롤러
// 	RANIMATIONTRANSITIONTYPE	m_transitionType;
// 	RBlendAnimationController*	m_pTransitionController;		///< 애니메이션의 전환을 위해 블렌드 콘트롤러는 항상 하나 가지고있다.

	RMatrix						m_matInvWorld;					///< inverse of world
#ifdef USING_RSC_PHYSIQUE_QT
	RQuaternion					m_qInvWorldRot;					///< m_matInvWorld의 값
	RVector3					m_vInvWorldTran;				///< m_matInvWorld의 값
	RVector3					m_vInvWorldScale;				///< m_matInvWorld의 값 
#endif
	int							m_nResultTransformUpdatedFrame;	///< UpdateFinalResultTransforms 된 프레임 ( 한프레임에 여러번 업데이트 방지한다 )
	int							m_nAnimationFrameUpdatedFrame;	///< animation frame 이 여러번 업데이트되는걸 방지한다

	RListener*					m_pListener;					///< 애니메이션 이벤트를 받을 리스너


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 모든 액터에 적용가능한 렌더스테이트 설정
	//
//	static void					SetGlobalActorRenderState();	///< 전역적인 렌더스테이트 설정(fog 등)

	bool						m_bUseShade;					///< turn off shade 때문에 만들어놓은것
	bool						m_bUsingActorPassRenderer;		///< 액터 랜더 패스랜더러 사용여부
	bool						m_bUsingEnvironmentTiling;		///< 환경 구름 그림자 사용여부
	bool						m_bAlpha2PassRendering;			///< Alpha, Additive 마테리얼일 경우 알파 깨짐 방지를 위해 2패스로 그릴 때

	bool						m_bVertexMotionBlur;			// 버텍스 모션 블러 여부
	bool						m_bEdge;						// 외곽선 여부
	float						m_fEdgeTickNess;				// 외곽선 두께, 거리에 영향을 받지 않음
	bool						m_bPEEdge;						// 포스트 이펙트 외곽선 여부
	bool						m_bPEEdgeProj;					// 포스트 이펙트 외곽선 투시되는 여부				
	RVector3					m_vEdgeColor;									
	float						m_fMotionBlurBasisFrame;
	bool						m_bAccumMotionBlur;				// 누적 모션 블러 여부
	int							m_nPixelMotionBlur;				// 픽셀 모션 블러 여부

	float						m_fDistanceToCamera;			///< 카메라 거리, OnPreRender()에서 업데이트된다.

	bool						m_bExistCollisionTree;				///< (맵툴 등에서) 저장시에 충돌 생성 여부
	vector<RCapsule*>			m_vecColCapsule;				///< 세부 충돌 검사

	
	bool						m_bUseDoubleShadow;				///< 이중 그림자 적용 여부
	bool						m_bAOEnable;					///< AO 적용 여부

	// Max Ani BoundingBox
	bool						m_bIsMaxAniAABB;				///< Max Ani BoundingBox 정보가 있는가 // EXPORTER_ANI_VER8 추가 사항
	MBox						m_aabbLocal;					///< local bounding box

	bool						m_bShadowReceiver;

	RMaterialDecal				m_MaterialDecal;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 저장
	//
	virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void				SaveToXML(MXmlElement &element);

	friend class				RSyncCreateDeviceDependedPart;
	friend class				RAsyncCreateDeviceIndependedPart;
	virtual void				CreateDeviceIndependentPart(RMesh* pMesh);	//디바이스에 의존적인 부분과 비의존적인 부분으로 나누어 생성
	virtual void				CreateDeviceDependentPart(RMesh* pMesh);
	void						CreateMaterials(bool bBackground);

	// 백그라운드 로딩 콘트롤
	virtual void				OnLoadingComplete(RResource*);
	virtual int					CalcPriority();
	virtual bool				Load();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Update
	//
	virtual void				OnUpdate();
	virtual void				OnUpdateTransform() override;
	virtual void				OnUpdateAABB() override;
	void						UpdateAABB();
	RBoundingBox				ForceUpdateAABB(bool bBillboard = false);	///< Actor와 ActorNode의 AABB를 수집한다.


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 / 렌더링관련 업데이트
	//
	void						UpdateForRender(void);			///< 렌더링을 하기 위해 필요한 업데이트
	void						UpdateAnimation();				///< 애니메이션 프레임 업데이트
	void						UpdateMaterialAnimation();		///< material uv ani 프레임 업데이트
	bool						IsRenderable( RActorNode* pActorNode ); ///< 렌더링 가능한 액터노드인지 체크

	virtual void				OnPreRender( RCameraSceneNode* pCurrentCamera) override;
	virtual void				OnRender(RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override; // 패스렌더러를 쓰면 OnRender 계열의 함수는 불리지 않는다.
	void						RenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, RActorNodeRenderStrategy* pRenderStrategy );
	
protected:
	virtual void				RenderAlpha( RCameraSceneNode *pCamera,int iSubMaterial, RRENDERINGMETHOD eMothod) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 피킹 / 캡슐충돌
	//
	virtual bool				OnPick(RPICKINFO& pickInfo);
	void						UpdateColCapsule();				///< 캡슐형 충돌 처리(세부 충돌)


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메시정보가 다시 변경될 필요가 있을때
	//
	void						OnRestore();
	void						OnInvalidate();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링과 관련된 정보를 (다시)만든다 : sortnodes & lod boneset(s)
	//
	void						CreateRenderInfoAndNotifyToController(bool _bNotifyToController = true);
	void						DeleteActorNodeAndNotifyToController(RActorNode* pActorNode, bool _bNotifyToController = true);


	//////////////////////////////////////////////////////////////////////////
	// 파츠 교환
	//
	RActorNode*					NewActorNode(RMeshNode* pMNode);
	void						AddActorNode(RActorNode* pActorNode, RSceneNode* pParent);

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션 업데이트 / 이벤트
	//
	void						UpdateAnimationFrameAndEvent();
//	void						CheckAnimationEventSend();

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션 컨트롤러 강제설정 ForTest
	//
	void						SetForceAnimationControlPlayDoneForTest(bool bPlayDone);
	void						SetForceAnimationControlLoopForTest(bool bLoop);

	//////////////////////////////////////////////////////////////////////////
	// 렌더링리스트에 추가
	virtual void				AddToRenderingList(RRenderingList* pRenderingList)		override;
	virtual void				RemoveFromRenderingList(RRenderingList* pRenderingList)	override;
	

public:

	RActor(RRENDERPASS eRenderPass = RRP_ACTOR); 
	virtual ~RActor();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual bool				Create(const char* szMeshName, bool bBackgroundCreation = false);
	virtual bool				Create(RMesh* pMesh, bool bBackgroundCreation = false, bool bLoadMonitoring = false);

	virtual void				Destroy();

	virtual void				OnActorLoadComplete();
	virtual void				OnAutoAnimationLoaded();

	void						ChangeMatetial(string strActorNode, string strSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);
	void						ChangeMatetial(string strActorNode, int nSrcMtrl, int nDstMtrl, float fBlendingTime = 0.3f);
	void						ChangeMatetial(string strActorNode, int nSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);
	void						ChangeMatetial(int nActorNodeIndex, string strSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);
	void						ChangeMatetial(int nActorNodeIndex, int nSrcMtrl, int nDstMtrl, float fBlendingTime = 0.3f);
	void						ChangeMatetial(int nActorNodeIndex, int nSrcMtrl, string strDstMtrl, float fBlendingTime = 0.3f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Node , Decorator 관리
	//
	int							GetActorNodeCount() const;									///< 배열 크기
	vector<RActorNode*>&		GetActorNodes(){return m_actorNodes;}			///< 애니메이션에 참조되는 bone set을 리턴
	RActorNode*					GetActorNode(const char* szNodeName);						///< 이름으로 노드를 얻는다
	bool						GetNodePosition(char* nodename, RVector& vPos);				///< 노드위치값을 얻어온다. 노드를 순차검색하므로 차후 최적화해야함. - bird
	vector<RActorNode*>&		GetAnimationReferencedNodes() { return m_animationReferencedNodes; }

	RActorNode*					AddMeshNode(RMeshNode* pMeshNode, RActorNode* pParent, const char* szActorNodeName = NULL, bool bBackground = false, bool bNotifyToController = true);		///< 파츠를 추가 ( 같은 이름이 있으면 교체 ) 한다
	bool						DeleteActorNode(const char* szNodeName, bool bNotifyToController = true);

	bool						HasClothActorNode(void);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메시 관련
	//
	string						GetMeshName();
	RMesh*						m_pMesh;				// 연결된 모델정보.
	RMesh*						m_pLowPolyMesh;
	void						LinkLODMetricInfo();

	//////////////////////////////////////////////////////////////////////////
	// update for render 관련
	//
	bool						CheckAlreadyUpdatedForRender();
	void						AddUpdateForRenderRefCount(){ m_nUpdateForRenderRefCount++; }
	void						ReleaseUpdateForRenderRefCount(){ m_nUpdateForRenderRefCount--; }
	int							GetUpdateForRenderRefCount() const { return m_nUpdateForRenderRefCount; }
	void						UpdateAnimationReferenceNodes();	///< 실제로 렌더링에 쓰이는 애니메이션 뼈대 행렬 계산

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation 관련
	//
	bool						SetAnimation( const char* szAnimation ); ///< 애니메이션 변경, 애니메이션 전환 데이터를 RAnimationBlendTable에서 얻는다.
	bool						SetAnimation( RAnimation* pAnimation ); ///< 애니메이션 변경, 애니메이션 전환 데이터를 RAnimationBlendTable에서 얻는다.
// 	bool						SetAnimation( const char* szAnimation, RANIMATIONTRANSITIONTYPE transType, float fTransitionTime = 0.1f ); ///< 애니메이션 변경
// 	bool						SetAnimation( RAnimation* pAnimation, RANIMATIONTRANSITIONTYPE transType, float fTransitionTime = 0.1f ); ///< 애니메이션 변경

	RAnimation*					GetAnimation(const char *szAnimationName);	///< 애니메이션을 얻어낸다
	RAnimation*					GetAnimation();							///< 등록된 에니메이션을 얻어온다.
//	RANIMATIONTRANSITIONTYPE	GetAnimationTransitionStatus();			///< 애니메이션 전환의 상태
	RAnimationController&	GetAnimationController();

	void						SetSpeed(float s=1.f);					///< 에니메이션 속도를 변경한다. ( 1.0f = 1배속 )
	float						GetSpeed();

	void						Play();									///< 애니메이션 재생
	void						Stop();									///< 애니메이션 stop
	void						Pause();								///< 애니메이션 pause

	bool						IsPlayDone();							///< 플레이중인지 여부
	bool						IsOncePlayDone();						///< 에니메이션이 최소한 한번 플레이 되었는지 여부

	void						SetFrame(unsigned int nFrame);			///< 애니메이션 특정 프레임으로 세팅
	int							GetFrame();

	void						SetListener(RListener* pListener);		///< event listener
	RListener*					GetListener();

	vector<RActorNode*>&		GetAnimationReferrencedNodes();			///< 애니메이션에 참조되는 bone set을 리턴
	vector<RActorNode*>&		GetSortedRenderableNodes();				///< 정렬된 렌더링 노드들
	const RMatrix&				GetWorldInverseTransform();				///< 월드의 역행렬
 	const RQuaternion&			GetWorldInverseRotation();				///< 월드의 역행렬 관련 
 	const RVector3&				GetWorldInverseTranslation();			///< 월드의 역행렬 관련 
 	const RVector3&				GetWorldInverseScale();					///< 월드의 역행렬 관련 

	void						LoadAutoAnimation();					///< 자동적으로 수행되는 애니매이션 로딩
	void						PlayAutoAnimationRandom();				///< 무한히 자동적으로 수행되는 애니매이션 플레이
	bool						m_bAutoAnimationEnable;					///< 로딩된후로부터 자동적으로 애니매이션을 수행하는지 여부
	RAnimation*					m_pAutoAnimation;						///< 자동 로딩된 애니메이션

	bool						SetLocalAABB( const RBoundingBox* pAABB );

	// 샘플링 테스트
	void						GetSamplingNodeData(const char* szNodeName, unsigned int nStartFrame, unsigned int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt);

	void						SetUseVertexAnimation( bool bUse );		///< 버텍스 애니메이션 사용 여부를 셋팅

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 충돌관련
	//
	bool						GetCollisionCreationEnable() const		{ return m_bExistCollisionTree; }
	void						SetCollisionCreationEnable(bool bMake )	{ m_bExistCollisionTree = bMake; }

	void						CollectStaticCollisionPolygonsDo(RSceneNodePolygonCollector* pCollector, DWORD dwAttribute) const;	///< 실제로 충돌 메시를 리턴하는 작업을 하는 함수
	virtual void				CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const override;	///< static 충돌메시를 리턴해준다
	RActorCollision*			m_pCollision;


	//////////////////////////////////////////////////////////////////////////
	// 피킹
	//
	bool						PickActorNode(RPICKINFO& pickInfo);	///< 액터노드들 피킹


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 그림자 관련
	//
	bool						IsShadowReceiver() { return m_bShadowReceiver; }
	void						SetShadowReceiver(bool bEnable) { m_bShadowReceiver=bEnable; }
	void						SetUseDoubleShadow(bool bUse);
	bool						GetUseDoubleShadow()							{ return m_bUseDoubleShadow; }	

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// AO 관련
	//
	void						SetAO(bool bEnable)	{ m_bAOEnable = bEnable; }
	bool						GetAO(void)			{ return m_bAOEnable; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 팩터 관련
	//
	void						SetMultiplyColor(const RVector4 &vColor);
	void						ResetMultiplyColor();
	const RVector4&				GetMultiplyColor();
	bool						IsUseMultiplyColor() { return m_vMultiplyColor != RVector4(1,1,1,1); }

	void						SetIlluminationScale(const float fScale);
	void						SetIlluminationScale(string strActorNode, const float fScale);
	float						GetIlluminationScale(string strActorNode);

	RShaderEffectController*	m_pShaderEffectController;
	RShaderEffectController*	GetShaderEffectController() { return m_pShaderEffectController; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 관련
	//
	bool GetUsingEnvironmentTiling()		{ return m_bUsingEnvironmentTiling; }
	void SetUsingPassRenderer(bool _bUsing ){ m_bUsingActorPassRenderer = _bUsing; }

	void SetAlpha2PassRendering( bool bAlpha2PassRender );
	bool GetAlpha2PassRendering()							{ return m_bAlpha2PassRendering; }


	//void Render();
	void RenderSingle( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD eMothod);
	void RenderSingleAlpha2Pass(  RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD eMothod);

	void SetVertexMotionBlur( bool b);	// 버텍스 모션 블러 On/Off
	bool IsVertexMotionBlur()			{ return m_bVertexMotionBlur;					}	// 버텍스 모션 블러중인지

	void EnableEdge( const RVector3& _color, float _fTickNess = 2.f );				// 외곽선 On/Off
	bool IsEdge()						{ return m_bEdge;								}	// 외곽선 상태인지
	void DisableEdge();


	void EnablePEEdge( const RVector3& _vEdgeColor );				// 외곽선 On/Off
	bool IsPEEdge()						{ return m_bPEEdge;								}	// 외곽선 상태인지
	void DisablePEEdge();


	void SetAccumMotionBlur( bool b);	// 누적 모션 블러 On/Off
	bool IsAccumMotionBlur()			{ return m_bAccumMotionBlur;					}	// 누적 모션 블러중인지

	void SetPixelMotionBlur( int b);	// 픽셀 모션 블러 On/Off
	bool IsPixelMotionBlur()			{ return (bool)(m_nPixelMotionBlur>0);			}	// 픽셀 모션 블러중인지

	void SetMotionBlurBasisFrame( float f)	{ m_fMotionBlurBasisFrame = f;				}
	float GetMotionBlurBasisFrame()			{ return m_fMotionBlurBasisFrame;			}

	void SetVertexMotionBlurBasisFrame( float f)	{ SetMotionBlurBasisFrame(f);		}
	float GetVertexMotionBlurBasisFrame()			{ return GetMotionBlurBasisFrame();	}

	void					SetDistortion(bool bDistortion);
	bool					IsDistortion();
	bool					IsDistortionRenderable();
	RDistortionParameter*	GetDistortionParam();
	void					SetDistortionParam(RDistortionParameter* param);




	RMaterialDecal* GetMaterialDecal() { return &m_MaterialDecal; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 데이터 관련 : 툴에서 호출하기 위한..
	bool ReLoadMesh();
	float		GetEdgeTickNess()		{ return m_fEdgeTickNess; }
	bool		GetPEEdgeProjection()	{ return m_bPEEdgeProj; }
	const RVector3&	GetEdgeColor() const			{ return m_vEdgeColor; }
	void		SetEdgeColor( const RVector3& _vEdgeColor);

	void						EnableAddColorEffect( const char* _szNodeName, const RVector3& _vColor );
	void						DisableAddColorEffect( const char* _szNodeName );
	RBoundingBox				SetForceUpdateAABB()	{ return ForceUpdateAABB(); }

public:
	// 디버그용 렌더링
	void RenderSkeleton();
	void RenderMeshCollision();
	void RenderClothCollision();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 백그라운드 로딩 / 기타
	//
	bool						m_bBackgroundCreation;		///< 백그라운드로 생성되는지 여부
	bool						m_bSceneActor;				///< Import Scene된 액터. 씬 액터는 백그라운드 로딩에 우선순위가 있다.
	int							m_nBGWKPriorityOffset;		///< BackgroundWork Priority Offset;
	DWORD						m_dwBoneColor[RBIPID_END];	///< 디버그용 렌더링할때 색깔

	//////////////////////////////////////////////////////////////////////////
	// STATE
	//
	inline bool IsUseShade() { return m_bUseShade; }

	void						SetEnvironmentTexture( const char* _szEnvTexName );
	RTexture*					GetEnvironmentTexture()		{ return m_pEnvironmentTexture; }
	char*						GetEnvironmentTextureName()	{ return m_szEnvTexName; }
	void						EmptyEnvironment();
	

private:
	void						AddToBVH();
	void						RemoveFromBVH();

	char						m_szEnvTexName[128];
	RTexture*					m_pEnvironmentTexture;
	int							m_nUpdateForRenderRefCount;		///< UpdateForRender 가 필요한 오브젝트

	bool						IsUsingVertexAnimation();
	bool						m_bUsingVertexAnimation;

	bool						DeleteActorNode(RActorNode* pActorNode, bool bNotifyToController = true);
	void						RebuildAnimationOnActorNodeChanged();

	void						PreCreateShader();

	//////////////////////////////////////////////////////////////////////////
	// Builder
	bool						DeleteActorNode(std::vector<RActorNode*> &vecActorNode, bool bNotifyToController = true);	///< 리스트로 삭제
	bool						AddMeshNode(std::vector<RActorNodeBuilder::RADDMESHNODE*>& vecActorNode);


	RDistortionParameter*		m_pDistortionParam;				///< 디스토션 파라메터
};

//----------------------------------------------------------------------------------------------------
inline int RActor::GetActorNodeCount() const					{ return (int)m_actorNodes.size(); }		///< 배열 크기
//----------------------------------------------------------------------------------------------------
inline void RActor::SetListener(RListener* pListener)			{ m_pListener = pListener; }
//----------------------------------------------------------------------------------------------------
inline RListener* RActor::GetListener()							{ return m_pListener; }
//----------------------------------------------------------------------------------------------------
inline void RActor::SetMultiplyColor(const RVector4 &vColor)	{ m_vMultiplyColor = vColor; }
//----------------------------------------------------------------------------------------------------
inline void RActor::ResetMultiplyColor()						{ m_vMultiplyColor = RVector4(1,1,1,1); }
//----------------------------------------------------------------------------------------------------
inline const RVector4& RActor::GetMultiplyColor()				{ return m_vMultiplyColor; }
//----------------------------------------------------------------------------------------------------
inline RAnimationController& RActor::GetAnimationController()	{ return m_animationController; }
//----------------------------------------------------------------------------------------------------

class RS_API RActorMLeaf : public RActor, public MMemPool<RActorMLeaf, 50000>
{
	MDeclareRTTI;
public:
	RActorMLeaf(RRENDERPASS eRenderPass = RRP_ACTOR) : RActor(eRenderPass) {}
	virtual ~RActorMLeaf() {}
};

}

#endif//_RActor_h
