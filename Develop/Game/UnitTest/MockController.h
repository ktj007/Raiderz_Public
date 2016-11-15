#pragma once

#include "XController.h"
#include "XEventID.h"

class MockController : public XController
{
private:
	struct KeyNode
	{
		XVirtualKey nKey;
		float fPressTime;
		float fElapsedTime;
		KeyNode(XVirtualKey key, float pressTime) : nKey(key), fPressTime(pressTime), fElapsedTime(0.0f) {}
	};
	map<XVirtualKey, KeyNode>	m_PressKeyMap;
	vec3	m_vMockBackViewCameraDirection;
	bool	m_bVirtualKeyPressed[VKEY_COUNT];
	void PostEventToMyPlayer(XEventID nEventID, XVirtualKey nVKey);
public:
	MockController();
	virtual ~MockController() {}

	virtual bool Create() { return true; }
	virtual void Destroy() {}

	virtual void OnUpdate(float fDelta);

	virtual bool IsVirtualKeyPressed(XVirtualKey nKey);
	virtual bool IsVirtualKeyPressedAsync(XVirtualKey nKey);

	virtual void SetBackViewCameraDir( const vec3& dir) override;
	virtual vec3 GetBackViewCameraDir() override;
	virtual bool IsMainBackViewCamera() { return true; }

	void TestKeyPress(XVirtualKey nVKey, float fPressTime=0.0f);		// PressTime이 0.0이면 TestKeyRelease를 명시적으로 호출하여야 한다.
	void TestKeyRelease(XVirtualKey nVKey);
	void TestKeyClick(XVirtualKey nVKey);
	void TestSpecialKeyPress(SPECIAL_ACTION_COMMAND nSpecialActionCommand);

	void TestSetCameraDir(vec3 dir);

	void TestGuardAction();
	void TestGuardRelease();

	void TestMouseMove(vec3 vPos);
};
