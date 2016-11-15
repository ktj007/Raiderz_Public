#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RCameraSceneNode.h"

using namespace rs3;

bool EqualMatrix( const RMatrix &a, const RMatrix &b, float tolerance = TOLER )
{
	for ( int i = 0; i<16; ++i )
		if ( !MMath::Equals(a.m[i], b.m[i], tolerance))
			return false;

	return true;
};

class TestCamera : public NullDeviceFixture
{
};

TEST_F( TestCamera, UpdateViewFrustum )
{
	const float FOV = MMath::PI / 4.0f;
	const float ASPECT_RATIO = 1.0f;	// must 1.0f
	const float NEAR_Z = 1.f;
	const float FAR_Z = 10.f;

	RCameraSceneNode camera;
	camera.SetPosition(0, 0, 0);
	camera.SetDirection(RVector::AXISZ, RVector::AXISY);
	// set fov-x ...
	camera.SetFov(FOV, ASPECT_RATIO);
	camera.SetNearFarZ(NEAR_Z, FAR_Z);
	camera.UpdateViewFrustrum();

	RMatrix viewMatrix, projMatrix;
	
	viewMatrix.MakeIdentity();
	viewMatrix.SetLookAtMatrixRH(RVector::ZERO, RVector::AXISZ, RVector::AXISY);
	projMatrix.MakeIdentity();
	// set fov-y ...
	projMatrix.SetProjectionMatrixFovRH(FOV, ASPECT_RATIO, NEAR_Z, FAR_Z);

	RMatrix viewprojMat = viewMatrix * projMatrix;
	RMatrix viewprojMatFromCamera = camera.GetViewProjectionMatrix();

	EXPECT_TRUE( EqualMatrix(viewprojMat, viewprojMatFromCamera) );
}

TEST_F( TestCamera, ProjectionClipPlaneToggle )
{
	RCameraSceneNode camera;
	camera.SetPosition(0, 0, 0);
	camera.SetDirection(RVector::AXISZ, RVector::AXISY);
	// set fov-x ...
	camera.SetFov(MMath::PI/4.0f, 1.6f);
	camera.SetNearFarZ( 10.f, 10000.f);
	camera.UpdateViewFrustrum();

	RMatrix viewprojMat = camera.GetViewProjectionMatrix();

	RPlane plane;
	plane.SetPlane(camera.GetDirection(), camera.GetDirection()*100.f);

	camera.SetClipPlane(plane);
	camera.SetClipPlaneEnable(true);
	camera.UpdateViewFrustrum();

	// near clip-plane을 변경했으므로, 바뀌어져야 한다.
	EXPECT_NE(viewprojMat._33, camera.GetViewProjectionMatrix()._33);
	EXPECT_NE(viewprojMat._43, camera.GetViewProjectionMatrix()._43);

	camera.SetClipPlaneEnable(false);
	camera.UpdateViewFrustrum();

	// true -> false 일 경우 이전 matrix와 동일한 값
	EXPECT_TRUE( EqualMatrix(viewprojMat, camera.GetViewProjectionMatrix()) );
}