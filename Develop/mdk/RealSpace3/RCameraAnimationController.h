#ifndef _RCAMERA_ANIMATION_CONTROLLER_H
#define _RCAMERA_ANIMATION_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RAnimation.h"
#include "RAnimationResource.h"

namespace rs3 {

class RCameraSceneNode;
class RAnimation;

class RS_API RCameraAnimationController : public RCameraSceneNodeController
{
public:
	RCameraAnimationController( const char* _pSzControllerID, const char* pSzAniFileName, bool bManagedByCamera, bool bResetToOrg = false );
	virtual ~RCameraAnimationController();

	const char *	GetFileName();
	void			ResetTimer(float _fDurationTime);
	bool			IsValidAnimation()	{ return m_pAnimation != NULL; }
	bool			IsRunningInCamera()	{ return m_bCurrentRunning; }
	bool			IsOncePlayed()		{ return m_bOncePlayed; }
	bool			IsLoopType()		{ return m_bLoop; }

protected:

	virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera);

	virtual bool	IsBlendableBeforeController();
	virtual void	OnInsertToCamera( RCameraSceneNodeController* _pBeforeController );
	virtual void	OnRemoveFromCamera();

private:

	bool			m_bCurrentRunning;
	bool			m_bOncePlayed;

	DWORD			m_nFrame;
	DWORD			m_nMaxFrame;
	bool			m_bLoop;
	bool			m_bPlayDone;
	float			m_fSpeed;
	RAnimationResource*		m_pAnimation;

	RQuaternion		m_currentRotation;
	RVector			m_currentTranslation;
	RMatrix			m_matCurrent;

	bool			m_bResetToOrg;
	RMatrix			m_matCameraOrg;

	unsigned int	m_nPosKeyIterator;
	unsigned int	m_nRotKeyIterator;

	float			m_fDurationTime;
	float			m_fElapsedTime;
};

//----------------------------------------------------------------------------------------------------
inline const char* RCameraAnimationController::GetFileName()
{
	if (m_pAnimation)
		return m_pAnimation->GetName().c_str();
	return NULL;
}
//----------------------------------------------------------------------------------------------------
inline void RCameraAnimationController::ResetTimer(float _fDurationTime)
{
	m_fDurationTime = _fDurationTime;
	m_fElapsedTime = 0.f;

	// animation에 duration을 주면, loop로 설정을 바꾸고 duration동안 play한다.
	if (m_fDurationTime > FLT_EPSILON)
		m_bLoop = true;
	else
		m_bLoop = false;
}
//----------------------------------------------------------------------------------------------------
}

#endif
