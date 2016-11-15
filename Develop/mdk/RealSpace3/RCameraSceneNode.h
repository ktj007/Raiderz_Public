#ifndef _RCAMERA_SCENE_NODE_H
#define _RCAMERA_SCENE_NODE_H

#include "RCameraSceneNodeController.h"
#include "RSceneManager.h"
#include "RViewFrustum.h"
#include "RDevice.h"
#include "RVType.h"

#include "RViewFrustumCullingMgr.h"
#include "ROcclusionCullingMgr.h"

#include <map>
#include <set>


namespace rs3 {

class RInferredRenderingStrategy;

RS_API extern const char *RSID_CAMERASCENENODE;
class RTechnique;
class RGIProcessor;
class RRenderingStrategy;

enum RRENDERINGMETHOD;

class RAlphaPassRenderNode {
public:
	virtual void Render() { }
};

class RS_API RCameraSceneNode : public RSceneNode
{
	MDeclareRTTI;
//	DECLARE_USING_PASS(RRP_CAMERA);
	DECLARE_NODE_ID_STRING(RSID_CAMERASCENENODE);

//	friend class RSceneManager;
	friend class RCameraPostController;
	friend class RCameraSceneNodeController;

protected:
	bool			m_camActived;

	// 평행 투영을 위한 세팅
	bool			m_bUserProjection;
	bool			m_bUserView;

	// 프로젝션을 위한 세팅
	float			m_fNearZ,m_fFarZ;
	float			m_fFovY,m_fFovX;
	float			m_fAspectRatio;			///< width / height

	float			m_fNearWidth;
	float			m_fNearHeight;

	RMatrix			m_viewMatrix;
	RMatrix			m_projMatrix;
	RMatrix			m_viewProjectionMatrix;
	RMatrix			m_raw_ProjectionMatrix;
	RViewFrustum	m_viewFrustum;
	RMatrix			m_matControllerApplied;
	bool			m_bNeedToApplyControlledMat;

	// 다수의 RenderTarget 버퍼 및 DepthStencil 버퍼
	RTexture*		m_RenderTarget[MAX_RENDERTARGET];
	RTexture*		m_DepthBuffer;

	// RT를 사용 하는 후처리가 여러개이면 관리가 필요하다 - 090617, OZ
	//std::list<RTexture*>		m_MannagedRenderTargets[MAX_RENDERTARGET];
	
	RPlane			m_ClipPlane;
	bool			m_bClipPlaneEnable;

	// User ClipPlane 설정
	RPlane			m_UserClipPlanes[6];
	int				m_nUserClipPlaneNum;

	bool			m_PassIgnored[RRP_MAX];	// 이 카메라는 이 플래그가 true가 된 패스는 렌더링하지 않습니다.

	// Rendertarget width, height
	int				m_nWidth;
	int				m_nHeight;

	// Clear 색상, 초기깊이값, 초기스텐실값
	DWORD			m_dwClearColor;
	float			m_fClearDepth;
	DWORD			m_dwClearStencil;

	RTechnique*		m_pTechnique;

	// Fog (현재 카메라에 포그 적용)
	float			m_fFogNearZ, m_fFogFarZ;
	DWORD			m_dwFogColor;
	bool			m_bFogEnable;
	bool			m_bUnderWater;
	float			m_fUnderWaterFogNear, m_fUnderWaterFogFar;
	DWORD			m_dwUnderWaterFogColor;

	// Shadow
	float			m_fShadowFar;

	// 나무 user LOD
	bool			m_bTreeUserLOD;
	float			m_fTreeUserLOD;

	// 나무 user Cull Distance
	bool			m_bTreeUserCullDistance;
	float			m_fTreeUserCullDistance;

	// Grass
	float			m_fGrassFar;
	float			m_fGrassNear;

	// LOD
	float			m_fDistanceCullingCriterion;
	float			m_fDistanceCullingCriterionTangent;
	// 오클루전컬링 사용유무
	bool			m_bEnableOcclusionCulling;

	// 현재 렌더링되고 있는 정보
	static RViewFrustum			m_currentViewFrustum;

	// 카메라 콘트롤러 작동을 OnUpdate에서 하고 업데이트를 한틱당 한번만 하고 싶어서
	// RActor에서 썼던 프레임 카운트당(한프레임당) 한번업데이트 방법을 사용합니다.
	virtual void				OnUpdate();
	virtual void				OnUpdateTransform() override;
	int							m_nLastUpdatedFrameCount;	// 업데이트된 프레임수 ( 한프레임에 여러번 업데이트 방지한다 )

	// camera controller container
	typedef std::map< std::string, RCameraSceneNodeController* > CAMERA_CONTROLLER_MAP;
	CAMERA_CONTROLLER_MAP		m_mapCameraController[RCCT_MAX];
	
	// StaticMesh 컬링을 PrimaryCamera로 한다.
	bool						m_bStaticMeshCullFromPrimaryCamera;

	//User ViewPort 설정
	bool						m_bUserViewPort;
	RViewport					m_UserViewPort;

private:
	void						UpdateProjectionMatrix();
	void						UpdateNearDimension();

	//////////////////////////////////////////////////////////////////////////
	// 상태 백업을 위한 변수들, 렌더링 이후로 복구하기 위해 저장해둔다
	//RCameraSceneNode*			m_pBeforeCamera;
	RTexture*					m_hBeforeRenderTarget[MAX_RENDERTARGET];
	RTexture*					m_hBeforeDepthBuffer;

	//컬링매니저
	RCullingMgr*				m_pViewFrusumCullingMgr;
	//오클루전 컬링매니저
	ROcclusionCullingMgrSet*	m_pOcclusionCullingMgrSet;

public:
	RCameraSceneNode(void);
	virtual ~RCameraSceneNode(void);

	void						Destroy();

	virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void				SaveToXML(MXmlElement &element);

	inline const RMatrix&		GetViewProjectionMatrix() const;
	inline const RMatrix&		GetRawProjectionMatrix() const;
	inline const RMatrix&		GetProjectionMatrix() const;
	inline const RMatrix&		GetViewMatrix() const;
	inline const RViewFrustum&	GetViewFrustum() const;
	inline		 RViewFrustum*	GetViewFrustumPtr();

	RMatrix						GetProjectionMatrixLH();
	RMatrix						GetViewMatrixLH();

	inline const float			GetNearWidth() { return	m_fNearWidth; }
	inline const float			GetNearHeight() { return	m_fNearHeight; }

	// Camera의 ViewMatrix 를 설정할 때에는 Set 함수를 사용합시다. SetTransform은 의도대로 동작하지 않습니다.
	void						SetUserView(const RMatrix &matrix);
	void						Set(const RVector& pos, const RVector& dir, const RVector& up=RVector(0,0,1));
	void						SetFov(float fFovX);
	void						SetFov(float fFovX,float fAspectRatio);
	void						SetNearFarZ(float fNearZ,float fFarZ);
	void						SetUserProjection(const RMatrix &matrix);

	virtual void				UpdateViewFrustrum();

	// 렌더타겟을 생성, idx는 어플리케이션에서 적절히 지정해야 함, 이 함수 안부르면 프레임버퍼에 렌더링합니다.
	bool						CreateRenderTarget(int width, int height, int idx, RFORMAT fmt = RFMT_ARGB32, RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD dwFlags = 0);

	// 깊이버퍼를 생성, 크기는 최소한 렌더타겟과 같아야 함
	bool						CreateDepthBuffer(int width, int height, RFORMAT fmt = RFMT_D16);

	// 렌더타겟/깊이버퍼 해제
	void						DestroyBuffers();

	// Near Clip Plane, projection matrix에 계산되어 동작하는 것들
	void						SetClipPlane(const RPlane& p) { m_ClipPlane = p; }
	void						SetClipPlaneEnable(bool b) { m_bClipPlaneEnable = b; }
	bool						GetClipPlaneEnable() { return m_bClipPlaneEnable; }

	// User Clip Plane ( device의 UserClipPlane의 Wrapper이면서, fixed와 shader 렌더링시에 동일한 동작을 보장하도록 )
	void						SetUserClipPlane(const RPlane *pPlanes, int nNum);
	int							GetUserClipPlaneNum() { return m_nUserClipPlaneNum; }
	const RPlane*				GetUserClipPlane()	{	return m_UserClipPlanes; }

	void						UpdateUserClipPlane(bool bUpdate, bool bProgrammablePipe = true);

	void						SetRenderTarget(int idx, RTexture* pTexture);
	RTexture*					GetRenderTarget(int idx);	// 렌더타겟으로 쓰여진 텍스쳐를 리턴
	RTexture**					GetRenderTargetLP(int idx);	// 렌더타겟으로 쓰여진 텍스쳐를 리턴. DOF의 경우 HDR에서 설정 된 RT stage 0을 항상 참조하고자 한다 - 090612, OZ
	void						SetDepthBuffer(RTexture* pTexture);
	RTexture*					GetDepthBuffer();

	void						SetActive(bool p);			// 카메라가 활성화됐는지 세팅, *모든* 활성화된 카메라는 각자 자기 자신의 렌더타겟에 자기의 시점으로 렌더링합니다.
	bool						GetActive();				// RDevice::SetPrimaryCamera로 지정하면 자동으로 활성화 됩니다. Primary에서 내려오면 자동으로 비활성.

	void						SetPassIgnore(RRENDERPASS start, RRENDERPASS end = (RRENDERPASS)(RRP_MAX-1));	// start부터 end까지의 패스를 무시하도록 설정합니다. start만 쓰면 맨 start부터 끝까지
	void						SetPassIgnore(RRENDERPASS pass, bool bIgnore);
	void						RemoveAllIgnorePass();				// 패스 무시 설정을 모두 지우고 모든 패스를 렌더링하도록 함
	bool						GetPassIgnore(RRENDERPASS pass);	// 해당 패스가 무시 상태인지 리턴

	void						SetClearColor(DWORD col);			// 렌더타겟 클리어 색상 지정/반환
	DWORD						GetClearColor();

	void						SetClearDepth(float d);
	float						GetClearDepth();

	void						SetClearStencil(DWORD s);
	DWORD						GetClearStencil();

	bool						isHaveStencil();					// 스텐실버퍼를 가지고 있는가!?
	bool						isActive();

	float						GetNearZ() const ;
	float						GetFarZ() const ;

	float						GetAspectRatio() { return m_fAspectRatio; }
	float						GetFovX() { return m_fFovX; }
	float						GetFovY() { return m_fFovY; }

	float						GetDistanceTo(const RVector& worldPosition);
	float						GetDistanceTo(const RSphere& worldSphere);

	//-------------------------------------------------------------------------------------
	// Fog 관련
	void						SetFog(float n, float f, DWORD col) 
								{ m_fFogNearZ = n; m_fFogFarZ = f; m_dwFogColor = col; }
	void						SetFogEnable(bool bEnable) { m_bFogEnable = bEnable; }
	void						SetFogNear(float fNear) { m_fFogNearZ = fNear; }
	void						SetFogFar(float fFar) { m_fFogFarZ = fFar; }
	void						SetFogColor(DWORD dwColor) { m_dwFogColor = dwColor; }

	void						SetUnderWaterFog(float n, float f, DWORD col) 
								{ m_fUnderWaterFogNear = n; m_fUnderWaterFogFar = f; m_dwUnderWaterFogColor = col; }
	void						SetUnderWaterFogEnable(bool bUnderWater)	{ m_bUnderWater =bUnderWater; }
	void						SetUnderWaterFogNear(float fNear)	{ m_fUnderWaterFogNear =fNear; }
	void						SetUnderWaterFogFar(float fFar)	{ m_fUnderWaterFogFar =fFar; }
	void						SetUnderWaterFogColor(DWORD dwColor) { m_dwUnderWaterFogColor = dwColor; }
	
	
	bool						GetFogEnable() { return m_bFogEnable; }
	float						GetFogNear() { return m_bUnderWater ? m_fUnderWaterFogNear : m_fFogNearZ; }
	float						GetFogFar() { return m_bUnderWater ? m_fUnderWaterFogFar : m_fFogFarZ; }
	DWORD						GetFogColor() { return m_bUnderWater ? m_dwUnderWaterFogColor:m_dwFogColor; }
	bool						GetUnderWaterFogEnable()	{ return m_bUnderWater; }
	//-------------------------------------------------------------------------------------
	

	void						SetShadowFar( float fFar)				{ m_fShadowFar = fFar;		}
	float						GetShadowFar()							{ return m_fShadowFar;		}

	void						SetGrassFar( float fFar)				{ m_fGrassFar = fFar;		}
	float						GetGrassFar()							{ return m_fGrassFar;		}
	void						SetGrassNear( float fNear)				{ m_fGrassNear = fNear;		}
	float						GetGrassNear()							{ return m_fGrassNear;		}

	void						SetTreeUserLOD(bool bEnable, float fLOD);
	bool						GetTreeUserLODEnabled()				{ return m_bTreeUserLOD; }
	float						GetTreeUserLOD()					{ return m_fTreeUserLOD; }

	void						SetDistanceCullingCriterion( float f);
	float						GetDistanceCullingCriterion();
	float						GetDistanceCullingCriterionTangent();
	void						CalculateDistanceCullingCriterionTangent();

	virtual bool				GetTargetPosition( RVector& outPos ) { return false; } // 카메라가 바라보는 대상이 있는 경우 그 위치를 얻어온다.

	virtual void				StartShader() {}
	virtual void				EndShader() {}

	void						SetTechnique(RTechnique* pTechnique) { m_pTechnique = pTechnique; }
	RTechnique*					GetTechnique() { return m_pTechnique; }

	virtual bool				OnTestLoadingComplete(){ return true; }

	void						SetStaticMeshCullFromPrimaryCamera( bool b ) { m_bStaticMeshCullFromPrimaryCamera = b; }
	bool						IsStaticMeshCullFromPrimaryCamera() { return m_bStaticMeshCullFromPrimaryCamera; }

	//View 공간상에서 pSceneNode가 차지하는 영역의 최대, 최소 Z값을 찾는다.
	void						FindMinMaxZ(const RSceneNode* pSceneNode, float& outMinZ, float& outMaxZ);

	RVector3					GetFarClipCornerInView();

	void						SetEnableOcclusionCulling(bool bEnable)		{	m_bEnableOcclusionCulling = bEnable;	}
	bool						GetEnableOcclusionCulling(void)				{	return m_bEnableOcclusionCulling;	}

	//////////////////////////////////////////////////////////////////////////
	// 렌더링 관련
	void						BeginRender(int nSurface = 0, bool bTatgetCtrl = true, RRenderingStrategy* pInferredRenderingStrategy = NULL);	// cubemap 일경우 0~5
	void						SetupRender(int nSurface = 0, bool bTargetCtrl = true, RRenderingStrategy* pInferredRenderingStrategy = NULL);
	void						EndRender( bool bTargetCtrl = true);

	void						RenderScreenShot(const char* szFilename, RRenderingStrategy* pRenderingStrategy, int nCustomWidth, int nCustomHeight);


	// 카메라 컨트롤러 추가 / 제거 / 얻기
	void						AddCameraController( RCameraSceneNodeController* _pCameraController );
	RCameraSceneNodeController*	GetCameraController( RCAMERACONTROLLERTYPE _eType, const char* _pSzContollerName);

	virtual void				OnEndRender();		// EndRender 이후에 불려진다
	virtual void				OnClearBuffer();

	//  랜더 타겟은 포스트 이펙트 자체적으로 디바이스에 설정 하도록... - 090619, OZ
	//void						AddRenderTarget(int idx, RTexture* pTexture);	// PostEff에 쓰일 랜더 타겟은 여러 단계를 거칠 수도 있다 - 090617, OZ
	//void						DeleteRenderTarget(int idx, RTexture* pTexture);	// PostEff에 쓰일 랜더 타겟은 여러 단계를 거칠 수도 있다 - 090617, OZ

	//ViewPort 관련
	bool						IsHaveUserViewPort()	{ return m_bUserViewPort; }
	void						SetViewPort(int x, int y, int nWidth, int nHeight, float fMinZ = 0, float fMaxZ = 1);
	const void					SetViewPort(const RViewport viewport);
	RViewport					GetViewPort()	{ return m_UserViewPort; }

	//컬링매니저
	RCullingMgr*				GetViewFrustumCullingMgr(void)		{	return m_pViewFrusumCullingMgr;	}
	ROcclusionCullingMgrSet*	GetOcclusionCullingMgrSet(void)		{	return m_pOcclusionCullingMgrSet;	}


protected:
	virtual void				OnPreRender(  RCameraSceneNode* pCurrentCamera) {}	// beginrender 시에 불려진다



};

//----------------------------------------------------------------------------------------------------
inline const RMatrix& RCameraSceneNode::GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
//----------------------------------------------------------------------------------------------------
inline const RMatrix& RCameraSceneNode::GetRawProjectionMatrix() const { return m_raw_ProjectionMatrix; }
//----------------------------------------------------------------------------------------------------
inline const RMatrix& RCameraSceneNode::GetProjectionMatrix() const { return m_projMatrix; }
//----------------------------------------------------------------------------------------------------
inline const RMatrix& RCameraSceneNode::GetViewMatrix() const { return m_viewMatrix; }
//----------------------------------------------------------------------------------------------------
inline const RViewFrustum& RCameraSceneNode::GetViewFrustum() const { return m_viewFrustum;		}
inline		 RViewFrustum* RCameraSceneNode::GetViewFrustumPtr()    { return &m_viewFrustum;	}
//----------------------------------------------------------------------------------------------------
inline void	RCameraSceneNode::SetRenderTarget(int idx, RTexture* pTexture) { m_RenderTarget[idx] = pTexture; }
//----------------------------------------------------------------------------------------------------
inline RTexture*	RCameraSceneNode::GetRenderTarget(int idx) { return m_RenderTarget[idx]; }
//----------------------------------------------------------------------------------------------------
inline RTexture**	RCameraSceneNode::GetRenderTargetLP(int idx) { return &m_RenderTarget[idx]; }
//----------------------------------------------------------------------------------------------------
inline void	RCameraSceneNode::SetDepthBuffer(RTexture* pTexture) { m_DepthBuffer = pTexture; }
//----------------------------------------------------------------------------------------------------
inline RTexture*	RCameraSceneNode::GetDepthBuffer() { return m_DepthBuffer; }
//----------------------------------------------------------------------------------------------------
inline void RCameraSceneNode::SetActive(bool p) { m_camActived = p; SetVisible(m_camActived); }
//----------------------------------------------------------------------------------------------------
inline bool RCameraSceneNode::GetActive() { return m_camActived; }
//----------------------------------------------------------------------------------------------------
inline bool RCameraSceneNode::isActive()	{ return m_camActived; }
//----------------------------------------------------------------------------------------------------
inline void RCameraSceneNode::SetPassIgnore(RRENDERPASS pass, bool bIgnore)	{ m_PassIgnored[pass] = bIgnore; }
//----------------------------------------------------------------------------------------------------
inline bool RCameraSceneNode::GetPassIgnore(RRENDERPASS pass) { return m_PassIgnored[pass]; }
//----------------------------------------------------------------------------------------------------
inline DWORD RCameraSceneNode::GetClearColor() { return m_dwClearColor; }
//----------------------------------------------------------------------------------------------------
inline void RCameraSceneNode::SetClearDepth(float d) { m_fClearDepth = d; }
//----------------------------------------------------------------------------------------------------
inline float RCameraSceneNode::GetClearDepth() { return m_fClearDepth; }
//----------------------------------------------------------------------------------------------------
inline void RCameraSceneNode::SetClearStencil(DWORD s) { m_dwClearStencil = s; }
//----------------------------------------------------------------------------------------------------
inline DWORD RCameraSceneNode::GetClearStencil() { return m_dwClearStencil; }
//----------------------------------------------------------------------------------------------------
inline float RCameraSceneNode::GetNearZ()	const { return m_fNearZ; }
//----------------------------------------------------------------------------------------------------
inline float RCameraSceneNode::GetFarZ()	const { return m_fFarZ;  }
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
inline void RCameraSceneNode::AddCameraController( RCameraSceneNodeController* _pCameraController )
{
	_ASSERT( _pCameraController );
	CAMERA_CONTROLLER_MAP::iterator itr = m_mapCameraController[_pCameraController->GetType()].find( _pCameraController->GetCameraControllerID() );
	if( itr != m_mapCameraController[_pCameraController->GetType()].end() )
	{
		RCameraSceneNodeController* pBeforeController = itr->second;

		// 기존에것을 새로운것에 관계시키는것이 아니라면, 예전껄 지운다
		if ( _pCameraController->IsBlendableBeforeController()  == false )
		{
			if (_pCameraController->IsManagedByCamera())
				delete pBeforeController;

			pBeforeController = NULL;
		}

		// 컨트롤러에 카메라 추가, 예전 카메라 컨트롤러가 있다면 정보를 넘겨준다.
		_pCameraController->InsertToCameraWhenControlAdded( this, pBeforeController );
		itr->second = _pCameraController;
	}
	else
	{
		_pCameraController->InsertToCameraWhenControlAdded( this, NULL );
		m_mapCameraController[_pCameraController->GetType()].insert( CAMERA_CONTROLLER_MAP::value_type( _pCameraController->GetCameraControllerID(), _pCameraController ) );
	}
}

//----------------------------------------------------------------------------------------------------
inline RCameraSceneNodeController* RCameraSceneNode::GetCameraController(  RCAMERACONTROLLERTYPE _eType, const char* _pSzContollerName)
{
	CAMERA_CONTROLLER_MAP::iterator itr = m_mapCameraController[_eType].find( _pSzContollerName );
	if ( itr != m_mapCameraController[_eType].end() )
		return itr->second;
	else
		return NULL;
}
//----------------------------------------------------------------------------------------------------
inline void	RCameraSceneNode::SetTreeUserLOD(bool bEnable, float fLOD)
{
	m_bTreeUserLOD = bEnable;
	m_fTreeUserLOD = fLOD;
}



}


#endif
