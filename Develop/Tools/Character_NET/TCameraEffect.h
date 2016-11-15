#ifndef _TCAMERA_EFFECT_H
#define	_TCAMERA_EFFECT_H

#include "RCameraAnimationController.h"
#include "RCameraSceneNode.h"

//////////////////////////////////////////////////////////////////////////
class TCameraEffect
{
private:
	typedef map<string, RCameraAnimationController *> CAM_ANI_CONTROL;
	map<string, RCameraAnimationController *>		m_mapCameraAnimation;

	string											m_strDirPath;
	int												m_nCameraControllerId;
private:
	void											CreateCameraEffect(const char* szFileName);

	RCameraAnimationController *					GetCameraEffect(const string& szAniName);

public:
	TCameraEffect();
	virtual ~TCameraEffect();

	void											LoadCameraEffect(const char* szDirPath);
	void											DeleteCameraEffect();
	bool											PlayCameraEffect(const char* szAniName, float _fDuration);
};

#endif // _TCAMERA_EFFECT_H
