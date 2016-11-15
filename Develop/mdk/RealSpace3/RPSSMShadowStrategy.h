#pragma once

#include "RDepthMapShadowStrategy.h"

namespace rs3 {

class RS_API RPSSMShadowStrategy : public RDepthMapShadowStrategy
{
public:
	RPSSMShadowStrategy(const RShadowDesc& desc);

	virtual ~RPSSMShadowStrategy();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void Init();
	virtual void Destroy();

	void CreateBuffers(int nSize);
	void DestroyBuffers();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering

	virtual void Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld) override;


	//////////////////////////////////////////////////////////////////////////
	// PSSM을 위해 나누는 것관련 DepthMap

	virtual const RMatrix GetCurrentLightTransform();
	virtual const RMatrix GetCurrentLightViewTransform();
	virtual const RCameraSceneNode* GetCurrentDepthCamera();
	virtual RBoundingBox GetCurrentLightFrustumAABB();


	//////////////////////////////////////////////////////////////////////////
	// 기타 정보 얻기
	void SetCurrentCameraIndex(int nIndex);

	void SetCurrentViewNearZClip(float nz, float fz);

	float GetCurrentViewNearZ(RCameraSceneNode *pViewCamera);
	float GetCurrentViewFarZ(RCameraSceneNode *pViewCamera);

	int GetSplitCount()	{ return m_nTextureNum; }

	const RVector4* GetSplitInfo() { return m_vSplitInfo; }

private:
	int		m_nTextureNum;
	float	m_fSplitDistance[MAX_PSSM_SPLIT_COUNT+1];

	// Depth Camera ( Init 에서 강제 초기화 )
	struct SCameraSet
	{
		// nZ, fZ 은 0 ~ 1 사이의 값
		float nZ;
		float fZ;

		int nPrevRT, nCurrentRT;

		RCameraSceneNode* m_camShadow;
		RBoundingBox m_camFrustumAABB;
	};

	std::vector<SCameraSet> m_camShadows;
	int m_nCurrentIndex;

	RVector4 m_vSplitInfo[MAX_PSSM_SPLIT_COUNT];
};

}