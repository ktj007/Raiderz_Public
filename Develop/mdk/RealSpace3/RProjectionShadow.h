#pragma once

#include "RTypes.h"

namespace rs3 {

class RCameraSceneNode;

//////////////////////////////////////////////////////////////////////////
// Shadow Transform Interface
class RShadowTransform
{
protected:
	RVector3 m_cameraPosition;
	RVector m_lightDirection;

	RMatrix m_outProj;
	RMatrix m_outView;

public:
	RShadowTransform();

	const RVector& GetCameraPosition() { return m_cameraPosition; }

	const RMatrix& GetLightViewMatrix() { return m_outView; }
	const RMatrix& GetLightProjectionMatrix() { return m_outProj; }

	virtual void SetupForCull( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ, bool bReset) = 0;
	virtual void SetupForRender() = 0;
	virtual void AdjustMinMaxZ( float fMinZ, float fMaxZ ) {}
};

//////////////////////////////////////////////////////////////////////////
// Uniform Shadow Map
class RUniformShadow : public RShadowTransform
{
protected:
	typedef std::vector<RVector3> VEC3LIST;

public:
	// 반드시 pPoints의 크기는 8개 이상이어야 한다.
	static void GetFrustumPoints(RVector *pPoints, const RMatrix &matWVP);

	static void VecListToBBox(RBoundingBox &aabb, const VEC3LIST &vecList);
	static void BBoxToVecList(VEC3LIST &vecList, const RBoundingBox &aabb);

	static void BBoxOrthoProjection(RMatrix *pOut, const RBoundingBox &aabb);

	virtual void SetupForCull( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ, bool bReset) override;
	virtual void SetupForRender() override;

	void UpdateViewMatrix();
	virtual void UpdateProjection();
	void UpdateUniformProjection( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ );

	const RBoundingBox& GetLightFrustumAABB();

	virtual void AdjustMinMaxZ( float fMinZ, float fMaxZ ) override;

protected:
	RVector3 m_cameraDirection;
	RVector3 m_cameraUp;

	RBoundingBox m_outFrustumAABB;
	VEC3LIST m_vecList;	///< 캐스터들 점 집합

	void CalcUpVector(RVector3 &up, const RVector3 &viewDir, const RVector3 &lightDir);
	void TransformVECLIST(VEC3LIST &vecList, const RMatrix &mat);
};

//////////////////////////////////////////////////////////////////////////
// LiSPSM Shadow Transform
class RProjectionShadow : public RUniformShadow
{
	virtual void UpdateProjection();
};

inline const RBoundingBox& RUniformShadow::GetLightFrustumAABB()
{
	return m_outFrustumAABB;
}

//////////////////////////////////////////////////////////////////////////
// ShaderX6, Stable Shadow Map
class RStableShadowMap : public RShadowTransform
{
public:
	RStableShadowMap(float fTextureSize) : m_fShadowMapSize(fTextureSize) {}

	virtual void SetupForCull( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ, bool bReset) override;
	virtual void SetupForRender() override;

	void UpdateProjection( float fminz, float fmaxz, float fShadowMapSize );

	const RSphere& GetBoundingSphere() { return m_boundingSphere; }

	virtual void AdjustMinMaxZ( float fMinZ, float fMaxZ ) override;

protected:
	void UpdateViewTransform( float fShiftBack );
	void UpdateProjectionTransform( float fMinZ, float fMaxZ );

private:
	RSphere m_boundingSphere;
	float	m_fShadowMapSize;

	MPlane m_LightXYPlane;
};

};	// end of rs3