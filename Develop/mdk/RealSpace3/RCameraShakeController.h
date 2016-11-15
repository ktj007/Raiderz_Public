#ifndef _RCAMERA_SHAKE_CONTROLLER_H
#define _RCAMERA_SHAKE_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include <deque>

namespace rs3 {

class RCameraSceneNode;


class RS_API RCameraShakeController : public RCameraSceneNodeController
{
public:

	// position만 움직이며, 조금은 나중에 적용되어도 상관없으므로 id는 0 하나만 가지면 되겠다.
	RCameraShakeController( float _fDuration, float _fAmplitude, float _fFrequency ) : RCameraSceneNodeController(RCCT_PREUPDATE, "0_ShakeController", true)
	{
		m_fElapsedTime_ = 0.f;
		m_y = m_z = 0;

		m_fAmplitude_ = _fAmplitude;
		m_fFrequency_ = _fFrequency;
		m_fDecayTime_ = _fDuration;
	}

protected:

	virtual ~RCameraShakeController(){}
	virtual void UpdateCameraController(RMatrix& _rMatApplyToCamera);

private:

	float m_fAmplitude_;
	float m_fFrequency_;
	float m_fDecayTime_;

	float m_fElapsedTime_;

	float m_y;
	float m_z;
};

}

#endif
