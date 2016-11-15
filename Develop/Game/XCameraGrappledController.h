#pragma once

#include "XCameraSceneNodeController.h"
#include "XBaseGameCamera.h"

#include "XBaseCameraSceneNode.h"
#include "XInterpolation.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
#define BACKMOVE_FULLTIME			1.0f;
#define BACKMOVE_LENGTH				500.0f;

enum GRAPPLED_CAMERA_STEP
{
	GS_NONE,
	GS_STEP_1,
	GS_STEP_2
};

enum GRAPPLED_STEP
{
	MOVE_BACK,
	MOVE_STOP
};


class XCameraGrappledController : public XBaseGameCamera, public XCameraSceneNodeController
{
private:
	static const int CAMERA_DIST = 200;

	enum _STATE
	{
		NORMAL,
		ZOOM_OUT,
		ZOOM_IN,
		RELEASE
	};
	_STATE m_nState;
	MUID m_uidTarget;
	unsigned int	m_nLastTime;

	float		m_fInitialRadius;
	XLinearInterpolation m_ZoomOutInterpolator;
	XLinearInterpolation m_ZoomInInterpolator;


	void _UpdateCamera(float fDelta);
	float CalCameraSpeed( float fSpeed )
	{
		return powf(1.3f, fSpeed / 9.0f) * 116.27f;
	}
	void Interpolate( float fDelta );
protected:
	virtual void			UpdateCameraController(RMatrix& _rMatApplyToCamera);

public:
	XCameraGrappledController(void);
	virtual ~XCameraGrappledController(void);

	void Init(const MUID& uidTarget);
	void Release();
	void UpdateCamera( float fDelta, int nDeltaX, int nDeltaY );
};
