#pragma once
#include "XCameraSceneNodeController.h"

#include "XBaseCameraSceneNode.h"
#include "XCameraGroundTarget.h"
#include "XCameraLock.h"
#include "XCameraInteraction.h"

class XBaseCameraSceneNode;

class XCameraBackViewController : public XBaseGameCamera, public XCameraSceneNodeController
{
protected:
	bool			m_bFreeLook;
	bool			m_bFreeLookLimitDegree;
	float			m_fFreeLookLimitPhi;
	float			m_fFreeLookLimitTolerPhi;
	vec3			m_vObjectDir;
	bool			m_bSyncToTargetDir;

	XCameraGroundTarget m_GroundTarget;
	XCameraLock		m_CameraLock;
	XCameraInteraction m_CameraInteration;

	virtual void			UpdateCameraController(RMatrix& _rMatApplyToCamera) {}

private:
	void CalcMaxPayneCameraZ(float& r, float& theta);
	void _UpdateCamera(float fDelta);

	bool UpdateController( XPolarCoord* pPolarCoord, vec3 tar, vec3 &position, vec3 &eye, vec3 &direction, float fDelta );

	float CalCameraSpeed( float fSpeed )
	{
		// by pok. 
		//return powf(1.3f, fSpeed / 9.0f) * 116.27f;
		return  0.003f * (fSpeed / 50.f);
	}

	vec3 CalcCameraDirection(float fRadius, float fPhi, float fTheta);
	vec3 CalcCameraPosition(vec3& veye, float fRadius, float fPhi, float fTheta);

public:
	XCameraBackViewController(void);
	XCameraBackViewController(RCAMERACONTROLLERTYPE eType, const char* _pSzCameraControllerID, bool _bManagedByCamera);
	virtual ~XCameraBackViewController(void);

	void Init(MUID uidObject);

	void SetDirection(const vec3& dir, const vec3& up=vec3(0.0f, 0.0f, 1.0f));
	vec3 GetObjectDir();

	void SetFreeLook(bool bFree) { m_bFreeLook = bFree; }
	bool IsFreeLook() const { return m_bFreeLook; }
	bool IsCameraLock();

	void UpdateCamera();
	void UpdateCamera(float fDelta, int nDeltaX, int nDeltaY);

	void SetSyncDirToTargetDir(bool b) { m_bSyncToTargetDir = b; }		// 리플레이에서만 사용함

	void LimitDegreeOnFreeLook(float fTolerRad);
	void LimitDegreeOnFreeLook2(float fTolerRad);
	void ReleaseLimitDegreeOnFreeLook();

	//////////////////////////////////////////////////////////////////////////
	// 각 컨트롤 시작, 종료
	void StartGroundTarget();
	void EndGroundTarget();

	void StartCameraLock(MUID& uidTarget, wstring& strBoneName, float& fDurationTime);
	void EndCameraLock();

	void StartInteraction(MUID& uidTarget);
	void EndInteration();
	void ForceEndInteration();

};
