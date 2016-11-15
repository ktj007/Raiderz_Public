#include "StdAfx.h"

#include "RCameraShakeController.h"
#include "RCameraSceneNode.h"

namespace rs3 {

void RCameraShakeController::UpdateCameraController(RMatrix& _rMatApplyToCamera)
{
	m_fElapsedTime_ += (float)REngine::GetTimer().GetElapsedTime()/1000.f;
	if ( m_fElapsedTime_ < m_fDecayTime_ )
	{
		// W = sin( 2 * 3.14159f * (elapsedsec * freq) + phase ) * amp * (decaysec - t) / decaysec ; 
		m_z += (sin (2.0f * MMath::PI * (m_fElapsedTime_ * m_fFrequency_) + 0) * m_fAmplitude_ + 
			sin (2.0f * MMath::PI * (m_fElapsedTime_ * 7) + 0.2f) * 3.1f + 
			sin (2.0f * MMath::PI * (m_fElapsedTime_ * 15) + 0.5f) * 1.1f) * ( m_fDecayTime_ - m_fElapsedTime_ ) / m_fDecayTime_; 

		m_y += (sin (2.0f * MMath::PI * m_fElapsedTime_ * 3) * (m_fAmplitude_*0.2f) + 
			sin (2.0f * MMath::PI * m_fElapsedTime_ * 7 + 0.2f) * 1.1f + 
			sin (2.0f * MMath::PI * m_fElapsedTime_ * 15 + 0.5f) * 1.1f) * ( m_fDecayTime_ - m_fElapsedTime_) / m_fDecayTime_;

		_rMatApplyToCamera._43 += m_z;
		_rMatApplyToCamera._42 += m_y;
	}
	else
	{
		ReserveToRemove();
	}
}

}