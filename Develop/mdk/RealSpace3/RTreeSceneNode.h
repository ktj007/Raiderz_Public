#pragma once

#include "RSceneNode.h"
#include "RTreeResource.h"
#include "RTreePassRenderer.h"

#include "RLoadingWorkUnit.h"
#include "MMemPool.h"

namespace rs3 {

RS_API extern const char *RSID_TREE;

class RS_API RTreeSceneNode : public RSceneNode, public RResource::LoadingController, public MMemPool<RTreeSceneNode>
{
	friend class RTreeResource;
	friend class RTreeResourceMgr;
	friend class RTreePassRenderer;
	friend class RCullingMgr;	//삭제해야함
	friend class RTreeRenderingList;
	friend class RRenderingList;

public:
	RTreeSceneNode();
	~RTreeSceneNode();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneNode declaration
	//
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_TREE);
//	DECLARE_USING_PASS(RRP_SPEEDTREE);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setter for LOD
	//
	void						SetUserLOD(bool l) { m_bUserLOD = l;}
	void						SetLOD(float x) { m_fLod = x; }		// 0.0 : 빌보드 ~ 1.0 : 최고 LOD


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// XML로 저장/읽기
	//
	virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void				SaveToXML(MXmlElement &element);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Build / Clone
	//
	bool						Build(const char* filename, bool bBackgroundCreation = false);
	virtual RSceneNode*			Clone();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOD계산
	virtual bool				CalculateLOD(RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Collision
	//
	RTreeCollisionObject*		GetTreeCollisionInfo(RTreeCollisionType type);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// tool support
	//
	bool						SelectFrustumTest(const RViewFrustum& Frustum);		// 마우스 드래그로부터 생성된 프러스텀으로부터 선택
	const std::string&			GetTreeFileName();									// 리소스의 이름을 알려고 할때
	void						RegisterForMakingLightMap( RCameraSceneNode* pCurrentCamera);			// 라이트맵을 만들기 위해 패스랜더러에 등록

	const float*				GetLocalAABB() const { return m_afBoundingBox; }

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//
	static float				m_fLodFarDist;
	static void					SetLodFarDist( float f)	{ m_fLodFarDist = f;	}
	static float				GetLodFarDist()			{ return m_fLodFarDist;	}

	virtual bool				IsLoadingCompleted();

private:
	RTreeResource*				m_pTreeResource;
	RTreeSceneNode*				m_pNextRenderChain;

	float						m_afBoundingBox[6];			// tree's bounding box

	bool						m_bUserLOD;					// 사용자가 직접 LOD 레벨을 지정하는가?
	bool						m_bNeedUpdateMatrix;		//? 안쓴느듯

	//////////////////////////////////////////////////////////////////////////


	float                       m_fWindMatrixOffset;		// used to assigned instances of the same tree different wind matrices

	float                       m_fLod;						// 1.0 = highest (fully 3D), 0.0 = lowest (billboard)

	CSpeedTreeRT::SLodValues	m_SpeedTreeLodInfo;			//스피드트리에서 렌더링시에 필요한 LOD관련 정보

	float						m_fShadowing;


	//////////////////////////////////////////////////////////////////////////


	float						m_PrevSin;
	float						m_PrevCos;
	float						m_PrevAtan2;

	RMatrix						m_PositionScaleMatrix;		// Rotation값을 제외하고 Position, Scale만 미리 계산한 Matrix.
															// WorldViewProjection Matrix를 셰이더 상수로 설정하기 위한 용도.
															// ViewProjection Matrix에 Position, Scale값만 셰이더 상수로
															// 넘겨 곱하면, float정밀도 문제로 떨림 현상이 생긴다.
	float						m_fCosTheta;				// Z축 기준 회전 값.
	float						m_fSinTheta;				// Z축 기준 회전 값.

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RSceneNode 로 부터 구현한 내용
	//

	virtual void				OnUpdateTransform() override;
	virtual void				OnUpdateAABB() override;

	virtual void				OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual void				OnRenderShadowMap( RCameraSceneNode* pCurrentCamera , RShadowStrategy* pShadowStrategy ) override { OnRender( pCurrentCamera, -1, RENDERING_NULL); }

	virtual bool				OnPick(RPICKINFO& pickInfo);
	virtual void				CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const override;

	virtual void				AddToRenderingList(RRenderingList* pRenderingList)			override;
	virtual void				RemoveFromRenderingList(RRenderingList* pRenderingList)		override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Tree Background Loader
	//
// 	class TREE_LOADER : public RLoadingWorkUnit
// 	{
// 	protected:
// 		virtual int				CalcPriority();
// 		virtual void			OnBackgroundWork();
// 		virtual void			OnUpdateCompleteWork();
// 	public:
// 		TREE_LOADER(RTreeSceneNode* pTreeSceneNode) : m_pTreeSceneNode(pTreeSceneNode){}
// 		virtual ~TREE_LOADER() { InvalidateWork(); }
// 	
// 		RTreeSceneNode* m_pTreeSceneNode;
// 
// 		virtual const char*		GetName()			{ return "tree"; }
// 		virtual int				GetDebuggingID() { return 1; }
// 	} *m_pLoadingWorkUnit;

	virtual int				CalcPriority();
	virtual void			OnLoadingComplete(RResource*);

private:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 자신과 friend class 정도에만 필요한 헬퍼 메쏘드
	//
	void						SetUsableAndUpdate_();
	void						RenderCollisionObject();

	bool						UpdateLOD(RCameraSceneNode *pCamera, float fNearLod, float fFarLod, bool bUseFogFar);

	void                        SetWindMatrixOffset(float fOffset)	{ m_fWindMatrixOffset = float(int(10.0f * fOffset)); }
	float						GetWindMatrixOffset() const			{ return m_fWindMatrixOffset;	}

	bool						IsViewLeaves() { return (m_SpeedTreeLodInfo.m_anLeafActiveLods[0] != -1) || (m_SpeedTreeLodInfo.m_anLeafActiveLods[1] != -1); }
	bool						IsViewBranchFrond() { return (m_fLod > 0.0f); }
	bool						IsBillboardRenderable(void){ return m_SpeedTreeLodInfo.m_fBillboardFadeOut>0.0f ? true : false;	}	//이 트리에 해당하는 빌보드를 그려야되냐?

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 쉐이더로 업로드될 벡터를 반환하는 메소드들
	//
	RVector4					GetPositionScale() const;						// xyz에 위치, w에 스케일을 넣어 반환
	RVector4					GetRotateYValue() const;						// sin, cos, -sin, 0 반환
	float						GetRotate();									// 시작위치로부터 몇도나 돌았는지 반환 (z축으로만 돈다고 가정)
	float						GetScale() const { return RSceneNode::GetScale().x; }		// 스케일값 반환 - 세 축으로 동일하게 스케일된다고 가정
	const RMatrix&				GetPositionScaleMatrix() const { return m_PositionScaleMatrix; } // 회전값을 제외한 Position, Scale을 반영한 Matrix를 반환
	float                       GetBillboardFade(void) const { return m_SpeedTreeLodInfo.m_fBillboardFadeOut; }
	float						GetCosTheta() const	{ return m_fCosTheta;	}
	float						GetSinTheta() const	{ return m_fSinTheta;	}

private://스피드트리용
	void	InitSpeedTreeLODInfo(void);

};

//----------------------------------------------------------------------------------------------------
inline float RTreeSceneNode::GetRotate()
{
	RVector direction = GetDirection();
	if (direction.x != m_PrevSin && direction.y != m_PrevCos) 
	{
		m_PrevAtan2 = atan2(direction.y, direction.x);
		if( m_PrevAtan2 < 0.0f )
			m_PrevAtan2 += MMath::TWOPI;

		m_PrevSin = direction.x;
		m_PrevCos = direction.y;
	}
	return m_PrevAtan2;
}
//----------------------------------------------------------------------------------------------------
inline RVector4 RTreeSceneNode::GetPositionScale() const
{
	RVector4 ret;
	RVector pos = GetPosition();
	ret.x = pos.x;
	ret.y = pos.y;
	ret.z = pos.z;
	ret.w = m_scale.x;
	return ret;
}
//----------------------------------------------------------------------------------------------------
inline RVector4 RTreeSceneNode::GetRotateYValue() const
{
	RVector direction = GetDirection();

	RVector4 ret;
	ret.x = direction.x;
	ret.y = direction.y;
	ret.z = - direction.x;
	ret.w = 0;
	return ret;
}
//----------------------------------------------------------------------------------------------------

} // end of namespace
