#ifndef _RSCENEGRAPH_H
#define _RSCENEGRAPH_H

#include "RTypes.h"
#include "RDuplicateUpdateCheckContext.h"
#include "MRTTI.h"
#include "RViewFrustum.h"
#include "MXml.h"
#include <list>
#include <set>
#include "ROccluderContext.h"
#include "RLODNode.h"

using namespace std;

namespace rs3 {

class RSceneNode;
class RWorldSceneNode;
class RSceneNodeController;
class RDevice;
class RCameraSceneNode;
class RSceneManager;
class RLightSceneNode;
class RMultiPassLightSceneNode;
class RSpotLightSceneNode;
class RStencilLightSceneNode;
class RRenderHelper;
class RViewFrustum;
class RSceneNodeController;
class RScreenEffectManager;
class RCullingMgr;
class RRenderingList;
class RInferredRenderingStrategy;
class RShadowStrategy;

enum RRENDERINGMETHOD;

typedef std::set<RSceneNode*> SCENE_SET;
typedef std::list<RSceneNodeController*> RSCENENODECONTROLLERLIST;
typedef std::list<RSceneNode*> RSceneNodeList;
typedef std::list<bool> BoolList;
typedef vector<RSceneNode*> RSceneNodeArray;
typedef vector<RWorldSceneNode*> RWorldSceneNodeArray;
typedef set<RWorldSceneNode*> RWorldSceneNodeMap;
typedef bool (* RSerializeTest)( RSceneNode* pSceneNode );

/// 콘트롤러가 업데이트 되는 타이밍에 따라 타입이 나뉜다
enum RSNCTRL_TYPE
{
	RSNCTRL_PREUPDATE,
	RSNCTRL_UPDATE,
	RSNCTRL_POSTUPDATE,
	RSNCTRL_POSTANIMATION,
	RSNCTRL_PRERENDER,
	RSNCTRL_CUSTOMUPDATE,
	RSNCTRL_MAX,
};

struct RUPDATENODE {
	bool		bValid;		// 이 플래그가 false 이면 삭제되어야 한다
	RSceneNode*	pSceneNode;	// 업데이트할 노드
};

typedef list<RUPDATENODE> RAutoUpdateNodeList;
typedef RAutoUpdateNodeList::iterator RAutoUpdateNodeListItr;


//////////////////////////////////////////////////////////////////////////
//
// 렌더패스 : 엔진은 카메라를 통해 RENDERPASS의 순서대로 렌더링을 합니다.
//
// 주의사항 :
//		일반적으로 Update와 Render가 구분되지만, 다른 카메라 좌표계를 사용하는
//		워터 렌더링 패스나 쉐도우 렌더링 패스같은 경우는 업데이트에 해당하는
//		컬링과 그에따른 렌더체인 구성을 같이 합니다. 이럴경우,
//		SceneManager에서 구축해놓은 렌더체인을 이후패스부터는 사용할 수 없습니다.
//		따라서, RRP_NOT_SAFE_RENDERCHAIN 패스 이후에는 씬매니저 업데이트로 구축된
//		(Primary Camera로부터 구축된) 렌더체인에 독립적인 패스를 사용하세요.
//
//////////////////////////////////////////////////////////////////////////
enum RRENDERPASS
{
	RRP_NONE = -1,
	RRP_CAMERA = 0,
	RRP_WORLDSCENE,
	RRP_DIRLIGHT,
	RRP_SUBLIGHT,
	RRP_LIGHT,
	RRP_OCCLUDER,
	RRP_SHADOWMAP,
	RRP_PREEFFECT,

	RRP_START_PASS_OF_BACKGROUND,
	RRP_SKYBOX,
	RRP_END_PASS_OF_BACKGROUND,

	RRP_START_PASS_OF_GSTAGE,		/// G-stage 랜더 시 패스 범위 지정을 위한 것.
	RRP_START_PASS_OF_MSTAGE,		/// M-stage 랜더 시 패스 범위 지정을 위한 것.
	RRP_START_PASS_OF_ZSTAGE,		/// Z-stage 랜더 시 패스 범위 지정을 위한 것.
	
	RRP_SPEEDTREE,
	RRP_NORMAL,

	RRP_ACTOR,

	RRP_TERRAIN,
	RRP_STATICMESH,				// static mesh에 알파마테리얼이 있음.
	RRP_END_PASS_OF_GSTAGE,		// G-stage 랜더 시 패스 범위 지정을 위한 것.
	RRP_END_PASS_OF_ZSTAGE,		// Z-stage 랜더 시 패스 범위 지정을 위한 것.
	RRP_VBLUR,					// 버텍스 모션 블러. 이펙트가 그려지기 전에 그려져야해서 패스를 추가한다. 
	RRP_EDGE,					// 외곽선
	RRP_NOT_SAFE_RENDERCHAIN,	///< 이후의 렌더 패스는 렌더체인을 재구성 해주어야 합니다.
	RRP_DECAL_POLYGON,			///< Polygon Decal
	RRP_DECAL_PROJECTION,		///< Projection Decal
	RRP_WATER,
	RRP_DISTORTION,
	RRP_ALPHAADDITIVE,
	RRP_PARTICLE,
	RRP_ALPHA,					///< Alpha는 AlphaAdditive보다 밑에 있어야 AlphaAdditive를 가리지 않습니다.
	RRP_SPOTLIGHT,
	RRP_WEATHEREFFECT,

	RRP_POSTEFFECT,

	RRP_DEBUG,					// 디버그용 패스
	RRP_2D,						// 샘플에서 쓰는 텍스트 등등

	RRP_END_PASS_OF_MSTAGE,		// M-stage 랜더 시 패스 범위 지정을 위한 것.


	// 마스킹으로 쓰이는 것들 따로 
	RRP_ABLUR,					// 누적 모션 블러.
	RRP_PBLUR,					// 픽셀 모션 블러.
	RRP_POSTEFFECT_EDGE,		// 포스트 이펙트용 외곽선
	RRP_FINAL_SCENE,			// 포스트 이펙트 마지막

	RRP_MAX
};

const static RRENDERPASS RRP_RENDER_BEGIN	= RRP_SUBLIGHT;
const static RRENDERPASS RRP_RENDER_END		= (RRENDERPASS)(RRP_DEBUG-1);
const static RRENDERPASS RRP_RENDER_END_ALL	= (RRENDERPASS)(RRP_MAX-1);

const static RRENDERPASS RRP_CULL_BEGIN = RRP_SUBLIGHT;
const static RRENDERPASS RRP_CULL_END	= (RRENDERPASS)(RRP_MAX-1);

//////////////////////////////////////////////////////////////////////////
//
// Frustum Test Method
//
//////////////////////////////////////////////////////////////////////////
enum RFRUSTUMTESTMETHOD
{
	RFTM_FAST,
	RFTM_NORMAL,
	RFTM_DETAIL
};

//////////////////////////////////////////////////////////////////////////
/// 신노드를 업데이트 하는 방법
enum RUPDATEMETHOD
{
	RUM_UPDATE_MANUALLY,		//< 신노드를 수동으로 업데이트 해주어야 한다
	RUM_UPDATE_WHEN_VISIBLE,	//< 화면에 들어왔을때만 업데이트가 불리운다
	RUM_UPDATE_ALWAYS			//< 항상 업데이트가 불린다
};

//////////////////////////////////////////////////////////////////////////
//
// SceneNode Picking
//
//////////////////////////////////////////////////////////////////////////
enum RPICKMETHOD
{
	RPM_FAST,
	RPM_NORMAL,
	RPM_DETAIL,
};

class RS_API RPICKINFO
{
	friend RSceneManager;
	RVector tempPickDirection;	// 임시, 사용하지 말것

public:
	RPICKINFO() : inMethod(RPM_NORMAL), outDistanceSq(FLT_MAX), outNode(NULL) { }
	RPICKINFO( const RVector& vPickOrigin, const RVector& vPickTo, RPICKMETHOD pick_method )
		: inPickOrigin(vPickOrigin)
		, inPickTo(vPickTo)
		, inMethod(pick_method)
		, outDistanceSq(FLT_MAX)
		, outNode(NULL)
	{}

	RVector			inPickOrigin;		// 입력 : pick의 시작점. -> 위의 두개를 채워주고 호출
	RVector			inPickTo;			// 입력 : pick의 끝점.
	RPICKMETHOD		inMethod;			// 입력 : pick 방법

	RVector			outPosition;		// 반환 : 위치
	RVector			outNormal;			// 반환 : normal, 아직 미 구현
	float			outDistanceSq;		// 반환 : PickOrigin에서의 거리의 제곱
	RSceneNode*		outNode;			// 반환 : scenenode 의 pointer
};


//////////////////////////////////////////////////////////////////////////
//
// SceneNodeVisitor
//		씬노드를 Traverse할때 PreOrder와 PostOrder에서 방문하여 행동하는 객체
//		일반적으로 PreOrder만으로 작업을 처리
//		PreOrder일 경우, 리턴값이 true일때 다음 Traverse  수행
//
//////////////////////////////////////////////////////////////////////////

// Visitor Interface
class RS_API RSceneNodeVisitor
{
public:
	RSceneNodeVisitor(){}
	virtual ~RSceneNodeVisitor(){}

	virtual bool OnPreOrderVisit( RSceneNode* pSceneNode )	= 0;
	virtual void OnPostOrderVisit( RSceneNode* pSceneNode )	= 0;
};

class RS_API RSceneNodeConstVisitor
{
public:
	RSceneNodeConstVisitor(){}
	virtual ~RSceneNodeConstVisitor(){}

	virtual bool OnPreOrderVisit( const RSceneNode* pSceneNode )	= 0;
	virtual void OnPostOrderVisit( const RSceneNode* pSceneNode )	= 0;
};

// Update Visitor
class RS_API RSceneNodeUpdateVisitor : public RSceneNodeVisitor
{
public:
	RSceneNodeUpdateVisitor(){}
	virtual ~RSceneNodeUpdateVisitor(){}

	virtual bool OnPreOrderVisit( RSceneNode* pSceneNode );
	virtual void OnPostOrderVisit( RSceneNode* pSceneNode );
};

class RS_API RSceneNodeUpdateTransformVisitor : public RSceneNodeVisitor
{
public:
	RSceneNodeUpdateTransformVisitor(){}
	virtual ~RSceneNodeUpdateTransformVisitor(){}

	virtual bool OnPreOrderVisit( RSceneNode* pSceneNode );
	virtual void OnPostOrderVisit( RSceneNode* pSceneNode ) {}
};


//////////////////////////////////////////////////////////////////////////
//
// RSceneNodePolygonCollector
//		신그래프에서 폴리곤을 수집하기위한 인터페이스
//
//////////////////////////////////////////////////////////////////////////

class RS_API RSceneNodePolygonCollector
{
public:
	RSceneNodePolygonCollector(){}
	virtual ~RSceneNodePolygonCollector(){}

	virtual void	HandleError(const std::string& strError) = 0;
	virtual DWORD	GetZoneMaterialID(const std::string &strMaterialName) = 0;
	virtual void	OnAddPolygon( const RVector& p1, const RVector& p2, const RVector& p3, WORD wNodeFlag, WORD wUserFlag, DWORD dwMaterial ) = 0;
};


//////////////////////////////////////////////////////////////////////////
//
// SceneNodeAttribute
//
//////////////////////////////////////////////////////////////////////////
enum RSCENENODE_ATTRIBUTE : DWORD
{
	//RSNA_HAS_SCENE_DESC			= 1 << 1,	///< 씬 디스크립션을 가지는지 여부. 설정되면 다른파일의 정보에 우선해서 현재 xml의 씬정보를 읽는다.
	RSNA_DO_NOT_SAVE			= 1 << 2,	///< Scene 저장시에 저장되지 않는다.
	RSNA_NO_LIGHTMAP			= 1 << 3,	///< 라이트맵 생성여부. 이 플래그가 설정되면 라이트맵을 만들지 않는다.
	RSNA_NO_HIERARCHY_UPDATE	= 1 << 4,	///< 계층 업데이트를 하지 않는다.
	RSNA_NO_DEBUG_RENDERING		= 1 << 5,	///< 디버그 랜더링을 하지 않는다.
	RSNA_HIERARCHY_TREE			= 1 << 6,	///< 공간관리 트리 구조에 씬노드를 삽입하여 관리
	RSNA_DO_NOT_SAVE_COLLISION	= 1 << 7,	///< Collision ( *.cl2 ) 저장시에 저장되지 않는다.
};


//////////////////////////////////////////////////////////////////////////
//
// 씬노드 매크로
//
//////////////////////////////////////////////////////////////////////////
//#define DECLARE_USING_PASS(x)			virtual RRENDERPASS GetUsingPass() const	{ return x; }
#define DECLARE_PASS_RENDERER_ID(x)		enum { PASS_RENDERER_ID = x };
#define DECLARE_NODE_ID_STRING(x)		virtual const char* GetNodeIDString() const { return x; } \
										static const char* GetNodeIDFromType() { return x; }

//////////////////////////////////////////////////////////////////////////
//
// 모든 씬노드의 base
//
//////////////////////////////////////////////////////////////////////////
class RS_API RSceneNode : public RLODNode, public RHaveAttributes< RSCENENODE_ATTRIBUTE >
{
	friend RSceneNodeUpdateVisitor;
	friend RSceneNodeUpdateTransformVisitor;
	friend RSceneNodeController;

	friend RSceneManager;
	friend RRenderHelper;

	friend RCameraSceneNode;
	friend RMultiPassLightSceneNode;
	friend RSpotLightSceneNode;
	friend RCullingMgr;
	friend RRenderingList;
	friend RInferredRenderingStrategy;

public:

	//////////////////////////////////////////////////////////////////////////
	// SceneNode Declaration
	//
	MDeclareRootRTTI;

//	DECLARE_USING_PASS(RRP_NORMAL);
	DECLARE_NODE_ID_STRING("unknown");

	RRENDERPASS GetUsingPass() const;


protected:

	//////////////////////////////////////////////////////////////////////////
	// 상속받은 자식들이 만들펑션들
	//
	virtual void				OnUpdateTransform(){}	// UpdateTransform시에
	virtual void				OnPreUpdate(){}			// Update시에
	virtual void				OnUpdate(){}			// OnPreUpdate -> OnUpdate -> UpdateTransform -> children update -> OnPostUpdate 순으로 불립니다
	virtual void				OnUpdateAABB(){};		// UpdateBoundingVolume에서 호출
	virtual bool				OnUpdateSphere();		// UpdateBoundingVolume에서 호출, 리턴 false : 바운딩구가 안변함 , 리턴 true : 바운딩구가 변함	
	virtual void				OnPostUpdate(){}

	virtual void				OnPreRender( RCameraSceneNode* pCurrentCamera){}			// OnRegisterToRender(결정) -> OnPreRender -> OnRender -> OnPostRender
	virtual void				OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){}
	virtual void				OnRenderGStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){}
	virtual void				OnRenderMStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){}
	virtual void				OnRenderZStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){}
	virtual void				OnPostRender( RCameraSceneNode* pCurrentCamera){}

	virtual void				OnAdd(){}				// 부모에 더해지거나 빠질때 불리우는 펑션
	virtual void				OnRemove(){}

	virtual void				OnAddScene();			// 신그래프에 더해지거나 빠질때 불리우는 펑션
	virtual void				OnRemoveScene();

	virtual void				OnMessage(char* szMsg, void* param)	{}			// 메시지, 아마 용도가 char 형으로 SceneNode를 Control 하려할때..
	virtual bool				OnPick(RPICKINFO& pickInfo) { return false; }	//피킹, 씬노드의 AABB로 피킹을 할경우.

	virtual void				OnRenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy ){}

	virtual void				DeleteManagingChildren();	///< 관리하는 자식노드들을 삭제한다

	virtual void				AddToRenderingList(RRenderingList* pRenderingList);
	virtual void				RemoveFromRenderingList(RRenderingList* pRenderingList);

	//////////////////////////////////////////////////////////////////////////
	// 씬노드 속성상속받은 씬노드들이 읽거나 변경하는 목적으로 사용하는 멤버
	//
	std::string					m_strNodeName;				// SceneNode 이름

	bool						m_bCullingNode;				// 컬링의 단위가 되는지여부

	int							m_nUpdatedFrame;			// 자신의 업데이트 된 프레임, Update가 불렸을때 중복을 제거하는 용도

	RDuplicateUpdateCheckContext	m_TransformUpdateContext;
	RDuplicateUpdateCheckContext	m_VisibilityUpdateContext;
	RDuplicateUpdateCheckContext	m_NoiseRefUpdateContext;

	RMatrixA16*					m_matLocal;					// local transform은 position, rotation(dir,up,right) 으로 이루어진다, scale값은 빠져있다.
	RMatrixA16*					m_matWorld;					// 렌더링시에 사용되는 world transform
	RMatrixA16*					m_matWorldPrev;				// 렌더링시에 사용되는 world transform 이전의 값 

	RQuaternion					m_qRotWorld;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vTranWorld;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vScaleWorld;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...

	
#ifdef USING_RSC_PHYSIQUE_QT	// 굳이 디파인으로 막지 않아도 되긴 하지만 메모리가 아깝다.. 코드 조금 지저분해지더라도 메모리를 조금이라도 아끼자
	RQuaternion					m_qRotWorldPrev;			// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vTranWorldPrev;			// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vScaleWorldPrev;			// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RQuaternion					m_qRotLocal;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vTranLocal;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
	RVector						m_vScaleLocal;				// 본의 용도시 사용. 메모리 아끼게 포인터로 해야 할까...
#endif

	RVector						m_scale;					// local transform 중 scale 값
	bool						m_bUseForPhysiqueBone;		// 피직에 사용되는 본의 용도인지. 더 그럴싸한 네이밍이 있으면 나중에 변경. 우선은 직관적이지만 촌스런 네이밍
	float						m_fSBasecale;					// uniform 스케일의 경우 
	
	float						m_fVisibility;				// Visibility 값
	float						m_fVisibilityResult;		// 누적된 Visibility 값

	float						m_fNoiseRef;				/// 노이즈 알파 테스트의 레퍼런스 값
	float						m_fNoiseRefResult;			/// 누적된 NoiseRef 값

	RSCENENODECONTROLLERLIST	m_sceneNodeControllers[RSNCTRL_MAX];	// 씬노드 컨트롤러 컨테이너
	DWORD						m_dwSceneCount;							// 렌더링 된 씬의 Count


	RWorldSceneNode*			m_pRootScene;				// 현재 자신이 존재하는 트리 계층 최 상단의 루트 씬 노드



// TODO : 이것들 private 으로 두고싶다..
protected:
	RSceneNode*					m_pParent;					// 노드사이의 관계
	RSceneNodeList				m_children;					// (연결된 씬노드의 부모/자식 관계)


	

private:
	RRENDERPASS					m_eRenderPass;

	bool						m_bRegisteredCandidate;

	bool						m_bAutoUpdate;				// 자동으로 불려지는 업데이트( Update() )가 필요한지
	bool						m_bAutoUpdateRegistered;	// 자동으로 불려지는 업데이트가 등록되었는지
	RAutoUpdateNodeListItr		m_itrAutoUpdateItr;			// 자동 업데이트 목록에서 지울때 한번에 삭제하기 위해 이터레이터 보관
	bool						RegisterAutoUpdate();
	void						UnregisterAutoUpdate();

	DWORD						m_dwRefreshedTime;			// 한 프레임 안에서 로직상에 의해 갱신을 여러번 할 수도 있다. 갱신 시간을 비교하여 같으면 안하도록 하자. - 090716, OZ

	// TODO: #16828 기존 코드와의 호환성을 위해 protected로 둔다. 버전 업 후 private로 변경해야 한다. - charmke
protected:
	RBoundingBox				m_aabb;						// SceneNode AABB, 직접 값을 수정하는 걸 막기위해 private에 두었다. SetAABB()로 값을 변경해야 한다.
private:
	RSphere						m_sphere;					// 씬노드 구 바운딩
	bool						UpdateSpatialTree();		///< 공간분할 트리를 재구축한다.
	bool						m_bNeedUpdateBoundingVolume;// 바운딩 볼륨을 업데이트 해야 하는가

protected:
	void						NeedUpdateBoundingVolume()	{ m_bNeedUpdateBoundingVolume = true; }
	void						SetAutoUpdate(bool bEnable);			// 씬 매니저에서 관리되는 업데이트

public:
	// TODO: #16828 기존 코드와의 호환성을 위해 public로 둔다. 버전 업 후 변경해야 한다. - charmke
	void						UpdateBoundingVolume();

public:
	
	RSceneNode(RRENDERPASS eRenderPass = RRP_NORMAL);
	virtual ~RSceneNode();

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 읽기 / 저장
	// From XML/To XML 일 때 element는 해당 Scene의 ROOT element가 인자로 넘어온다.

	virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation){ return false; }
	virtual void				SaveToXML(MXmlElement &element){}

	bool						LoadCommonProperty(MXmlElement &element);	// 기본속성 읽기

	virtual RSceneNode*			Clone()	{ return NULL; }					// 복사 생성
	void						Clear();									// 씬노드를 원래의 빈 상태로 만들어 주기

	void						AddToSceneNodeGroup(RSceneNode* pSceneNode);		// 씬노드 그룹에 등록, Deep Deletion 관리
	void						RemoveFromSceneNodeGroup(RSceneNode* pSceneNode);	// 씬노드 그룹에서 제거, 정상작동 할라나... COMMENT OZ
	RSceneNode*					GetGroupOwner();

	bool						IsNeedHierarchicalUpdate();///< 계층 업데이트를 해야 하는지 여부
	int							GetUpdatedFrame();		///< 최종 Update 된 frame 을 얻는다
	void						NeedUpdateTransform();	///< transform 을 다시 계산해야 하는지 여부
	bool						UpdateTransform();		///< transform 을 다시 계산한다, 리턴값은 tm 이 바뀌었는지 여부

	void						NeedUpdateVisibility();	///< Visibility를 다시 계산해야 하는지 여부
	virtual	void				UpdateVisibility();

	void						NeedUpdateNoiseRef();	///< NoiseRef를 다시 계산해야 하는지 여부
	virtual	void				UpdateNoiseRef();

	void						NeedUpdateAllParentDependentValues();///< 부모 의존적 파라메터 모두 다시 계산해야 하는지 여부
	void						UpdateAllParentDependentValues();	 ///< 부모 의존적 파라메터를 모두 업데이트 한다	

	bool						IsAutoUpdate(){ return m_bAutoUpdate; }	// AutoUpdate인지..

	inline bool					IsPrimaryCamRenerable()	{ return m_bPrimaryCamRenderable; }

	bool						IsNeedUpdateBoundingVolume(){ return m_bNeedUpdateBoundingVolume; }

	const RBoundingBox&			GetAABB()	const		{ return m_aabb; }
	// TODO: #16828 기존 코드와의 호환성을 위해 메서드를 다시 살렸다. 버전 업 후 제거해야 한다. - charmke
	RBoundingBox&				GetAABB()				{ return m_aabb; }
	void						SetAABB(const RAABB& aabb)	{ m_aabb = aabb; NeedUpdateBoundingVolume(); }

	//구 체인 (공간분할에 쓰인다)
	bool						UseSpatialTree(void);
	const RSphere&				GetSphere()	const				{ return m_sphere; }
	void						SetSphere(RSphere& sphere)		{ m_sphere = sphere; NeedUpdateBoundingVolume(); }
	void						DebugRenderSphere(DWORD dwColor,float plusRadius = 0.0f);

	virtual bool				IsLoadingCompleted()	{ return true; }	// 로딩 요청한 것이 모두 끝났는지 여부

	void						SetRootSceneNode( RWorldSceneNode* p);	//{ m_pRootScene = p;			} + 하위 트리 탐색
	RWorldSceneNode*			GetRootSceneNode()						{ return m_pRootScene;		}

	//////////////////////////////////////////////////////////////////////////
	// 충돌및 picking 에 관한 것들
	//
	bool						Pick(RPICKINFO& pickInfo);
	// pFarPlane - [0] : RV_FAR, pPlanes - 그 외의 Plane들 
//	virtual bool				FrustumTest(const RPlane *pPlanes, int nNum)	{	return true; }
	virtual void				CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const {} // static 충돌메시를 리턴해준다


	//////////////////////////////////////////////////////////////////////////
	// 노드 관리
	//
	bool						AddChild(RSceneNode *pChild);
	bool						RemoveChild(RSceneNode *pChild);
	bool						RemoveFromParent();

	void						SetParent(RSceneNode* pParent);
	RSceneNode*					GetParent()			{ return m_pParent; }
	const RSceneNodeList&		GetChildren() const;

	void						ExclusionTraverseRefMesh( RSceneNodeConstVisitor* pVisitor ) const;

	void						Traverse( RSceneNodeVisitor* pVisitor );
	void						Traverse( RSceneNodeConstVisitor* pVisitor ) const;
	void						SerializeToVector( RSerializeTest pSerailTestFunc, std::vector< RSceneNode* >& refSerialedVector ) const ; //자식 씬노드를 백터로 직렬화

	//////////////////////////////////////////////////////////////////////////
	// controllers
	//
	void						AddController(RSceneNodeController* pController);
	bool						RemoveController(RSceneNodeController* pController);
	void						UpdateSceneNodeControllers( RSNCTRL_TYPE eType, RCameraSceneNode* pCamera = NULL );
	void						ReNewContollers();
	void						EnableControllers(bool bEnable);
	RSceneNodeController*		GetFirstControllerByName( RSNCTRL_TYPE eType, const char* szControllerName);
	RSCENENODECONTROLLERLIST*	GetControllers( RSNCTRL_TYPE eType );


	//////////////////////////////////////////////////////////////////////////
	// 씬노드 속성 : Transform / position / ...
	//
	const RMatrixA16&			GetLocalTransform() const;			// local transform 을 얻어냄
	const RMatrixA16&			GetWorldTransform() const;			// 부모가 없으면 world = local 이다.
	const RMatrixA16&			GetWorldTransformPrev() const;	

	const RQuaternion&			GetLocalRotation() const;			
	const RVector&				GetLocalTranslation() const;		
	const RVector&				GetLocalScale() const;		

	const RQuaternion&			GetWorldRotation() const;			
	const RVector&				GetWorldTranslation() const;		
	const RVector&				GetWorldScale() const;		

	const RQuaternion&			GetWorldRotationPrev() const;		
	const RVector&				GetWorldTranslationPrev() const;	
	const RVector&				GetWorldScalePrev() const;	

	void						SetTransform(const RMatrix& tm);	// local transform 을 세팅
	void						SetTransform(const RVector& pos, const RVector& dir, const RVector& up=RVector(0,0,1));

	void						SetPosition(float x,float y,float z);
	void						SetPosition(const RVector& pos);
	void						SetDirection(const RVector& dir, const RVector& up=RVector(0,0,1));
	void						SetRotation(const RQuaternion& quat);

	RVector						GetPosition() const;
	RVector						GetDirection() const;
	RVector						GetRight() const;
	RVector						GetUp() const;

	RVector						GetWorldPosition() const;	// 월드좌표를 얻는다. 렌더링시에는 현재프레임의 좌표, 업데이트시에는 이전프레임의 좌표이다.
	RVector						GetWorldDirection() const;
	RVector						GetWorldRight() const;
	RVector						GetWorldUp() const;
	RVector						GetWorldPositionPrev() const;

	void						SetScale(const RVector& scale);
	const RVector&				GetScale() const;
	float						GetScaleUniform() const;
	RVector						GetScaleFromWorldTransform();

	void						SetNodeName(const std::string &strName)	{	m_strNodeName = strName; }
	const string&				GetNodeName()	const					{	return m_strNodeName;	 }

	void						Message(char* szMsg, void* param);
	DWORD						GetLastSceneCount()	{ return m_dwSceneCount; }
	void						UpdateLastSceneCount(DWORD count)	{	m_dwSceneCount = count; }

	void						SetVisibility(float fVisibility);	// 반투명하게 보이는 정도
	float						GetVisibility();
	float						GetVisibilityResult() const;	///< 부모로부터 누적된 반투명값

	void						SetNoiseRef(float fVisibility);	// 노이즈 페이드 아웃 값
	float						GetNoiseRef();
	float						GetNoiseRefResult();	///< 부모로부터 누적된 노이즈 페이드 아웃 값

	inline void					SetUseForPhysiqueBone( bool b = true)	{ m_bUseForPhysiqueBone = b;	}	
	inline bool					IsUseForPhysiqueBone()					{ return m_bUseForPhysiqueBone;	}	

	bool						IsCullingNode()							{ return m_bCullingNode;		}
	bool						IsRegisteredCandidate()					{ return m_bRegisteredCandidate;}
	void						SetRegisteredCandidate( bool b)			{ m_bRegisteredCandidate = b;	}

	void						SetAttached( bool b)					{ m_bAttached = b;				}
	bool						IsAttached()							{ return m_bAttached;			}

	//////////////////////////////////////////////////////////////////////////
	// Usable : 업데이트, 렌더 여부 / Visible : 렌더 여부
	//
	inline void					SetVisible(bool bFlag)	{ m_bVisible = bFlag; }	//보이고 안보이고 여부
	virtual bool				GetVisible() const		{ return m_bVisible && !GetOcclued() && IsLODCullingPassed(); }

	inline void					SetOcclued(bool bFlag)	{ m_bOccluded = bFlag; }	
	virtual bool				GetOcclued() const		{ return m_bOccluded; }


	inline  void				SetUsable(bool bFlag)	{ m_bUsableSceneNode = bFlag; }
	virtual  bool				GetUsable()	const		{ return m_bUsableSceneNode; }

	//////////////////////////////////////////////////////////////////////////
	// Render
	//
	void PreRender(RCameraSceneNode *pCamera);
	void PostRender(RCameraSceneNode *pCamera);

	virtual void RenderAlpha( RCameraSceneNode *pCamera,int iSubMaterial, RRENDERINGMETHOD eMothod){}
	virtual void Render( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){ OnRender( pCurrentCamera, nSubMaterialID,renderMothod );}
	virtual void RenderGStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){ OnRenderGStage( pCurrentCamera, nSubMaterialID,renderMothod );}
	virtual void RenderMStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){ OnRenderMStage( pCurrentCamera, nSubMaterialID,renderMothod );}
	virtual void RenderZStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod){ OnRenderZStage( pCurrentCamera, nSubMaterialID,renderMothod );}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 유저데이터
	//
	void						SetUserData(void *ptr)	{	m_pUserData = ptr; }
	void*						GetUserData()	const	{	return m_pUserData; }

	void						AddAABB( RBoundingBox* _pAABB );

	//////////////////////////////////////////////////////////////////////////
	// 유저프로퍼티
	//
	void						PushUserProperty(const string& strPropertyName, const string& strValue);
	void						RemoveUserProperty(unsigned int index);
	const string&				GetUserPropertyName(unsigned int index) const;
	const string&				GetUserPropertyValue(unsigned int index) const;
	void						SetUserPropertyValue(unsigned int index, const string& strValue);
	unsigned int				GetUserPropertyCount(void) const	{ return m_vecUserProperty.size(); }

private:

	//////////////////////////////////////////////////////////////////////////
	// Update / Render Flag
	//
	bool						m_bUsableSceneNode;			///< 업데이트 + 렌더링 여부
	bool						m_bVisible;					///< 렌더링 여부
	bool						m_bOccluded;				///< 
	bool						m_bAttached;				///< 신그래프에 달려있는지 여부

	bool						m_bPrimaryCamRenderable;	///< Primary Camera 렌더링 여부


	//////////////////////////////////////////////////////////////////////////
	// 씬노드 속성
	//
	void*						m_pUserData;				///< 사용자 정의 데이터

	//////////////////////////////////////////////////////////////////////////
	// 씬노드 그룹, Deep Delete 관리
	//
	unsigned int				m_nManagingSceneNodeCount;
	RSceneNode*					m_pSceneNodeGroupOwner;


	//////////////////////////////////////////////////////////////////////////
	// clear - 빈 씬노드로 만들어 주기
	//

	// 씬노드 그룹을 통해 지워줄 자식과 지워주지 않을 자식으로 구분
	class RSceneNodeChildrenCleanUpVisitor : public RSceneNodeVisitor
	{
	public:
		RSceneNodeChildrenCleanUpVisitor(){}
		virtual ~RSceneNodeChildrenCleanUpVisitor(){}

		virtual bool			OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void			OnPostOrderVisit(RSceneNode* pSceneNode ){}

		RSceneNode*				m_pRequestSceneNode;
		std::vector< RSceneNode* > m_vecNodeToChangeParent;
	};
	friend class RSceneNodeChildrenCleanUpVisitor;

	//////////////////////////////////////////////////////////////////////////
	// 유저프로퍼티
	//
	vector<pair<string, string>> m_vecUserProperty;
};
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::NeedUpdateTransform()	{ m_TransformUpdateContext.NeedUpdate(); }
inline void RSceneNode::NeedUpdateVisibility()	{ m_VisibilityUpdateContext.NeedUpdate(); }
inline void RSceneNode::NeedUpdateNoiseRef()	{ m_NoiseRefUpdateContext.NeedUpdate(); }
//----------------------------------------------------------------------------------------------------
inline int RSceneNode::GetUpdatedFrame()		{ return m_nUpdatedFrame; }
//----------------------------------------------------------------------------------------------------
inline const RMatrixA16& RSceneNode::GetLocalTransform() const					{ return (*m_matLocal);	}
inline const RMatrixA16& RSceneNode::GetWorldTransform() const					{ return (*m_matWorld);	}
inline const RMatrixA16& RSceneNode::GetWorldTransformPrev() const				{ return (*m_matWorldPrev);}	

inline const RQuaternion& RSceneNode::GetWorldRotation() const				{ return m_qRotWorld;	}	
inline const RVector& RSceneNode::GetWorldTranslation() const				{ return m_vTranWorld;	}	
inline const RVector& RSceneNode::GetWorldScale() const						{ return m_vScaleWorld;	}
//----------------------------------------------------------------------------------------------------

#ifdef USING_RSC_PHYSIQUE_QT
inline const RQuaternion& RSceneNode::GetLocalRotation() const				{ return m_qRotLocal;	}	
inline const RVector& RSceneNode::GetLocalTranslation() const				{ return m_vTranLocal;	}	
inline const RVector& RSceneNode::GetLocalScale() const						{ return m_vScaleLocal;	}
inline const RQuaternion& RSceneNode::GetWorldRotationPrev() const			{ return m_qRotWorldPrev;	}	
inline const RVector& RSceneNode::GetWorldTranslationPrev() const			{ return m_vTranWorldPrev;	}	
inline const RVector& RSceneNode::GetWorldScalePrev() const					{ return m_vScaleWorldPrev;	}	
#endif
//----------------------------------------------------------------------------------------------------
inline const RSceneNodeList& RSceneNode::GetChildren() const	{ return m_children; }
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::SetVisibility(float fVisibility)
{
	_ASSERT(fVisibility>=0.0f);
	m_fVisibility = fVisibility;
	NeedUpdateVisibility();
}
//----------------------------------------------------------------------------------------------------
inline float RSceneNode::GetVisibility()						{ return m_fVisibility; }
//----------------------------------------------------------------------------------------------------
inline float RSceneNode::GetVisibilityResult() const			{ return m_fVisibilityResult; }
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::SetNoiseRef(float fRef)
{
	_ASSERT(fRef>=0.0f);
	m_fNoiseRef = fRef;
	NeedUpdateNoiseRef();
}
//----------------------------------------------------------------------------------------------------
inline float RSceneNode::GetNoiseRef()						{ return m_fNoiseRef; }
//----------------------------------------------------------------------------------------------------
inline float RSceneNode::GetNoiseRefResult()					{ return m_fNoiseRefResult; }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetWorldPosition() const				{ return RVector(m_matWorld->_41,m_matWorld->_42,m_matWorld->_43); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetWorldRight() const				{ return RVector(m_matWorld->_11, m_matWorld->_12, m_matWorld->_13); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetWorldDirection() const			{ return RVector(m_matWorld->_21,m_matWorld->_22,m_matWorld->_23); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetWorldUp() const					{ return RVector(m_matWorld->_31,m_matWorld->_32,m_matWorld->_33); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetWorldPositionPrev() const			{ return RVector(m_matWorldPrev->_41,m_matWorldPrev->_42,m_matWorldPrev->_43); }
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::SetPosition(float x,float y,float z)	{ SetPosition(RVector(x,y,z)); }
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::SetPosition(const RVector& pos)
{

#ifdef USING_RSC_PHYSIQUE_QT
	if( IsUseForPhysiqueBone())
		m_vTranLocal = pos;
#endif

	m_matLocal->_41 = pos.x; m_matLocal->_42 = pos.y; m_matLocal->_43 = pos.z;

	NeedUpdateTransform();
}
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::SetScale(const RVector& scale)	{
	m_scale = scale; 	NeedUpdateTransform(); 
#ifdef USING_RSC_PHYSIQUE_QT
	m_vScaleLocal = scale;
#endif
}
//----------------------------------------------------------------------------------------------------
inline float RSceneNode::GetScaleUniform() const	{ return m_scale.z;}		
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetPosition() const		{ return RVector(m_matLocal->_41,m_matLocal->_42,m_matLocal->_43); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetDirection() const		{ return RVector(m_matLocal->_21,m_matLocal->_22,m_matLocal->_23); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetRight() const			{ return RVector(m_matLocal->_11,m_matLocal->_12,m_matLocal->_13); }
//----------------------------------------------------------------------------------------------------
inline RVector RSceneNode::GetUp() const			{ return RVector(m_matLocal->_31,m_matLocal->_32,m_matLocal->_33); }
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::AddToSceneNodeGroup(RSceneNode* pSceneNode) // 씬노드 그룹에 등록
{
	_ASSERT( pSceneNode->m_pSceneNodeGroupOwner == NULL );
	++m_nManagingSceneNodeCount;
	pSceneNode->m_pSceneNodeGroupOwner = this;
}
//----------------------------------------------------------------------------------------------------
inline void RSceneNode::RemoveFromSceneNodeGroup(RSceneNode* pSceneNode) // 씬노드 그룹에 등록
{
	if( pSceneNode->m_pSceneNodeGroupOwner == this)
	{
		--m_nManagingSceneNodeCount;
		pSceneNode->m_pSceneNodeGroupOwner = NULL;
	}
}
//----------------------------------------------------------------------------------------------------
inline RSceneNode* RSceneNode::GetGroupOwner()
{
	return m_pSceneNodeGroupOwner;
}
//----------------------------------------------------------------------------------------------------
inline RRENDERPASS RSceneNode::GetUsingPass() const	{ return m_eRenderPass; }
//----------------------------------------------------------------------------------------------------
inline RSCENENODECONTROLLERLIST* RSceneNode::GetControllers( RSNCTRL_TYPE eType ) { return &m_sceneNodeControllers[eType]; }

}

#endif
