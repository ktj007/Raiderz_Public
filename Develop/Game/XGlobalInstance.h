#pragma once

#include "XGlobal.h"
#include "XGlobalVar.h"

class XGlobalInstance
{
private:
	void SetError(const wchar_t* szMessage);
public:
	GlobalVar*				var;
	XSystem*				system;
	XUISystem*				ui;
	XNetwork*				net;
	XEngine*				engine;
	XRealSound*				sound;
	XScript*				script;
	XHistory*				history;
	XCameraManager*			camera;
	XEffectManager*			emgr;
	XResourceManager*		res;
	XBandiCapturer*			bandi_capturer;

	XPhysX*					phx;
	XFeedbackSystem*		feedback;
	XMsgCommandFacade*		msg;

	XGlobalInstance();
	virtual ~XGlobalInstance();
	virtual bool Init(HWND hWnd);
	bool InitForDummyClient(HWND nHwnd);
	virtual void Fini();
};

class XGlobalInfoInstance : public XGlobalInfo
{
private:
	void SetError(wchar_t* szMessage);
public:
	XGlobalInfoInstance();
	virtual ~XGlobalInfoInstance();
	virtual bool Init();
};