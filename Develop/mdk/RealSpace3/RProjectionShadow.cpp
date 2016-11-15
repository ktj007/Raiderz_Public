#include "stdafx.h"
#include "RProjectionShadow.h"
#include "RCameraSceneNode.h"

//#define _RH_PROJECTION

namespace rs3 {

RShadowTransform::RShadowTransform()
{
	m_outView.MakeIdentity();
	m_outProj.MakeIdentity();
}

void RUniformShadow::GetFrustumPoints(RVector *pPoints, const RMatrix &matWVP)
{
	static const RVector4 FRUSTUM_POINT_IN_CLIP[8] = 
	{
		RVector4(-1,-1, 1, 1),	/// 0 Far Left - bottom
		RVector4( 1,-1, 1, 1),	/// 1 Far Right- bottom
		RVector4(-1, 1, 1, 1),	/// 2 Far Left - top
		RVector4( 1, 1, 1, 1),	/// 3 Far Right- top

		RVector4(-1,-1, 0, 1),	/// 4 Near Left - bottom
		RVector4( 1,-1, 0, 1),	/// 5 Near Right- bottom
		RVector4(-1, 1, 0, 1),	/// 6 Near Left - top
		RVector4( 1, 1, 0, 1),	/// 7 Near Right- top
	};

	RMatrix matInv;
	matWVP.GetInverse(&matInv);

	// make view frustum points
	for (int i = 0; i<8; ++i)
	{
		RVector4 P = FRUSTUM_POINT_IN_CLIP[i] * matInv;
		P /= P.w;
		pPoints[i] = RVector(P.x, P.y, P.z);
	}
}

void RUniformShadow::VecListToBBox(RBoundingBox &aabb, const VEC3LIST &vecList)
{
	if (!vecList.empty())
	{
		RVector &vmin = aabb.vmin;
		RVector &vmax = aabb.vmax;

		vmin = vecList[0];
		vmax = vecList[0];

		for ( size_t si = 0; si<vecList.size(); ++si)
		{
			const RVector3 &P = vecList[si];
			vmin.x = min(vmin.x, P.x);
			vmin.y = min(vmin.y, P.y);
			vmin.z = min(vmin.z, P.z);

			vmax.x = max(vmax.x, P.x);
			vmax.y = max(vmax.y, P.y);
			vmax.z = max(vmax.z, P.z);
		}
	}
}

void RUniformShadow::BBoxOrthoProjection(RMatrix *pOut, const RBoundingBox &aabb)
{
	const RVector &vmin = aabb.vmin;
	const RVector &vmax = aabb.vmax;

#ifdef _RH_PROJECTION
	pOut->SetOrthoOffCenterRH( vmin.x, vmax.x, vmin.y, vmax.y, vmin.z, vmax.z );
#else
	pOut->SetOrthoOffCenterLH( vmin.x, vmax.x, vmin.y, vmax.y, vmin.z, vmax.z );
#endif
}

//////////////////////////////////////////////////////////////////////////
//
void RUniformShadow::CalcUpVector(RVector3 &up, const RVector3 &viewDir, const RVector3 &lightDir)
{
#if 1
	RVector3 left = viewDir.CrossProduct(lightDir);
	up = lightDir.CrossProduct(left);
	up.Normalize();
#else
	RVector3 left = lightDir.CrossProduct(m_cameraDirection);
	up = left.CrossProduct(lightDir);
	up.Normalize();
#endif
}

void RUniformShadow::TransformVECLIST(RProjectionShadow::VEC3LIST &vecList, const RMatrix &mat)
{
	for (size_t si = 0; si<vecList.size(); ++si)
	{
		RVector4 v;
		mat.TransformVect(vecList[si], v);
		v /= v.w;

		vecList[si].x = v.x;
		vecList[si].y = v.y;
		vecList[si].z = v.z;
	}
}

void RUniformShadow::UpdateViewMatrix()
{
	CalcUpVector(m_cameraUp, m_cameraDirection, m_lightDirection);

#ifdef _RH_PROJECTION
	m_outView.SetLookAtMatrixRH(m_cameraPosition, m_cameraPosition + m_lightDirection, m_cameraUp);
#else
	m_outView.SetLookAtMatrixLH(m_cameraPosition, m_cameraPosition + m_lightDirection, m_cameraUp);
#endif
}

void RProjectionShadow::UpdateProjection()
{
	m_outFrustumAABB.Initialize();

	float dotProd = m_cameraDirection.DotProduct(m_lightDirection);
#if 1
	if ( fabs(dotProd) > 0.99f )
	{
		RUniformShadow::UpdateProjection();
		return;
	}
#endif

	VEC3LIST recList = m_vecList;
	TransformVECLIST(recList, m_outView);

	RBoundingBox aabb;
	VecListToBBox(aabb, recList);

	float d = aabb.vmax.y - aabb.vmin.y;
	float n = ( aabb.vmin.y > 0.0f ? ( aabb.vmin.y + d * 0.2f ) : ( d * 0.65f ));
	float f = n + d;
	float p = aabb.vmin.y - n;

	RMatrix matLisp;
	matLisp.MakeIdentity();
	matLisp._22 = f / (f-n);
	matLisp._42 =-f*n / (f-n);
	matLisp._24 = 1.f;
	matLisp._44 = 0.0f;

	RMatrix matLVP;
	matLVP.MakeIdentity();
	matLVP.SetTranslation(RVector3(0, -p, 0));

	// view 를 건드리지 말고 proj 쪽으로 몰아 넣자.
//	m_outView *= matLVP;
//	matLVP *= matLisp;

	TransformVECLIST(recList, matLVP * matLisp);
	VecListToBBox(m_outFrustumAABB, recList);
	BBoxOrthoProjection(&m_outProj, m_outFrustumAABB);

	m_outProj = matLVP * matLisp * m_outProj;
}

void RUniformShadow::UpdateProjection()
{
	VEC3LIST recList = m_vecList;

	TransformVECLIST(recList, m_outView);

	m_outFrustumAABB.Initialize();
	VecListToBBox(m_outFrustumAABB, recList);
	BBoxOrthoProjection(&m_outProj, m_outFrustumAABB);

	return;
}

void RUniformShadow::AdjustMinMaxZ( float fMinZ, float fMaxZ )
{
	m_vecList.push_back(m_cameraPosition + m_lightDirection*fMinZ);
	m_vecList.push_back(m_cameraPosition + m_lightDirection*fMaxZ);
	
	// 광원방향 최소인 지점까지 카메라를 뒤로 밀어준다
	RVector vecCameraShiftBack = m_lightDirection*fMinZ;

	RVector vecCameraNewPosition = m_cameraPosition + vecCameraShiftBack;

	m_cameraPosition = vecCameraNewPosition;
}

void RUniformShadow::SetupForCull( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ, bool bReset)
{
	RPFC_THISFUNC;
	m_cameraPosition = pViewCamera->GetWorldPosition();
	m_cameraDirection = pViewCamera->GetWorldDirection();
	m_lightDirection = lightDirection;

	UpdateViewMatrix();
	UpdateUniformProjection( pViewCamera, m_lightDirection, fNearZ, fFarZ );
}


/// 원래 view-projection 을 nearz , farz 기준으로 잘라낸 view-projection matrix 를 얻는다
RMatrix GetClipedViewProjectionMatrix(RCameraSceneNode *pViewCamera, float fNearZ, float fFarZ )
{
	RMatrix matProj;
	matProj.MakeIdentity();
	matProj.SetProjectionMatrixFovRH(pViewCamera->GetFovY(), pViewCamera->GetAspectRatio(), fNearZ, fFarZ);

	return pViewCamera->GetViewMatrix() * matProj;
}

void RUniformShadow::UpdateUniformProjection( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ )
{
	_ASSERT(pViewCamera);

	RMatrix matClipedViewProjection = GetClipedViewProjectionMatrix( pViewCamera, fNearZ, fFarZ );

	// receive 할 frustum 을 구한다
	m_vecList.resize(8);
	GetFrustumPoints(&m_vecList[0], matClipedViewProjection);

	UpdateProjection( );
}

void RUniformShadow::SetupForRender()
{
	RPFC_THISFUNC;
	UpdateViewMatrix();
	UpdateProjection();
}

//////////////////////////////////////////////////////////////////////////
// Stable Shadow Map
void RStableShadowMap::SetupForCull( RCameraSceneNode* pViewCamera, const RVector3 &lightDirection, float fNearZ, float fFarZ, bool bReset)
{
	RPFC_THISFUNC;
	float fHalfFovY = 0.5f * pViewCamera->GetFovY();
	float fHalfFovX = 0.5f * pViewCamera->GetFovX();

	// near-z 평면의 대각선 절반길이(nd)를 구한다
	float nx = fNearZ * tanf( fHalfFovX );
	float ny = fNearZ * tanf( fHalfFovY );
	float nd = sqrt ( nx * nx + ny * ny);

	// far-z 평면의 대각선 절반길이(fd)를 구한다
	float fx = fFarZ * tanf( fHalfFovX );
	float fy = fFarZ * tanf( fHalfFovY );
	float fd = sqrt ( fx * fx + fy * fy);

	float h = fFarZ - fNearZ;

	float x = (nd*nd + h*h - fd*fd) / ( 2*h );

	// 중심
	m_boundingSphere.center = pViewCamera->GetWorldPosition() + pViewCamera->GetWorldDirection()* ( fFarZ - x );

	// 반지름
	m_boundingSphere.radius = sqrtf( x*x + fd*fd );

	m_lightDirection = lightDirection;


	float fDistance =0.0f;
	if(bReset)
	{
		UpdateViewTransform( m_boundingSphere.radius );
		RVector3 vNormal = m_boundingSphere.center-m_cameraPosition;
		m_LightXYPlane.SetPlane(vNormal.Normalize(), m_cameraPosition);

		fDistance = m_boundingSphere.radius;
	}
	else
	{
		fDistance =m_LightXYPlane.GetDistanceTo(m_boundingSphere.center);
		UpdateViewTransform( fDistance );
	}

	if(fDistance>=m_boundingSphere.radius)
		UpdateProjectionTransform(0, fDistance + m_boundingSphere.radius);
	else
		UpdateProjectionTransform(fDistance-m_boundingSphere.radius, fDistance + m_boundingSphere.radius);
}

void RStableShadowMap::UpdateViewTransform( float fShiftBack )
{
	m_cameraPosition =m_boundingSphere.center - m_lightDirection * fShiftBack;

	RVector3 vAt;
	if(fShiftBack>=0)
		vAt = m_boundingSphere.center;
	else
		vAt = m_cameraPosition*2 - m_boundingSphere.center;

#ifdef _RH_PROJECTION
	m_outView.SetLookAtMatrixRH(m_cameraPosition, vAt, RVector(0,0,1) );
#else
	m_outView.SetLookAtMatrixLH(m_cameraPosition, vAt, RVector(0,0,1) );
#endif
}

void RStableShadowMap::UpdateProjectionTransform( float fMinZ, float fMaxZ )
{
#ifdef _RH_PROJECTION
	m_outProj.SetOrthoOffCenterRH( -m_boundingSphere.radius, m_boundingSphere.radius, -m_boundingSphere.radius, m_boundingSphere.radius, fMinZ, fMaxZ);
#else
	m_outProj.SetOrthoOffCenterLH( -m_boundingSphere.radius, m_boundingSphere.radius, -m_boundingSphere.radius, m_boundingSphere.radius, fMinZ, fMaxZ);
#endif
}

void RStableShadowMap::AdjustMinMaxZ( float fminz, float fmaxz )
{
	RPFC_THISFUNC;
	UpdateProjectionTransform(fminz, fmaxz);

	// 이동에 안정적인 그림자맵을 얻기위해 texel 크기를 반올림하여 align, 책 참조
	RMatrix matViewProjection = m_outView * m_outProj;
	RVector originShadow(0,0,0);
	matViewProjection.TransformVect( originShadow );
	
	float fHalfShadowMapSize = m_fShadowMapSize / 2;

	RVector originRounded(originShadow);
	originRounded *= fHalfShadowMapSize;
	originRounded.x = floor( originRounded.x + 0.5f);
	originRounded.y = floor( originRounded.y + 0.5f);
	originRounded /= fHalfShadowMapSize;

	RMatrix matRound;
	matRound.SetTranslationMatrix( originRounded.x - originShadow.x , originRounded.y - originShadow.y, 0);

	m_outProj *= matRound;
}

void RStableShadowMap::SetupForRender()
{
	// AdjustMinMaxZ 때 다 해서 할일 없다
}

};