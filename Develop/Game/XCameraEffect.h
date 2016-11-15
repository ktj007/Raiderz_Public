#ifndef _XCAMERA_EFFECT_H
#define	_XCAMERA_EFFECT_H

#include "RCameraAnimationController.h"
#include "RCameraSceneNode.h"

//////////////////////////////////////////////////////////////////////////
class XCameraEffect
{
private:
	typedef map<wstring, RCameraAnimationController *> CAM_ANI_CONTROL;
	map<wstring, RCameraAnimationController *>		m_mapCameraAnimation;

	wstring											m_strDirPath;
	int												m_nCameraControllerId;
private:
	void											CreateCameraEffect(const wchar_t* szFileName);

	RCameraAnimationController *					GetCameraEffect(const wstring& szAniName);

public:
	XCameraEffect();
	virtual ~XCameraEffect();

	void											LoadCameraEffect(const wchar_t* szDirPath);
	void											DeleteCameraEffect();
	bool											PlayCameraEffect(const wchar_t* szAniName, float _fDuration);
};

#endif // _XCAMERA_EFFECT_H
