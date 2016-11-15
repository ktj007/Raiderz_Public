#ifndef _XGAME_PLAY_CONTROLLER_H
#define _XGAME_PLAY_CONTROLLER_H

#include "XController.h"

class XGame;


/// 사용자의 입력을 담당하는 클래스
class XGamePlayController : public XController
{
protected:
	bool			m_bFreeLookCamera;
	bool			m_bCameraRotationButtonDown;
	bool			m_bMouseMoveButtonDown;
	bool			m_bMouseClickActivate;

	float			m_fMouseMoveCycleTime;

public:
	XGamePlayController();
	virtual ~XGamePlayController();

	virtual bool Create();

	virtual bool OnEvent( MEvent& e);
	virtual bool OnFoceEvent( MEvent& e);

	virtual void OnUpdate( float fDelta);
	virtual void OnChat( CSMsgType nMsgType, const wstring& strMsg);

	virtual bool CheckEnableKey( XVirtualKey nVKey);					///< 키 사용 가능 여부 확인

	virtual void UpdateCameraAfterGameUpdate(float fDelta);

	virtual void SetBackViewCameraDir( const vec3& dir);
	virtual vec3 GetBackViewCameraDir();
	virtual bool IsMainBackViewCamera();
	virtual bool IsCameraLockView();
	virtual bool IsCameraPvPFreeView();

	virtual void SetFreeLookCamera( bool bFree)		{ m_bFreeLookCamera = bFree;		}

	virtual void LimitDegreeOnFreeLook( float fTolerRad);
	virtual void LimitDegreeOnFreeLook2( float fTolerRad);
	virtual void ReleaseLimitDegreeOnFreeLook();
protected:
	void UpdateCamera( float fDelta);
	void _UpdateCamera( float fDelta, int nDeltaX, int nDeltaY );
	void UpdateMouseMove(float fDelta);
	void UpdateMouseClickActivate();

	void SendEventMouseMoveClick( int x, int y );
};



#endif // _XGAME_PLAY_CONTROLLER_H